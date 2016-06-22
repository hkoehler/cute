/***************************************************************************
                          lexer.cpp  -  represents a scintilla lexer
                             -------------------
    begin                : Sam, 22 Feb 2003
    copyright            : (C) 2003 by Heiko Khler
    email                : heicom@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfileinfo.h>
#include <qregexp.h>
#include "lexer.h"

// important characters for defining word chars of lexers
#define ALPHA_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUM_CHARS "0123456789"
#define ACCENTED_CHARS "ÿÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖØøÙùÚúÛûÜüÝýÞþßö"

namespace Config{

// default word characters
static const char *wordChars = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

struct Property
{
	PyObject_HEAD
	const char *font;
	int foregroundColor;
	int backgroundColor;
	int size;
	bool eolfilled;
	bool bold;
	bool underline;
	bool italic;
};

static Lexer *plainLexer;

/** init function for lexer configuration */
static void initPerlLexer();
static void initPHPLexer();
static void initVBScriptLexer();
static void initJavaScriptLexer();
static void initPythonLexer();

static void initCLikeLexer(Lexer *);
static void initCppLexer();
static void initCSharpLexer();
static void initIDLLexer();
static void initJavaLexer();

static void initHTMLLexer();
static void initCSSLexer();
static void initFortranLexer();
static void initXMLLexer();
static void initMakefileLexer();
static void initDiffLexer();
static void initLatexLexer();
static void initSQLLexer();
static void initShellLexer();
static void initAsmLexer();
static void initMatlabLexer();

static PyObject *property_getAttr(PyObject *obj, char *name);
static int property_setAttr(PyObject *obj, char *name, PyObject *v);

static PyTypeObject property_PropertyType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "Property",
    sizeof(Property),
    0,
    0, /*tp_dealloc*/
    0,          /*tp_print*/
    property_getAttr,          /*tp_getattr*/
    property_setAttr,          /*tp_setattr*/
    0,          /*tp_compare*/
    0,          /*tp_repr*/
    0,          /*tp_as_number*/
    0,          /*tp_as_sequence*/
    0,          /*tp_as_mapping*/
    0,          /*tp_hash */
};

static PyObject *property_getAttr(PyObject *obj, char *name)
{
	Property *p = (Property*)obj;
	if(!strcmp(name, "fore"))
		return PyLong_FromLong(p->foregroundColor);
	else if(!strcmp(name, "back"))
		return PyLong_FromLong(p->backgroundColor);
	else if(!strcmp(name, "font"))
		return PyString_FromString(p->font);
	else if(!strcmp(name, "size"))
		return PyLong_FromLong(p->size);
	else if(!strcmp(name, "eolfilled"))
		return PyLong_FromLong(p->eolfilled);
	else if(!strcmp(name, "bold"))
		return PyLong_FromLong(p->bold);
	else if(!strcmp(name, "italic"))
		return PyLong_FromLong(p->italic);
	else if(!strcmp(name, "underline"))
		return PyLong_FromLong(p->underline);
	return NULL;
}

static int property_setAttr(PyObject *obj, char *name, PyObject *v)
{
	Property *p = (Property*)obj;
	if(!strcmp(name, "fore"))
		p->foregroundColor = PyLong_AsLong(v);
	else if(!strcmp(name, "back"))
		p->backgroundColor = PyLong_AsLong(v);
	else if(!strcmp(name, "font"))
		p->font = strdup(PyString_AsString(v));
	else if(!strcmp(name, "size"))
		p->size = PyLong_AsLong(v);
	else if(!strcmp(name, "eolfilled"))
		p->eolfilled = PyLong_AsLong(v);
	else if(!strcmp(name, "bold"))
		p->bold = PyLong_AsLong(v);
	else if(!strcmp(name, "italic"))
		p->italic = PyLong_AsLong(v);
	else if(!strcmp(name, "underline"))
		p->underline = PyLong_AsLong(v);
	else 
		return -1;
	return 0;
}

// static vars of LexerManager
std::map<int, Lexer*> LexerManager::lexers;
std::list<Lexer*> LexerManager::lexer_list;
PyObject *config;

Lexer::Lexer(int scintillaID, char*name)
{
	defaultFont.fromString( Config::view.getString("defaultFont") );
	lexerID = scintillaID;
	useMonospacedFont = false;
	configModule = ConfigModule(lang, name);
	configModule.add("streamCommentStart", "");
	configModule.add("streamCommentEnd", "");
	configModule.add("blockCommentStart", "");
	configModule.add("fileExtensions", "");
	configModule.add("wordCharacters", Config::wordChars);
}

void Lexer::addKeywords(const char *kw)
{
	keywords.push_back(kw);
}

void Lexer::initScintilla(QextScintilla *sc)
{
	PyObject *dict, *key, *value;
	int pos = 0;
	const char* name;
	int style_number;
	long property_fore, property_back,property_size;
	const char* property_font;
	QFont f;
	bool property_eolfilled, property_bold, property_underline, property_italic;

	dict = configModule.dict();

	sc->SendScintilla(QextScintillaBase::SCI_STYLESETFONT,QextScintillaBase::STYLE_DEFAULT,defaultFont.family().latin1());
	sc->SendScintilla(QextScintillaBase::SCI_STYLESETSIZE,QextScintillaBase::STYLE_DEFAULT,defaultFont.pointSize());
	sc->SendScintilla(QextScintillaBase::SCI_STYLESETBOLD,QextScintillaBase::STYLE_DEFAULT,defaultFont.bold());
	sc->SendScintilla(QextScintillaBase::SCI_STYLESETITALIC,QextScintillaBase::STYLE_DEFAULT,defaultFont.italic());
	sc->SendScintilla(QextScintillaBase::SCI_STYLESETUNDERLINE,QextScintillaBase::STYLE_DEFAULT,defaultFont.underline());

	sc->SendScintilla(QextScintillaBase::SCI_STYLECLEARALL);
	sc->SendScintilla(QextScintillaBase::SCI_SETLEXER, lexerID, 0L);
	sc->SendScintilla(QextScintillaBase::SCI_SETSTYLEBITS, 7);
	sc->SendScintilla(QextScintillaBase::SCI_STYLERESETDEFAULT);

	for(int i = 0; i < keywords.size(); i++)
		sc->SendScintilla(QextScintillaBase::SCI_SETKEYWORDS, i, reinterpret_cast<long>(keywords[i]));
	while(PyDict_Next(dict, &pos, &key, &value)){
		name = PyString_AsString(key);
		if(!strcmp(name, "__doc__"))
			continue;
		else if(!strcmp(name, "__name__"))
			continue;
		else if(!strcmp(name, "streamCommentStart"))
			continue;
		else if(!strcmp(name, "streamCommentEnd"))
			continue;
		else if(!strcmp(name, "blockCommentStart"))
			continue;
		else if(!strcmp(name, "fileExtensions"))
			continue;
		else if(!strcmp(name, "wordCharacters")){
			sc->SendScintilla(QextScintillaBase::SCI_SETWORDCHARS, (long unsigned)0, PyString_AsString(value));
			continue;
		}
	
		style_number = property_names[name];
		property_fore = reinterpret_cast<Property*>(value)->foregroundColor;
		property_size = reinterpret_cast<Property*>(value)->size;
		property_font = reinterpret_cast<Property*>(value)->font;
		property_back = reinterpret_cast<Property*>(value)->backgroundColor;
		property_eolfilled = reinterpret_cast<Property*>(value)->eolfilled;
		property_bold = reinterpret_cast<Property*>(value)->bold;
		property_underline = reinterpret_cast<Property*>(value)->underline;
		property_italic = reinterpret_cast<Property*>(value)->italic;

		sc->SendScintilla(QextScintillaBase::SCI_STYLESETBOLD,style_number,property_bold);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETITALIC,style_number,property_italic);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETUNDERLINE,style_number,property_underline);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETEOLFILLED,style_number,property_eolfilled);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETFONT,style_number,property_font);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETFORE, style_number, property_fore);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETBACK, style_number, property_back);
		sc->SendScintilla(QextScintillaBase::SCI_STYLESETSIZE, style_number, property_size);
	}
	if(useMonospacedFont){
		QFont monospacedFont;
		monospacedFont.fromString( Config::view.getString("monospacedFont") );
		for(int i = 0; i <= 127; i++){
			if (i == QextScintillaBase::STYLE_LINENUMBER)
				continue;
			sc->SendScintilla(QextScintillaBase::SCI_STYLESETFONT,i,monospacedFont.family().latin1());
			sc->SendScintilla(QextScintillaBase::SCI_STYLESETSIZE,i,monospacedFont.pointSize());
		}
	}
	
	sc->SendScintilla(QextScintillaBase::SCI_COLOURISE, 0L, -1L);
}

void Lexer::addProperty(std::string name, int scintillaID, int fore, char *font, int size, 
		bool bold, bool italic, bool underline, int back, bool eolfilled)
{
	Property *p = PyObject_New(Property, &property_PropertyType);
	char *name_cpy;

	name_cpy = strdup(name.c_str());
	p->foregroundColor = fore;
	if(font)
		p->font = strdup(font);
	else
		p->font = strdup(defaultFont.family().latin1());
	if(back)
		p->backgroundColor = back;
	else
		p->backgroundColor = 0xFFFFFF;
	if(size)
		p->size = size;
	else
		p->size = defaultFont.pointSize();
	p->bold = bold;
	p->italic = italic;
	p->underline = underline;
	p->eolfilled = eolfilled;
	PyObject *py_object = reinterpret_cast<PyObject*>(p);
	Py_INCREF(py_object);
	configModule.add(name_cpy, py_object);
	property_names[name] = scintillaID;
}

void Lexer::setFileExtensions( QString fileExt )
{
	configModule.setString("fileExtensions", strdup(fileExt.latin1()));
	fileExtensions = QStringList::split(QRegExp("\\s"), fileExt);
}

void Lexer::setStreamComment( QString start, QString end )
{
	configModule.setString("streamCommentStart", strdup(start.latin1()));
	configModule.setString("streamCommentEnd", strdup(end.latin1()));
}

void Lexer::setWordChars( QString wc )
{
	configModule.setString("wordCharacters", strdup(wc.latin1()));
}

void Lexer::setBlockComment( QString start )
{
	configModule.setString("blockCommentStart", strdup(start.latin1()));
}

QString Lexer::streamCommentStart()
{
	return configModule.getString("streamCommentStart");
}

QString Lexer::streamCommentEnd()
{
	return configModule.getString("streamCommentEnd");
}

QString Lexer::blockCommentStart()
{
	return configModule.getString("blockCommentStart");
}

QString Lexer::wordChars()
{
	return configModule.getString("wordCharacters");
}

void Lexer::setUseMonospacedFont(bool b)
{
	useMonospacedFont = b;
}

Lexer *LexerManager::lexer(int lexerID)
{
	return lexers[lexerID];
}

/* choose lexer */
Lexer *LexerManager::lexer(const char *filename)
{
	std::list<Lexer*>::iterator iter = lexer_list.begin();
	QFileInfo file_info(filename);
	QString ext = file_info.extension(false);
	QStringList fileExtensions;

	for(; iter != lexer_list.end(); ++iter){
		fileExtensions = (*iter)->fileExtensions;
		if( fileExtensions.contains(ext) )
			return *iter;
		if( fileExtensions.contains(filename) )
			return *iter;
	}
	return plainLexer;
}

void LexerManager::addLexer(Lexer* lexer)
{
	lexers[lexer->lexerID] = lexer;
	lexer_list.push_back( lexer );
}

Lexer *LexerManager::lexerByLang(const char* name)
{
	std::list<Lexer*>::iterator i = lexer_list.begin();
	for(; i != lexer_list.end(); ++i){
		if( !strcmp((*i)->lang_name, name))
			return *i;
	}
	return NULL;
}

std::list<std::string> LexerManager::langStringList()
{
	std::list<std::string> str_list;

	std::list<Lexer*>::iterator i = lexer_list.begin();
	for(; i != lexer_list.end(); ++i){
		str_list.push_back(std::string((*i)->lang_name));
	}
	return str_list;
}

void LexerManager::setUseMonospacedFont(bool b)
{
	std::list<Lexer*>::iterator i = lexer_list.begin();
	for(; i != lexer_list.end(); ++i){
		(*i)->setUseMonospacedFont(b);
	}
}

// some default color styles
int keywordColor = 0x7F0000;
int errorColor = 0xFFFF00;
int commentBoxColor = 0x3f7f3f;
int commentLineColor = 0x3f7f3f;
int commentDocColor = 0x3F703F;
int numberColor = 0x7F7F00;
int stringColor = 0x7F007F;
int characterColor = 0x7F007F;
int operatorColor = 0x000000;
int doubleQuotedStringColor = 0x800080;
int singleQuotedStringColor = 0x800080;
int backTicksColor = 0xFFFF00;
int regexColor = 0xFF0000;
int arrayColor = 0x000000;
int preprocColor = 0x007F7F;

static const char* php_keywords = "and argv as argc break case cfunction class continue declare default do "
	"die echo else elseif empty enddeclare endfor endforeach endif endswitch "
	"endwhile e_all e_parse e_error e_warning eval exit extends false for "
	"foreach function global http_cookie_vars http_get_vars http_post_vars "
	"http_post_files http_env_vars http_server_vars if include include_once "
	"list new not null old_function or parent php_os php_self php_version "
	"print require require_once return static switch stdclass this true var "
	"xor virtual while __file__ __line__ __sleep __wakeup";

static const char *java_script_keywords = "abstract boolean break byte case catch char class "
	"const continue debugger default delete do double else enum export extends "
	"final finally float for function goto if implements import in instanceof "
	"int interface long native new package private protected public "
	"return short static super switch synchronized this throw throws "
	"transient try typeof var void volatile while with";

static const char* python_keywords = "and assert break class continue def del elif else except exec "
	"finally for from global if import in is lambda None not or "
	"pass print raise return try while yield";

static const char* vb_script_keywords = "and begin case call continue do each else elseif end erase "
	"error event exit false for function get gosub goto if implement in load loop lset "
	"me mid new next not nothing on or property raiseevent rem resume return rset "
	"select set stop sub then to true unload until wend while with withevents "
	"attribute alias as boolean byref byte byval const compare currency date declare dim double "
	"enum explicit friend global integer let lib long module object option optional "
	"preserve private property public redim single static string type variant";

static void initPlainLexer()
{
	static const char* plain_file_ext = "txt";
	static const char* name = "Plain";

	plainLexer = new Lexer(QextScintilla::SCLEX_NULL, "plain");
	plainLexer->setLangName(name);
	plainLexer->addProperty("default", 0, 0, "courier");
	plainLexer->addProperty("matchedOperators2",35 , 0x0000FF, 0, 0, true);
	plainLexer->addProperty("matchedOperators1",34 , 0xFF0000, 0, 0, true);
	plainLexer->setFileExtensions(plain_file_ext);
	LexerManager::addLexer(plainLexer);
}

static void initCLikeLexer(Lexer *lexer)
{
	lexer->addProperty("default", 32, 0x808080L);
	lexer->addProperty("whiteSpace", 0, 0x808080L);
	lexer->addProperty("comment", 1, commentBoxColor, "new century schoolbook", 9);
	lexer->addProperty("commentLine", 2, commentLineColor, "new century schoolbook", 9);
	lexer->addProperty("commentDoc", 3, commentDocColor, "new century schoolbook", 9);
	lexer->addProperty("number", 4, numberColor);
	lexer->addProperty("keyword", 5, keywordColor, "lucidatypewriter", 9, true);
	lexer->addProperty("doubleQuotedString", 6, doubleQuotedStringColor, 0);
	lexer->addProperty("singleQuotedString", 7, singleQuotedStringColor, 0);
	lexer->addProperty("UUIDs", 8, 804080, 0);
	lexer->addProperty("operators", 10, operatorColor, "lucidatypewriter", 9, true);
	lexer->addProperty("identifiers", 11, 0x000000, 0);
	lexer->addProperty("EOLStringNotClosed", 12, 0, 0, 0, 0, 0, 0, 0xE0C0E0, true);
	lexer->addProperty("verbatimStringsCSharp", 13, 0x007F00, 0, 0, 0, 0, 0, 0xE0FFE0, true);
	lexer->addProperty("RegExpJavaScript", 14, 0x3F7F3F, 0 , 0, 0, 0, 0, 0xE0F0FF, true);
	lexer->addProperty("docCommentLine", 15, commentDocColor, "new century schoolbook", 9);
	lexer->addProperty("keyword2", 16, 0xB00040, 0, 0, true);
	lexer->addProperty("commentKeyword", 17, 0x3060A0, 0);
	lexer->addProperty("commentKeywordError", 18, 0x804020, 0);
	lexer->addProperty("matchedOperators2",35 , 0x0000FF, "lucidatypewriter", 9, true);
	lexer->addProperty("matchedOperators1",34 , 0xFF0000, "lucidatypewriter", 9, true);
	
	lexer->setBlockComment("//");
	lexer->setStreamComment("/*", "*/");
	lexer->setWordChars( NUM_CHARS ALPHA_CHARS "#_" );
}

static void initPerlLexer()
{
	QString perl_file_ext = "pl pm awk";
	static const char perl_keywords[] = "NULL __FILE__ __LINE__ __PACKAGE__ __DATA__ __END__ AUTOLOAD "
		"BEGIN CORE DESTROY END EQ GE GT INIT LE LT NE CHECK abs accept "
		"alarm and atan2 bind binmode bless caller chdir chmod chomp chop "
		"chown chr chroot close closedir cmp connect continue cos crypt "
		"dbmclose dbmopen defined delete die do dump each else elsif endgrent "
		"endhostent endnetent endprotoent endpwent endservent eof eq eval "
		"exec exists exit exp fcntl fileno flock for foreach fork format "
		"formline ge getc getgrent getgrgid getgrnam gethostbyaddr gethostbyname "
		"gethostent getlogin getnetbyaddr getnetbyname getnetent getpeername "
		"getpgrp getppid getpriority getprotobyname getprotobynumber getprotoent "
		"getpwent getpwnam getpwuid getservbyname getservbyport getservent "
		"getsockname getsockopt glob gmtime goto grep gt hex if index "
		"int ioctl join keys kill last lc lcfirst le length link listen "
		"local localtime lock log lstat lt m map mkdir msgctl msgget msgrcv "
		"msgsnd my ne next no not oct open opendir or ord our pack package "
		"pipe pop pos print printf prototype push q qq qr quotemeta qu "
		"qw qx rand read readdir readline readlink readpipe recv redo "
		"ref rename require reset resturn reverse rewinddir rindex rmdir "
		"s scalar seek seekdir select emctl semget semop send setgrent "
		"sethostent setnetent setpgrp setpriority setprotoent setpwent "
		"setservent setsockopt shift shmctl shmget shmreantf sqrt srand "
		"stat study sub substr symlink syscall sysopen sysreadd shmwrite shutdown "
		"sin sleep socket socketpair sort splice split spri sysseek "
		"system syswrite tell telldir tie tied time times tr truncate "
		"uc ucfirst umask undef unless unlink unpack unshift untie until "
		"use utime values vec wait waitpid wantarray warn while write "
		"x xor y";
	static const char* name = "Perl";

	Lexer *perlLexer = new Lexer(QextScintilla::SCLEX_PERL, "perl");
	perlLexer->setLangName(name);
	perlLexer->setFileExtensions(perl_file_ext);
	perlLexer->setBlockComment("#");
	perlLexer->addProperty("default", 32, 0x808080L, "courier");
	perlLexer->addProperty("error", 1, errorColor);
	perlLexer->addProperty("commentLine", 2, commentLineColor, "new century schoolbook", 9);
	perlLexer->addProperty("pod", 3, 0x004000L);
	perlLexer->addProperty("number", 4, numberColor);
	perlLexer->addProperty("keyword", 5, keywordColor, "lucidatypewriter", 9, true);
	perlLexer->addProperty("string", 6, stringColor);
	perlLexer->addProperty("character", 7, characterColor);
	perlLexer->addProperty("punctuation", 8, 0x0L);
	perlLexer->addProperty("preprocessor", 9, 0x0L);
	perlLexer->addProperty("operator", 10, operatorColor, "lucidatypewriter", 9, true);
	perlLexer->addProperty("identifier", 11, 0x00007FL);
	perlLexer->addProperty("scalar", 12, 0x0L);
	perlLexer->addProperty("array", 13, 0x0L);
	perlLexer->addProperty("hash", 14, 0x0L);
	perlLexer->addProperty("symbolTable", 15, 0x0L);
	perlLexer->addProperty("regex", 17, 0x0L);
	perlLexer->addProperty("regSubst", 18, 0x0L);
	perlLexer->addProperty("longQuote", 19, 0xFF0F00L);
	perlLexer->addProperty("backTicks", 20, 0x0000FFL);
	perlLexer->addProperty("dataSection", 21, 0x600000L);
	perlLexer->addProperty("hereDelim", 22, 0x000000L);
	perlLexer->addProperty("hereQ", 23, 0x7F007FL);
	perlLexer->addProperty("hereQQ", 24, 0x7F007FL);
	perlLexer->addProperty("hereQX", 25, 0x7F007FL);
	perlLexer->addProperty("stringQ", 26, 0xF007FL);
	perlLexer->addProperty("stringQQ", 27, doubleQuotedStringColor);
	perlLexer->addProperty("stringQX", 28, backTicksColor);
	perlLexer->addProperty("stringQR", 29, regexColor);
	perlLexer->addProperty("stringQW", 30, arrayColor);
	perlLexer->addProperty("matchedOperators2",35 , 0x0000FF, "lucidatypewriter", 9, true);
	perlLexer->addProperty("matchedOperators1",34 , 0xFF0000, "lucidatypewriter", 9, true);
	perlLexer->addKeywords(perl_keywords);
	LexerManager::addLexer(perlLexer);
}

void initCppLexer()
{
	static const char *cpp_keywords = "asm auto bool break case catch char class const const_cast continue \
		default delete do double dynamic_cast else enum explicit export extern false float for \
		friend goto if inline int long mutable namespace new operator private protected public \
		register reinterpret_cast return short signed sizeof static static_cast struct switch \
		template this throw true try typedef typeid typename union unsigned using \
		virtual void volatile wchar_t while";
	static const char *doxygen_keywords = "a addindex addtogroup anchor arg attention" 
		"author b brief bug c class code date def defgroup deprecated dontinclude "
		"e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception "
		"f$ f[ f] file fn hideinitializer htmlinclude htmlonly "
		"if image include ingroup internal invariant interface latexonly li line link "
		"mainpage name namespace nosubgrouping note overload "
		"p page par param post pre ref relates remarks return retval "
		"sa section see showinitializer since skip skipline struct subsection "
		"test throw todo typedef union until "
		"var verbatim verbinclude version warning weakgroup $ @ \\ & < > # { }";
	static const char *user_keywords = "connect signals slots SIGNAL SLOT PROPERTY";
	static const char* cpp_file_ext = "cpp cc c C h hxx hpp cxx CXX H xpm rc";
	static const char *name = "C++";
	
	Lexer *cppLexer = new Lexer(QextScintilla::SCLEX_CPP, "cpp");
	cppLexer->setLangName(name);
	cppLexer->setFileExtensions(cpp_file_ext);
	initCLikeLexer(cppLexer);
	cppLexer->addProperty("preprocessor", 9, preprocColor);
	cppLexer->addKeywords(cpp_keywords);
	cppLexer->addKeywords(user_keywords);
	cppLexer->addKeywords(doxygen_keywords);
	LexerManager::addLexer(cppLexer);
}

void initCSharpLexer()
{
	static const char* cs_file_ext = "cs";
	static const char* csharp_keywords = "abstract as base bool break byte case catch char checked class \
		const continue decimal default delegate do double else enum \
		event explicit extern false finally fixed float for foreach goto if \
		implicit in int interface internal is lock long namespace new null \
		object operator out override params private protected public \
		readonly ref return sbyte sealed short sizeof stackalloc static \
		string struct switch this throw true try typeof uint ulong \
		unchecked unsafe ushort using virtual void while";
	static const char* name = "C#";

	Lexer *cSharpLexer = new Lexer(QextScintilla::SCLEX_CPP, "csharp");
	cSharpLexer->setLangName(name);
	cSharpLexer->setFileExtensions(cs_file_ext);
	initCLikeLexer(cSharpLexer);
	cSharpLexer->addKeywords(csharp_keywords);
	LexerManager::addLexer(cSharpLexer);
}

void initJavaLexer()
{
	static const char* java_file_ext = "java";
	static const char* java_keywords = "abstract assert boolean break byte case catch char class const \
		continue default do double else extends final finally float for future \
		generic goto if implements import inner instanceof int interface long \
		native new null operator outer package private protected public rest \
		return short static super switch synchronized this throw throws \
		transient try var void volatile while";
	static const char* name = "Java";
	
	Lexer *javaLexer = new Lexer(QextScintilla::SCLEX_CPP, "java");
	javaLexer->setLangName(name);
	javaLexer->setFileExtensions(java_file_ext);
	initCLikeLexer(javaLexer);
	javaLexer->addKeywords(java_keywords);
	LexerManager::addLexer(javaLexer);
}

void initJavaScriptLexer()
{
	static const char *java_script_file_ext = "js qs";
	static const char *name = "Java Script";

	Lexer *javaScriptLexer = new Lexer(QextScintilla::SCLEX_CPP, "javascript");
	javaScriptLexer->setLangName(name);
	javaScriptLexer->setFileExtensions(java_script_file_ext);
	initCLikeLexer(javaScriptLexer);
	javaScriptLexer->addKeywords(java_script_keywords);
	LexerManager::addLexer(javaScriptLexer);
}

void initPythonLexer()
{
	static const char* py_file_ext = "py python";
	static const char* name = "Python";

	Lexer *pythonLexer = new Lexer(QextScintilla::SCLEX_PYTHON, "python");
	pythonLexer->setLangName(name);
	pythonLexer->setFileExtensions(py_file_ext);
	pythonLexer->setBlockComment("#");
	pythonLexer->addProperty("whiteSpace", 0, 0x808080, "courier");
	pythonLexer->addProperty("comment", 1, commentLineColor);
	pythonLexer->addProperty("number", 2, numberColor);
	pythonLexer->addProperty("string", 3, stringColor);
	pythonLexer->addProperty("singleQuotedString", 4, singleQuotedStringColor);
	pythonLexer->addProperty("keyword", 5, keywordColor, "lucidatypewriter", 9, true);
	pythonLexer->addProperty("tripleQuote", 6, 0x00007F);
	pythonLexer->addProperty("tripleDoubleQuote", 7, 0x00007F);
	pythonLexer->addProperty("classes", 8, 0x0000FF, 0, 0, true);
	pythonLexer->addProperty("method", 9, 0x007F7F, 0, 0, true);
	pythonLexer->addProperty("operator", 10, operatorColor, "lucidatypewriter", 9, true);
	pythonLexer->addProperty("identifiers", 11, 0x000000);
	pythonLexer->addProperty("commentBlock", 12, commentLineColor);
	pythonLexer->addProperty("EOLStringNotClosed",13 , 0x000000, 0, 0, 0, 0, 0, 0xE0C0E0, true);
	pythonLexer->addProperty("matchedOperators2",35 , 0x0000FF, "lucidatypewriter", 9, true);
	pythonLexer->addProperty("matchedOperators1",34 , 0xFF0000, "lucidatypewriter", 9, true);
	pythonLexer->addKeywords(python_keywords);
	LexerManager::addLexer(pythonLexer);
}

void initHTMLLexer()
{
	static const char* html_file_ext = "html htm asp shtml htd htt cfm";
	static const char* name = "HTML";
	static const char* html_keywords = "a abbr acronym address applet area "
		"b base basefont bdo big blockquote body br button "
		"caption center cite code col colgroup "
		"dd del dfn dir div dl dt "
		"em "
		"fieldset font form frame frameset "
		"h1 h2 h3 h4 h5 h6 head hr html "
		"i iframe img input ins isindex "
		"kbd "
		"label legend li link "
		"map menu meta "
		"noframes noscript "
		"object ol optgroup option "
		"p param pre "
		"q "
		"s samp script select small span strike strong style "
		"sub sup "
		"table tbody td textarea tfoot th thead title tr tt "
		"u ul "
		"var "
		"xml xmlns "
		"abbr accept-charset accept accesskey action align "
		"alink alt archive axis "
		"background bgcolor border "
		"cellpadding cellspacing char charoff charset checked "
		"cite class classid clear codebase codetype color "
		"cols colspan compact content coords "
		"data datafld dataformatas datapagesize datasrc "
		"datetime declare defer dir disabled "
		"enctype event "
		"face for frame frameborder "
		"headers height href hreflang hspace http-equiv "
		"id ismap label lang language leftmargin link "
		"longdesc "
		"marginwidth marginheight maxlength media method "
		"multiple "
		"name nohref noresize noshade nowrap "
		"object onblur onchange onclick ondblclick onfocus "
		"onkeydown onkeypress onkeyup onload onmousedown "
		"onmousemove onmouseover onmouseout onmouseup onreset "
		"onselect onsubmit onunload "
		"profile prompt "
		"readonly rel rev rows rowspan rules "
		"scheme scope selected shape size span src standby "
		"start style summary "
		"tabindex target text title topmargin type "
		"usemap "
		"valign value valuetype version vlink vspace "
		"width "
		"text password checkbox radio submit reset file "
		"hidden image "
		"public !doctype";
	static const char* vxml_elements = "assign audio block break catch choice clear disconnect else elseif \
		emphasis enumerate error exit field filled form goto grammar help \
		if initial link log menu meta noinput nomatch object option p paragraph \
		param phoneme prompt property prosody record reprompt return s say-as \
		script sentence subdialog submit throw transfer value var voice vxml";
	static const char* vxml_attributes = "accept age alphabet anchor application base beep bridge category charset \
		classid cond connecttimeout content contour count dest destexpr dtmf dtmfterm \
		duration enctype event eventexpr expr expritem fetchtimeout finalsilence \
		gender http-equiv id level maxage maxstale maxtime message messageexpr \
		method mime modal mode name namelist next nextitem ph pitch range rate \
		scope size sizeexpr skiplist slot src srcexpr sub time timeexpr timeout \
		transferaudio type value variant version volume xml:lang";

	Lexer *htmlLexer = new Lexer(QextScintilla::SCLEX_HTML, "HTML");
	htmlLexer->setLangName(name);
	htmlLexer->setFileExtensions(html_file_ext);
	htmlLexer->setStreamComment("<!--", "-->");
	htmlLexer->setWordChars(NUM_CHARS ALPHA_CHARS "-_");

	htmlLexer->addProperty("text", 0, 0x000000, "courier");
	htmlLexer->addProperty("tag", 1, 0x000080, 0, 0, true);
	htmlLexer->addProperty("unknownTag", 2, 0xFF0000);
	htmlLexer->addProperty("attributes", 3, 0x008080);
	htmlLexer->addProperty("unknownAttributes", 4, 0xFF0000);
	htmlLexer->addProperty("number", 5, numberColor);
	htmlLexer->addProperty("doubleQuotedString", 6, doubleQuotedStringColor);
	htmlLexer->addProperty("singleQuotedString", 7, singleQuotedStringColor);
	htmlLexer->addProperty("insideTag", 8, 0x800080);
	htmlLexer->addProperty("comment", 9, 0x808000, "new century schoolbook", 9);
	htmlLexer->addProperty("entities", 10, 0x800080);
	htmlLexer->addProperty("styleTagEnd", 11, 0x000080);
	htmlLexer->addProperty("identifierStart", 12, 0x0000FF);
	htmlLexer->addProperty("identifierEnd", 13, 0x0000FF);
	htmlLexer->addProperty("script", 14, 0x000080);
	htmlLexer->addProperty("asp1", 15, 0xFFFF00);
	htmlLexer->addProperty("asp2", 16, 0xFFDF00);
	htmlLexer->addProperty("cdata", 17, 0xFFDF00);
	htmlLexer->addProperty("php", 18, 0x0000FF, 0, 0, 0, 0, 0, 0xFFEFBF);
	htmlLexer->addProperty("unquotedValue", 19, 0xFF00FF, 0, 0, 0, 0, 0, 0xFFEFFF);

	htmlLexer->addProperty("sgmlTag", 21, 0x000080, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlCommand", 22, 0x000080, 0, 0, true, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlFirstParam", 23, 0x006600, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlDoubleString", 24, 0x800000, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlSingleString", 25, 0x993300, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlError", 26, 0x800000, 0, 0, 0, 0, 0, 0xFF6666);
	htmlLexer->addProperty("sgmlSpecial", 27, 0x3366FF, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlEntity", 28, 0x333333, 0, 0, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlComment", 29, 0x808000, "new century schoolbook", 9, 0, 0, 0, 0xEFEFFF);
	htmlLexer->addProperty("sgmlBlock", 31, 0x000066, 0, 0, 0, 0, 0, 0xCCCCE0);

	htmlLexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	htmlLexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);

	htmlLexer->addProperty("javaScriptStart", 40, 0x7F7F00, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptDefault", 41, 0x000000, 0, 0, true, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("javaScriptComment", 42, commentBoxColor, "new century schoolbook", 9, 0, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("javaScriptLineComment", 43, commentLineColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptDocComment", 44, commentDocColor, 0, 0, 0, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("javaScriptNumber", 45, numberColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptWord", 46, 0x000000, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptKeyword", 47, keywordColor, "lucidatypewriter", 0, true, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptDoubleQuotedString", 48, doubleQuotedStringColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptSingleQuotedString", 49, singleQuotedStringColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptSymbols", 50, operatorColor, 0, 0, true, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("javaScriptEOL", 51, 0x000000, 0, 0, 0, 0, 0, 0xBFBBB0, true);
	htmlLexer->addProperty("javaScriptRegularExpression", 52, 0x000000, 0, 0, 0, 0, 0, 0xFFBBB0);

	htmlLexer->addProperty("ASPJavaScriptStart", 55, 0x7F7F00, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptDefault", 56, 0x000000, 0, 0, true, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("ASPJavaScriptComment", 57, commentBoxColor, "new century schoolbook", 9, 0, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("ASPJavaScriptLineComment", 58, commentLineColor, "new century schoolbook", 9, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptDocComment", 59, commentDocColor, "new century schoolbook", 9, 0, 0, 0, 0xE0C0E0, true);
	htmlLexer->addProperty("ASPJavaScriptNumber", 60, numberColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptWord", 61, 0x000000, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptKeyword", 62, keywordColor, 0, 0, true, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptDoubleQuotedString", 63, doubleQuotedStringColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptSingleQuotedString", 64, singleQuotedStringColor, 0, 0, 0, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptSymbols", 65, operatorColor, 0, 0, true, 0, 0, 0xE0C0E0);
	htmlLexer->addProperty("ASPJavaScriptEOL", 66, 0x000000, 0, 0, 0, 0, 0, 0xBFBBB0, true);
	htmlLexer->addProperty("ASPJavaScriptRegularExpression", 67, 0x000000, 0, 0, 0, 0, 0, 0xFFBBB0);

	htmlLexer->addProperty("VBScriptStart", 70, 0x000000, 0, 0, 0, 0, 0, 0x000000);
	htmlLexer->addProperty("VBScriptDefault", 71, 0x000000, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptComment", 72, 0x008000, "new century schoolbook", 9, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptNumber", 73, 0x008080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptKeyword", 74, 0x000080, "lucidatypewriter", 0, true, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptString", 75, 0x800080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptIdentifier", 76, 0x000080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("VBScriptUnterminatedString", 77, 0x000080, 0, 0, 0, 0, 0, 0x7F7FFF, true);

	htmlLexer->addProperty("ASPVBScriptStart", 80, 0x000000, 0, 0, 0, 0, 0, 0x000000);
	htmlLexer->addProperty("ASPVBScriptDefault", 81, 0x000000, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptComment", 82, 0x008000, "new century schoolbook", 9, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptNumber", 83, 0x008080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptKeyword", 84, 0x000080, "lucidatypewriter", 0, true, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptString", 85, 0x800080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptIdentifier", 86, 0x000080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	htmlLexer->addProperty("ASPVBScriptUnterminatedString", 87, 0x000080, 0, 0, 0, 0, 0, 0x7F7FFF, true);

	htmlLexer->addProperty("pythonStart", 90, 0x808080, 0, 0, 0, 0, 0, 0x000000);
	htmlLexer->addProperty("pythonDefault", 91, 0x808080, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonComment", 92, 0x007F00, "new century schoolbook", 9, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonNumber", 93, 0x007F7F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonString", 94, 0x7F007F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonSingleQuotedString", 95, 0x7F007F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonKeyword", 96, 0x00007F, "lucidatypewriter", 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonTripleQuote", 97, 0x7F0000, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonTripleDoubleQuote", 98, 0x7F0000, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonClass", 99, 0x0000FF, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonFunction", 100, 0x007F7F, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonOperator", 101, 0x000000, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("pythonIdentifier", 102, 0x000000, 0, 0, 0, 0, 0, 0xEFFFEF, true);

	htmlLexer->addProperty("ASPPythonStart", 105, 0x808080, 0, 0, 0, 0, 0, 0x000000);
	htmlLexer->addProperty("ASPPythonDefault", 106, 0x808080, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonComment", 107, 0x007F00, "new century schoolbook", 9, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonNumber", 108, 0x007F7F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonString", 109, 0x7F007F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonSingleQuotedString", 110, 0x7F007F, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonKeyword",111, 0x00007F, "lucidatypewriter", 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonTripleQuote", 112, 0x7F0000, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonTripleDoubleQuote", 113, 0x7F0000, 0, 0, 0, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonClass", 114, 0x0000FF, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonFunction", 115, 0x007F7F, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonOperator", 116, 0x000000, 0, 0, true, 0, 0, 0xEFFFEF, true);
	htmlLexer->addProperty("ASPPythonIdentifier", 117, 0x000000, 0, 0, 0, 0, 0, 0xEFFFEF, true);

	htmlLexer->addProperty("PHPDefault", 118, 0x000033, 0, 0, 0, 0, 0, 0xFFF8F8, true);
	htmlLexer->addProperty("PHPDoubleQuotedString", 119, doubleQuotedStringColor, 0, 0, 0, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPSingleQuotedString", 120, singleQuotedStringColor, 0, 0, 0, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPKeyword", 121, keywordColor, "lucidatypewriter", 0, true, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPNumber", 122, numberColor, 0, 0, 0, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPVariable", 123, 0x00007F, 0, 0, true, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPComment", 124, commentBoxColor, "new century schoolbook", 9, 0, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPLineComment", 125, commentLineColor, "new century schoolbook", 9, 0, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPVariableInDoubleQuotedString", 126, 0x00007F, 0, 0, true, 0, 0, 0xFFF8F8);
	htmlLexer->addProperty("PHPOperator", 127, operatorColor, 0, 0, 0, 0, 0, 0xFFF8F8);

	htmlLexer->addKeywords(html_keywords);
	htmlLexer->addKeywords(java_script_keywords);
	htmlLexer->addKeywords(vb_script_keywords);
	htmlLexer->addKeywords(python_keywords);
	htmlLexer->addKeywords(php_keywords);
	htmlLexer->addKeywords("ELEMENT DOCTYPE ATTLIST ENTITY NOTATION");
	LexerManager::addLexer(htmlLexer);
}

void initCSSLexer()
{
	static const char* css_file_ext = "css";
	static const char* name = "CSS";
	static const char* keywords1 = "font-family font-style font-variant font-weight font-size font "
		"color background-color background-image background-repeat background-attachment background-position background "
		"word-spacing letter-spacing text-decoration vertical-align text-transform text-align text-indent line-height "
		"margin-top margin-right margin-bottom margin-left margin "
		"padding-top padding-right padding-bottom padding-left padding "
		"border-top-width border-right-width border-bottom-width border-left-width border-width "
		"border-top border-right border-bottom border-left border "
		"border-color border-style width height float clear "
		"display white-space list-style-type list-style-image list-style-position list-style";
	static const char* keywords2 = "first-letter first-line active link visited";

	Lexer *lexer = new Lexer(QextScintilla::SCLEX_CSS, "css");
	lexer->setLangName(name);
	lexer->setFileExtensions(css_file_ext);
	lexer->setWordChars(ALPHA_CHARS NUM_CHARS "-_");
	
	lexer->addProperty("default", 0, 0x330000, 0, 0, 0, 0, 0, 0x000000, true);
	lexer->addProperty("tag", 1, 0x000080, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("CSSClass", 2, 0x000080, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("pseudoClass", 3, 0x000080, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("unknownPseudoClass", 4, 0x0000FF, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("operator", 5, 0x800000, 0, 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("identifier", 6, 0x000000, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("unknownIdentifier", 7, 0x0000FF, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("value", 8, 0x800080, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("comment", 9, 0x007F00, "new century schoolbook", 9, 0, 0, 0, 0x000000, false);
	lexer->addProperty("ID", 10, 0x000080, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("important", 11, 0xFF0000, 0, 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("directive", 12, 0x008000, 0, 0, true, 0, 0, 0x000000, false);

	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	
	lexer->addKeywords(keywords1);
	lexer->addKeywords("");
	lexer->addKeywords(keywords2);
	LexerManager::addLexer(lexer);
}

void initFortranLexer()
{
	static const char* file_ext = "f for f90 f95 f77 fortran FOR";
	static const char* name = "Fortran";
	static const char* keywords1 = "allocatable allocate assignment backspace block "
		"blockdata call case character close common complex contains continue "
		"cycle data deallocate default dimension direct do double doubleprecision "
		"elemental else elseif elsewhere end endblock endblockdata enddo endfile "
		"endforall endfunction endif endinterface endmodule endprogram endselect "
		"endsubroutine endtype endwhere entry equivalence err exist exit external forall "
		"format formatted function go goto if implicit in inout include "
		"inquire integer intent interface intrinsic iolength iostat kind "
		"len logical module namelist none null nullify only open operator optional "
		"parameter pointer position precision print private procedure program public pure "
		"out read readwrite real rec recursive result return rewind save select selectcase "
		"sequence sequential stat status "
		"stop subroutine target then to type unformatted unit use where while write";

	static const char* keywords2 = "abs achar acos acosd adjustl adjustr "
		"aimag aimax0 aimin0 aint ajmax0 ajmin0 akmax0 akmin0 all allocated alog "
		"alog10 amax0 amax1 amin0 amin1 amod anint any asin asind associated "
		"atan atan2 atan2d atand bitest bitl bitlr bitrl bjtest bit_size bktest break "
		"btest cabs ccos cdabs cdcos cdexp cdlog cdsin cdsqrt ceiling cexp char "
		"clog cmplx conjg cos cosd cosh count cpu_time cshift csin csqrt dabs "
		"dacos dacosd dasin dasind datan datan2 datan2d datand date "
		"date_and_time dble dcmplx dconjg dcos dcosd dcosh dcotan ddim dexp "
		"dfloat dflotk dfloti dflotj digits dim dimag dint dlog dlog10 dmax1 dmin1 "
		"dmod dnint dot_product dprod dreal dsign dsin dsind dsinh dsqrt dtan dtand "
		"dtanh eoshift epsilon errsns exp exponent float floati floatj floatk floor fraction "
		"free huge iabs iachar iand ibclr ibits ibset ichar idate idim idint idnint ieor ifix "
		"iiabs iiand iibclr iibits iibset iidim iidint iidnnt iieor iifix iint iior iiqint iiqnnt iishft "
		"iishftc iisign ilen imax0 imax1 imin0 imin1 imod index inint inot int int1 int2 int4 "
		"int8 iqint iqnint ior ishft ishftc isign isnan izext jiand jibclr jibits jibset jidim jidint "
		"jidnnt jieor jifix jint jior jiqint jiqnnt jishft jishftc jisign jmax0 jmax1 jmin0 jmin1 "
		"jmod jnint jnot jzext kiabs kiand kibclr kibits kibset kidim kidint kidnnt kieor kifix "
		"kind kint kior kishft kishftc kisign kmax0 kmax1 kmin0 kmin1 kmod knint knot kzext "
		"lbound leadz len len_trim lenlge lge lgt lle llt log log10 logical lshift malloc matmul "
		"max max0 max1 maxexponent maxloc maxval merge min min0 min1 minexponent minloc "
		"minval mod modulo mvbits nearest nint not nworkers number_of_processors pack popcnt "
		"poppar precision present product radix random random_number random_seed range real "
		"repeat reshape rrspacing rshift scale scan secnds selected_int_kind "
		"selected_real_kind set_exponent shape sign sin sind sinh size sizeof sngl snglq spacing "
		"spread sqrt sum system_clock tan tand tanh tiny transfer transpose trim ubound unpack verify";

	static const char* keywords3 = "cdabs cdcos cdexp cdlog cdsin cdsqrt cotan cotand "
		"dcmplx dconjg dcotan dcotand decode dimag dll_export dll_import doublecomplex dreal "
		"dvchk encode find flen flush getarg getcharqq getcl getdat getenv gettim hfix ibchng "
		"identifier imag int1 int2 int4 intc intrup invalop iostat_msg isha ishc ishl jfix "
		"lacfar locking locnear map nargs nbreak ndperr ndpexc offset ovefl peekcharqq precfill "
		"rompt qabs qacos qacosd qasin qasind qatan qatand qatan2 qcmplx qconjg qcos qcosd "
		"qcosh qdim qexp qext qextd qfloat qimag qlog qlog10 qmax1 qmin1 qmod qreal qsign qsin "
		"qsind qsinh qsqrt qtan qtand qtanh ran rand randu rewrite segment setdat settim system "
		"timer undfl unlock union val virtual volatile zabs zcos zexp zlog zsin zsqrt";
		
	Lexer *lexer = new Lexer(QextScintilla::SCLEX_F77, "fortran");
	lexer->setLangName(name);
	lexer->setFileExtensions(file_ext);
	lexer->setWordChars(ALPHA_CHARS NUM_CHARS "-_");
	lexer->setBlockComment("!~");
	
	lexer->addProperty("default", 0, 0x808080, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("comment", 1, commentBoxColor, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("number", 2, numberColor, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("singleQuotedString", 3, singleQuotedStringColor, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("doubleQuotedString", 4, doubleQuotedStringColor, "courier", 0, 0, 0, 0, 0x000000, false);
	//lexer->addProperty("EOLStringNotClosed", 5, 0, "courier", 0, 0, 0, 0, 0xE0C0E0, true);
	lexer->addProperty("EOLStringNotClosed", 5, 0, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("operator", 6, operatorColor, "courier", 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("identifier", 7, 0x000000, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("keywords", 8, keywordColor, "courier", 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("keywords2", 9, 0x4000B0, "courier", 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("keywords3", 10, 0x8040B0, "courier", 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("preprocessor", 11, preprocColor, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("dotOperators", 12, operatorColor, "courier", 0, true, 0, 0, 0x000000, false);
	lexer->addProperty("labels", 13, 0xE0C0E0, "courier", 0, 0, 0, 0, 0x000000, false);
	lexer->addProperty("continuation", 14, 0x000000, "courier", 0, 0, 0, 0, 0x80E0F0, false);
	lexer->addProperty("default2", 32, 0x000000, "courier", 0, 0, 0, 0, 0x000000, false);
	
	lexer->addKeywords(keywords1);
	lexer->addKeywords(keywords2);
	lexer->addKeywords(keywords3);
	LexerManager::addLexer(lexer);
}

void initXMLLexer()
{
	static const char* xml_file_ext = "xml xsl svg xul xsd xslt axl tpl dtd hta vxml ui cute";
	static const char* name = "XML";

	Lexer *xmlLexer = new Lexer(QextScintilla::SCLEX_HTML, "xml");
	xmlLexer->setLangName(name);
	xmlLexer->setFileExtensions(xml_file_ext);
	xmlLexer->setStreamComment("<!--", "-->");
	
	xmlLexer->addProperty("text", 0, 0x000000, "courier");
	xmlLexer->addProperty("tag", 1, 0x000080, 0, 0, true);
	xmlLexer->addProperty("unknownTag", 2, 0x80L, 0, 0, true);
	xmlLexer->addProperty("attributes", 3, 0x008080);
	xmlLexer->addProperty("unknownAttributes", 4, 0x008080);
	xmlLexer->addProperty("number", 5, numberColor);
	xmlLexer->addProperty("doubleQuotedString", 6, doubleQuotedStringColor);
	xmlLexer->addProperty("singleQuotedString", 7, singleQuotedStringColor);
	xmlLexer->addProperty("insideTag", 8, 0x800080);
	xmlLexer->addProperty("comment", 9, 0x808000, "new century schoolbook", 9);
	xmlLexer->addProperty("entities", 10, 0x800080);
	xmlLexer->addProperty("styleTagEnd", 11, 0x000080);
	xmlLexer->addProperty("identifierStart", 12, 0x800080, 0, 0, true);
	xmlLexer->addProperty("identifierEnd", 13, 0x800080, 0, 0, true);
	xmlLexer->addProperty("cdata", 17, 0x800000, 0, 0, true, 0, 0, 0xFFF0F0, true);
	xmlLexer->addProperty("question", 18, 0x800000);
	xmlLexer->addProperty("unquotedValue", 19, 0xFF00FF, 0, 0, 0, 0, 0, 0xFFEFFF);

	xmlLexer->addProperty("sgmlTag", 21, 0x000080);
	xmlLexer->addProperty("sgmlCommand", 22, 0x000080, 0, 0, true);
	xmlLexer->addProperty("sgmlFirstParam", 23, 0x006600);
	xmlLexer->addProperty("sgmlDoubleString", 24, 0x800000);
	xmlLexer->addProperty("sgmlSingleString", 25, 0x993300);
	xmlLexer->addProperty("sgmlError", 26, 0x800000);
	xmlLexer->addProperty("sgmlSpecial", 27, 0x3366FF);
	xmlLexer->addProperty("sgmlEntity", 28, 0x333333);
	xmlLexer->addProperty("sgmlComment", 29, 0x808000, "new century schoolbook", 9);
	xmlLexer->addProperty("sgmlBlock", 31, 0x000066);

	xmlLexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	xmlLexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);

	xmlLexer->addKeywords("");
	xmlLexer->addKeywords("");
	xmlLexer->addKeywords("");
	xmlLexer->addKeywords("");
	xmlLexer->addKeywords("");
	xmlLexer->addKeywords("ELEMENT DOCTYPE ATTLIST ENTITY NOTATION");
	LexerManager::addLexer(xmlLexer);
}

void initPHPLexer()
{
	static const char *php_file_ext = "php3 phtml php";
	static const char *name = "PHP";
	static const char *null_str = "";
	
	Lexer *phpLexer = new Lexer(QextScintilla::SCLEX_HTML, "php");
	phpLexer->setFileExtensions(php_file_ext);
	phpLexer->setLangName(name);
	phpLexer->setStreamComment("/*", "*/");
	phpLexer->setBlockComment("//");
	phpLexer->addProperty("text", 0, 0x000000, "courier");
	phpLexer->addProperty("tag", 1, 0x000080);
	phpLexer->addProperty("unknownTag", 2, 0xFF0000);
	phpLexer->addProperty("attributes", 3, 0x008080);
	phpLexer->addProperty("unknownAttributes", 4, 0xFF0000);
	phpLexer->addProperty("number", 5, numberColor);
	phpLexer->addProperty("doubleQuotedString", 6, doubleQuotedStringColor);
	phpLexer->addProperty("singleQuotedString", 7, singleQuotedStringColor);
	phpLexer->addProperty("insideTag", 8, 0x800080);
	phpLexer->addProperty("comment", 9, 0x808000, "new century schoolbook", 9);
	phpLexer->addProperty("entities", 10, 0x800080);
	phpLexer->addProperty("styleTagEnd", 11, 0x000080);
	phpLexer->addProperty("identifierStart", 12, 0x0000FF);
	phpLexer->addProperty("identifierEnd", 13, 0x0000FF);
	phpLexer->addProperty("script", 14, 0x000080);
	phpLexer->addProperty("asp1", 15, 0xFFFF00);
	phpLexer->addProperty("asp2", 16, 0xFFDF00);
	phpLexer->addProperty("cdata", 17, 0xFFDF00);
	phpLexer->addProperty("php", 18, 0x0000FF, 0, 0, 0, 0, 0, 0xFFEFBF);
	phpLexer->addProperty("unquotedValue", 19, 0xFF00FF, 0, 0, 0, 0, 0, 0xFFEFFF);

	phpLexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	phpLexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);

	phpLexer->addProperty("PHPDefault", 118, 0x000033, 0, 0, 0, 0, 0, 0xFFF8F8, true);
	phpLexer->addProperty("PHPDoubleQuotedString", 119, doubleQuotedStringColor, 0, 0, 0, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPSingleQuotedString", 120, singleQuotedStringColor, 0, 0, 0, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPKeyword", 121, keywordColor, 0, 0, true, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPNumber", 122, numberColor, "lucidatypewriter", 0, 0, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPVariable", 123, 0x00007F, 0, 0, true, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPComment", 124, commentBoxColor, "new century schoolbook", 9, 0, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPLineComment", 125, commentLineColor, "new century schoolbook", 9, 0, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPVariableInDoubleQuotedString", 126, 0x00007F, 0, 0, true, 0, 0, 0xFFF8F8);
	phpLexer->addProperty("PHPOperator", 127, operatorColor, 0, 0, 0, 0, 0, 0xFFF8F8);

	phpLexer->addKeywords(null_str);
	phpLexer->addKeywords(null_str);
	phpLexer->addKeywords(null_str);
	phpLexer->addKeywords(null_str);
	phpLexer->addKeywords(php_keywords);
	LexerManager::addLexer(phpLexer);
}

void initVBScriptLexer()
{
	static const char *vb_script_file_ext = "vb bas frm cls ctl pag dsr dob";
	static const char *name = "VB Script";
	
	Lexer *vbScriptLexer = new Lexer(QextScintilla::SCLEX_VB, "vbscript");
	vbScriptLexer->setFileExtensions(vb_script_file_ext);
	vbScriptLexer->setLangName(name);
	vbScriptLexer->setBlockComment("'");
	vbScriptLexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	vbScriptLexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	vbScriptLexer->addProperty("VBScriptStart", 70, 0x000000, 0, 0, 0, 0, 0, 0x000000);
	vbScriptLexer->addProperty("VBScriptDefault", 71, 0x000000, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptComment", 72, 0x008000, "new century schoolbook", 9, 0, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptNumber", 73, 0x008080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptKeyword", 74, 0x000080, "lucidatypewriter", 0, true, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptString", 75, 0x800080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptIdentifier", 76, 0x000080, 0, 0, 0, 0, 0, 0xEFEFFF, true);
	vbScriptLexer->addProperty("VBScriptUnterminatedString", 77, 0x000080, 0, 0, 0, 0, 0, 0x7F7FFF, true);
	vbScriptLexer->addKeywords(vb_script_keywords);
	LexerManager::addLexer(vbScriptLexer);
}

void initIDLLexer()
{
	static const char *idl_file_ext = "idl odl";
	static const char *name = "IDL";
	static const char *idl_keywords = "aggregatable allocate appobject arrays async async_uuid \
		auto_handle \
		bindable boolean broadcast byte byte_count \
		call_as callback char coclass code comm_status \
		const context_handle context_handle_noserialize \
		context_handle_serialize control cpp_quote custom \
		decode default defaultbind defaultcollelem \
		defaultvalue defaultvtable dispinterface displaybind dllname \
		double dual \
		enable_allocate encode endpoint entry enum error_status_t \
		explicit_handle \
		fault_status first_is float \
		handle_t heap helpcontext helpfile helpstring \
		helpstringcontext helpstringdll hidden hyper \
		id idempotent ignore iid_as iid_is immediatebind implicit_handle \
		import importlib in include in_line int __int64 __int3264 interface \
		last_is lcid length_is library licensed local long \
		max_is maybe message methods midl_pragma \
		midl_user_allocate midl_user_free min_is module ms_union \
		ncacn_at_dsp ncacn_dnet_nsp ncacn_http ncacn_ip_tcp \
		ncacn_nb_ipx ncacn_nb_nb ncacn_nb_tcp ncacn_np \
		ncacn_spx ncacn_vns_spp ncadg_ip_udp ncadg_ipx ncadg_mq \
		ncalrpc nocode nonbrowsable noncreatable nonextensible notify \
		object odl oleautomation optimize optional out out_of_line \
		pipe pointer_default pragma properties propget propput propputref \
		ptr public \
		range readonly ref represent_as requestedit restricted retval \
		shape short signed size_is small source strict_context_handle \
		string struct switch switch_is switch_type \
		transmit_as typedef \
		uidefault union unique unsigned user_marshal usesgetlasterror uuid \
		v1_enum vararg version void wchar_t wire_marshal";
	
	Lexer *idlLexer = new Lexer(QextScintilla::SCLEX_CPP, "idl");
	idlLexer->setFileExtensions(idl_file_ext);
	idlLexer->setLangName(name);
	initCLikeLexer(idlLexer);
	idlLexer->addKeywords(idl_keywords);
	LexerManager::addLexer(idlLexer);
}

void initMakefileLexer()
{
	static const char *file_ext = "makefile Makefile mak";
	static const char *name = "Makefile";
	static const char *makefile_keywords = "";

	Lexer *makefileLexer = new Lexer(QextScintilla::SCLEX_MAKEFILE, "makefile");
	makefileLexer->setFileExtensions(file_ext);
	makefileLexer->setLangName(name);
	makefileLexer->setBlockComment("#");
	makefileLexer->addProperty("default", 0, 0x000000, "courier", 0, 0, 0, 0, 0x000000);
	makefileLexer->addProperty("comment", 1, commentBoxColor, "new century schoolbook", 9, 0, 0, 0, 0x000000);
	makefileLexer->addProperty("preprocessor", 2, preprocColor, 0, 0, 0, 0, 0, 0x000000);
	makefileLexer->addProperty("variable", 3, keywordColor, 0, 0, true, 0, 0, 0x000000);
	makefileLexer->addProperty("operator", 4, operatorColor, 0, 0, 0, 0, 0, 0x000000);
	makefileLexer->addProperty("target", 5, 0xA00000, 0, 0, 0, 0, 0, 0x000000);
	makefileLexer->addProperty("error", 9, errorColor, 0, 0, 0, 0, 0, 0x000000, true);
	makefileLexer->addProperty("matchedOperator", 34, 0x0000FF, 0, 0, true, 0, 0, 0x000000);
	makefileLexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true, 0, 0, 0x000000);
	makefileLexer->addKeywords(makefile_keywords);
	LexerManager::addLexer(makefileLexer);
}

static void initDiffLexer()
{
	static const char *file_ext = "diff patch";
	static const char *name = "Diff";
	static const char *keywords = "";

	Lexer *lexer = new Lexer(QextScintilla::SCLEX_DIFF, "diff");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	lexer->addProperty("default", 0, 0x000000, "courier", 0, 0, 0, 0, 0x000000);
	lexer->addProperty("comment", 1, 0x007F0, "new century schoolbook", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("command", 2, 0x7F7F00, "lucidatypewriter", 0, 0, 0, 0, 0x000000);
	lexer->addProperty("sourcefile", 3, 0x7F0000, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("positionSetting", 4, 0x7F007F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("lineRemoval", 5, 0x007F7F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("lineAddition", 6, 0x00007F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addKeywords(keywords);
	LexerManager::addLexer(lexer);
}

static void initLatexLexer()
{
	static const char *file_ext = "tex sty";
	static const char *name = "Latex";
	static const char *keywords = "";

	Lexer *lexer = new Lexer(QextScintilla::SCLEX_LATEX, "latex");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	
	lexer->addProperty("whiteSpace", 0, 0x000000, "courier", 0, 0, 0, 0, 0x000000);
	lexer->addProperty("command", 1, 0xAA0000, "lucidatypewriter", 0, true, 0, 0, 0x000000);
	lexer->addProperty("tag", 2, 0x880088, 0, 0, true, 0, 0, 0x000000);
	lexer->addProperty("math", 3, 0xFF0000, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("comment", 4, 0x00AA00, "new century schoolbook", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	lexer->addKeywords(keywords);
	LexerManager::addLexer(lexer);
}

static void initSQLLexer()
{
	static const char *file_ext = "sql SQL";
	static const char *name = "SQL";
	static const char *keywords = "ABSOLUTE ACTION ADD ADMIN AFTER AGGREGATE "
	"ALIAS ALL ALLOCATE ALTER AND ANY ARE ARRAY AS ASC "
	"ASSERTION AT AUTHORIZATION "
	"BEFORE BEGIN BINARY BIT BLOB BOOLEAN BOTH BREADTH BY "
	"CALL CASCADE CASCADED CASE CAST CATALOG CHAR CHARACTER "
	"CHECK CLASS CLOB CLOSE COLLATE COLLATION COLUMN COMMIT "
	"COMPLETION CONNECT CONNECTION CONSTRAINT CONSTRAINTS "
	"CONSTRUCTOR CONTINUE CORRESPONDING CREATE CROSS CUBE CURRENT "
	"CURRENT_DATE CURRENT_PATH CURRENT_ROLE CURRENT_TIME CURRENT_TIMESTAMP "
	"CURRENT_USER CURSOR CYCLE "
	"DATA DATE DAY DEALLOCATE DEC DECIMAL DECLARE DEFAULT "
	"DEFERRABLE DEFERRED DELETE DEPTH DEREF DESC DESCRIBE DESCRIPTOR "
	"DESTROY DESTRUCTOR DETERMINISTIC DICTIONARY DIAGNOSTICS DISCONNECT "
	"DISTINCT DOMAIN DOUBLE DROP DYNAMIC "
	"EACH ELSE END END-EXEC EQUALS ESCAPE EVERY EXCEPT "
	"EXCEPTION EXEC EXECUTE EXTERNAL "
	"FALSE FETCH FIRST FLOAT FOR FOREIGN FOUND FROM FREE FULL "
	"FUNCTION "
	"GENERAL GET GLOBAL GO GOTO GRANT GROUP GROUPING "
	"HAVING HOST HOUR "
	"IDENTITY IGNORE IMMEDIATE IN INDICATOR INITIALIZE INITIALLY "
	"INNER INOUT INPUT INSERT INT INTEGER INTERSECT INTERVAL "
	"INTO IS ISOLATION ITERATE "
	"JOIN "
	"KEY "
	"LANGUAGE LARGE LAST LATERAL LEADING LEFT LESS LEVEL LIKE "
	"LIMIT LOCAL LOCALTIME LOCALTIMESTAMP LOCATOR "
	"MAP MATCH MINUTE MODIFIES MODIFY MODULE MONTH "
	"NAMES NATIONAL NATURAL NCHAR NCLOB NEW NEXT NO NONE "
	"NOT NULL NUMERIC "
	"OBJECT OF OFF OLD ON ONLY OPEN OPERATION OPTION "
	"OR ORDER ORDINALITY OUT OUTER OUTPUT "
	"PAD PARAMETER PARAMETERS PARTIAL PATH POSTFIX PRECISION PREFIX "
	"PREORDER PREPARE PRESERVE PRIMARY "
	"PRIOR PRIVILEGES PROCEDURE PUBLIC "
	"READ READS REAL RECURSIVE REF REFERENCES REFERENCING RELATIVE "
	"RESTRICT RESULT RETURN RETURNS REVOKE RIGHT "
	"ROLE ROLLBACK ROLLUP ROUTINE ROW ROWS "
	"SAVEPOINT SCHEMA SCROLL SCOPE SEARCH SECOND SECTION SELECT "
	"SEQUENCE SESSION SESSION_USER SET SETS SIZE SMALLINT SOME| SPACE "
	"SPECIFIC SPECIFICTYPE SQL SQLEXCEPTION SQLSTATE SQLWARNING START "
	"STATE STATEMENT STATIC STRUCTURE SYSTEM_USER "
	"TABLE TEMPORARY TERMINATE THAN THEN TIME TIMESTAMP "
	"TIMEZONE_HOUR TIMEZONE_MINUTE TO TRAILING TRANSACTION TRANSLATION "
	"TREAT TRIGGER TRUE "
	"UNDER UNION UNIQUE UNKNOWN "
	"UNNEST UPDATE USAGE USER USING "
	"VALUE VALUES VARCHAR VARIABLE VARYING VIEW "
	"WHEN WHENEVER WHERE WITH WITHOUT WORK WRITE "
	"YEAR ZONE";

	Lexer *lexer = new Lexer(QextScintilla::SCLEX_SQL, "sql");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	lexer->addProperty("whiteSpace", 0, 0x808080, "Courier", 0, 0, 0, 0, 0x000000);
	lexer->addProperty("comment", 1, 0x007F00, "new century schoolbook", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("lineComment", 2, 0x007F00, "new century schoolbook", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("docComment", 3, 0x7F7F7F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("number", 4, 0x007F7F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("keyword", 5, 0x00007F, "lucidatypewriter", 9, true, 0, 0, 0x000000);
	lexer->addProperty("doubleQuotedString", 6, 0x7F007F, "lucidatypewriter", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("singleQuotedString", 7, 0x7F007F, "lucidatypewriter", 9, 0, 0, 0, 0x000000);
	lexer->addProperty("symbols", 8, 0x007F7F, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("preprocessor", 9, 0x7F7F00, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("operators", 10, 0, 0, 0, true, 0, 0, 0x000000);
	lexer->addProperty("identifiers", 11, 0, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("EOLStringNotClosed", 12, 0, 0, 0, 0, 0, 0, 0xE0C0E0, true);
	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	lexer->addKeywords(keywords);
	LexerManager::addLexer(lexer);
}

static void initShellLexer()
{
	static const char *file_ext = "sh bsh";
	static const char *name = "Shell";
	static const char *keywords = "alias "
	"ar asa awk banner basename bash bc bdiff break "
	"bunzip2 bzip2 cal calendar case cat cc cd chmod cksum "
	"clear cmp col comm compress continue cp cpio crypt "
	"csplit ctags cut date dc dd declare deroff dev df diff diff3 "
	"dircmp dirname do done du echo ed egrep elif else env "
	"esac eval ex exec exit expand export expr false fc "
	"fgrep fi file find fmt fold for function functions "
	"getconf getopt getopts grep gres hash head help "
	"history iconv id if in integer jobs join kill local lc "
	"let line ln logname look ls m4 mail mailx make "
	"man mkdir more mt mv newgrp nl nm nohup ntps od "
	"pack paste patch pathchk pax pcat perl pg pr print "
	"printf ps pwd read readonly red return rev rm rmdir "
	"sed select set sh shift size sleep sort spell "
	"split start stop strings strip stty sum suspend "
	"sync tail tar tee test then time times touch tr "
	"trap true tsort tty type typeset ulimit umask unalias "
	"uname uncompress unexpand uniq unpack unset until "
	"uudecode uuencode vi vim vpax wait wc whence which "
	"while who wpaste wstart xargs zcat";

	static Lexer *lexer = new Lexer(QextScintilla::SCLEX_PERL, "shell");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	lexer->setBlockComment("#");
	lexer->addProperty("default", 32, 0x808080L, "courier");
	lexer->addProperty("error", 1, errorColor);
	lexer->addProperty("commentLine", 2, commentLineColor, "new century schoolbook", 9);
	lexer->addProperty("pod", 3, 0x004000L);
	lexer->addProperty("number", 4, numberColor);
	lexer->addProperty("keyword", 5, keywordColor, "lucidatypewriter", 9, true);
	lexer->addProperty("string", 6, stringColor);
	lexer->addProperty("character", 7, characterColor);
	lexer->addProperty("punctuation", 8, 0x0L);
	lexer->addProperty("preprocessor", 9, 0x0L);
	lexer->addProperty("operator", 10, operatorColor, 0, 0, true);
	lexer->addProperty("identifier", 11, 0x00007FL);
	lexer->addProperty("scalar", 12, 0x0L);
	lexer->addProperty("array", 13, 0x0L);
	lexer->addProperty("hash", 14, 0x0L);
	lexer->addProperty("symbolTable", 15, 0x0L);
	lexer->addProperty("regex", 17, 0x0L);
	lexer->addProperty("regSubst", 18, 0x0L);
	lexer->addProperty("longQuote", 19, 0xFFFF00L);
	lexer->addProperty("backTicks", 20, 0x0000FFL);
	lexer->addProperty("dataSection", 21, 0x600000L);
	lexer->addProperty("hereDelim", 22, 0x000000L);
	lexer->addProperty("hereQ", 23, 0x7F007FL);
	lexer->addProperty("hereQQ", 24, 0x7F007FL);
	lexer->addProperty("hereQX", 25, 0x7F007FL);
	lexer->addProperty("stringQ", 26, 0xF007FL);
	lexer->addProperty("stringQQ", 27, doubleQuotedStringColor);
	lexer->addProperty("stringQX", 28, backTicksColor);
	lexer->addProperty("stringQR", 29, regexColor);
	lexer->addProperty("stringQW", 30, arrayColor);
	lexer->addProperty("matchedOperators2",35 , 0x0000FF, 0, 0, true);
	lexer->addProperty("matchedOperators1",34 , 0xFF0000, 0, 0, true);
	lexer->addKeywords(keywords);
	LexerManager::addLexer(lexer);
}

static void initTCLLexer()
{
	static const char *file_ext = "tcl itcl";
	static const char *name = "TCL";
	static const char *keywords = "after append array auto_execok auto_import auto_load "
		"auto_load_index auto_qualify beep binary break case catch cd clock "
		"close concat continue dde default echo else elseif encoding eof "
		"error eval exec exit expr fblocked fconfigure fcopy file "
		"fileevent flush for foreach format gets glob global history if "
		"incr info interp join lappend lindex linsert list llength load "
		"lrange lreplace lsearch lsort namespace open package pid "
		"pkg_mkIndex proc puts pwd read regexp regsub rename resource "
		"return scan seek set socket source split string subst switch "
		"tclLog tclMacPkgSearch tclPkgSetup tclPkgUnknown tell time "
		"trace unknown unset update uplevel upvar variable vwait while "
		// Tk keywords
		"bell bind bindtags button canvas checkbutton console "
		"destroy entry event focus font frame grab grid image label listbox menu "
		"menubutton message pack place radiobutton raise scale scrollbar "
		"text tk tkwait toplevel winfo wm "
		// Tk commands
		"tkButtonDown tkButtonEnter tkButtonInvoke "
		"tkButtonLeave tkButtonUp tkCancelRepeat tkCheckRadioInvoke tkDarken "
		"tkEntryAutoScan tkEntryBackspace tkEntryButton1 tkEntryClosestGap "
		"tkEntryInsert tkEntryKeySelect tkEntryMouseSelect tkEntryNextWord "
		"tkEntryPaste tkEntryPreviousWord tkEntrySeeInsert tkEntrySetCursor "
		"tkEntryTranspose tkEventMotifBindings tkFDGetFileTypes tkFirstMenu "
		"tkFocusGroup_Destroy tkFocusGroup_In tkFocusGroup_Out tkFocusOK "
		"tkListboxAutoScan tkListboxBeginExtend tkListboxBeginSelect "
		"tkListboxBeginToggle tkListboxCancel tkListboxDataExtend "
		"tkListboxExtendUpDown tkListboxMotion tkListboxSelectAll "
		"tkListboxUpDown tkMbButtonUp tkMbEnter tkMbLeave tkMbMotion "
		"tkMbPost tkMenuButtonDown tkMenuDownArrow tkMenuDup tkMenuEscape "
		"tkMenuFind tkMenuFindName tkMenuFirstEntry tkMenuInvoke tkMenuLeave "
		"tkMenuLeftArrow tkMenuMotion tkMenuNextEntry tkMenuNextMenu "
		"tkMenuRightArrow tkMenuUnpost tkMenuUpArrow tkMessageBox "
		"tkPostOverPoint tkRecolorTree tkRestoreOldGrab tkSaveGrabInfo "
		"tkScaleActivate tkScaleButton2Down tkScaleButtonDown "
		"tkScaleControlPress tkScaleDrag tkScaleEndDrag tkScaleIncrement "
		"tkScreenChanged tkScrollButton2Down tkScrollButtonDown "
		"tkScrollButtonUp tkScrollByPages tkScrollByUnits tkScrollDrag "
		"tkScrollEndDrag tkScrollSelect tkScrollStartDrag tkScrollToPos  "
		"tkScrollTopBottom tkTabToWindow tkTearOffMenu tkTextAutoScan "
		"tkTextButton1 tkTextClosestGap tkTextInsert tkTextKeyExtend "
		"tkTextKeySelect tkTextNextPara tkTextNextPos tkTextNextWord "
		"tkTextPaste tkTextPrevPara tkTextPrevPos tkTextResetAnchor "
		"tkTextScrollPages tkTextSelectTo tkTextSetCursor tkTextTranspose "
		"tkTextUpDownLine tkTraverseToMenu tkTraverseWithinMenu tk_bisque "
		"tk_chooseColor tk_dialog tk_focusFollowsMouse tk_focusNext "
		"tk_focusPrev tk_getOpenFile tk_getSaveFile tk_messageBox "
		"tk_optionMenu tk_popup tk_setPalette tk_textCopy tk_textCut "
		"tk_textPaste";

	static Lexer *lexer = new Lexer(QextScintilla::SCLEX_TCL, "tcl");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	lexer->setBlockComment("#");
	lexer->addProperty("whiteSpace", 0, 0x808080, "courier");
	lexer->addProperty("comment", 1, commentLineColor, "new century schoolbook", 9);
	lexer->addProperty("number", 2, 0x007F7F);
	lexer->addProperty("string", 3, 0x7F007F, "lucidatypewriter");
	lexer->addProperty("singleQuotedString", 4, 0x7F007F, "lucidatypewriter");
	lexer->addProperty("keyword", 5, 0x00007F, "lucidatypewriter", 0, true);
	lexer->addProperty("tripleQuotes", 6, 0x7F0000);
	lexer->addProperty("tripleDoubleQuotes", 7, 0x7F0000);
	lexer->addProperty("classNameDefinition", 8, 0x0000FF, 0, 0, true);
	lexer->addProperty("functionNameDefinition", 9, 0x007F7F, 0, 0, true);
	lexer->addProperty("operators", 10, 0x0);
	lexer->addProperty("identifiers", 11, 0x0);
	lexer->addProperty("commentBlocks", 12, commentBoxColor, "new century schoolbook", 9);
	lexer->addProperty("EOLStringNotClosed", 13, 0x0, 0, 0, 0, 0, true, 0xE0C0E0);
	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	lexer->addKeywords(keywords);
	LexerManager::addLexer(lexer);
}

static void initDocbookLexer()
{
	static const char *file_ext = "docbook";
	static const char *name = "Docbook";
	static const char *keywords = 
		"abbrev abstract accel ackno acronym action address affiliation alt anchor "
		"answer appendix appendixinfo application area areaset areaspec arg article "
		"articleinfo artpagenums attribution audiodata audioobject author authorblurb "
		"authorgroup authorinitials beginpage bibliocoverage bibliodiv biblioentry "
		"bibliography bibliographyinfo biblioid bibliomisc bibliomixed bibliomset "
		"bibliorelation biblioset bibliosource blockinfo blockquote book bookinfo "
		"bridgehead callout calloutlist caption caution chapter chapterinfo citation "
		"citebiblioid citerefentry citetitle city classname classsynopsis classsynopsisinfo "
		"cmdsynopsis co collab collabname colophon colspec command computeroutput "
		"confdates confgroup confnum confsponsor conftitle constant constraint "
		"constraintdef constructorsynopsis contractnum contractsponsor contrib "
		"copyright coref corpauthor corpname country database date dedication "
		"destructorsynopsis edition editor email emphasis entry entrytbl envar "
		"epigraph equation errorcode errorname errortext errortype example "
		"exceptionname fax fieldsynopsis figure filename firstname firstterm "
		"footnote footnoteref foreignphrase formalpara funcdef funcparams "
		"funcprototype funcsynopsis funcsynopsisinfo function glossary glossaryinfo "
		"glossdef glossdiv glossentry glosslist glosssee glossseealso glossterm "
		"graphic graphicco group guibutton guiicon guilabel guimenu guimenuitem "
		"guisubmenu hardware highlights holder honorific htm imagedata imageobject "
		"imageobjectco important index indexdiv indexentry indexinfo indexterm "
		"informalequation informalexample informalfigure informaltable initializer "
		"inlineequation inlinegraphic inlinemediaobject interface interfacename "
		"invpartnumber isbn issn issuenum itemizedlist itermset jobtitle keycap "
		"keycode keycombo keysym keyword keywordset label legalnotice lhs lineage "
		"lineannotation link listitem iteral literallayout lot lotentry manvolnum "
		"markup medialabel mediaobject mediaobjectco member menuchoice methodname "
		"methodparam methodsynopsis mm modespec modifier ousebutton msg msgaud "
		"msgentry msgexplan msginfo msglevel msgmain msgorig msgrel msgset msgsub "
		"msgtext nonterminal note objectinfo olink ooclass ooexception oointerface "
		"option optional orderedlist orgdiv orgname otheraddr othercredit othername "
		"pagenums para paramdef parameter part partinfo partintro personblurb "
		"personname phone phrase pob postcode preface prefaceinfo primary primaryie "
		"printhistory procedure production productionrecap productionset productname "
		"productnumber programlisting programlistingco prompt property pubdate publisher "
		"publishername pubsnumber qandadiv qandaentry qandaset question quote refclass "
		"refdescriptor refentry refentryinfo refentrytitle reference referenceinfo "
		"refmeta refmiscinfo refname refnamediv refpurpose refsect1 refsect1info refsect2 "
		"refsect2info refsect3 refsect3info refsection refsectioninfo refsynopsisdiv "
		"refsynopsisdivinfo releaseinfo remark replaceable returnvalue revdescription "
		"revhistory revision revnumber revremark rhs row sbr screen screenco screeninfo "
		"screenshot secondary secondaryie sect1 sect1info sect2 sect2info sect3 sect3info "
		"sect4 sect4info sect5 sect5info section sectioninfo see seealso seealsoie "
		"seeie seg seglistitem segmentedlist segtitle seriesvolnums set setindex "
		"setindexinfo setinfo sgmltag shortaffil shortcut sidebar sidebarinfo simpara "
		"simplelist simplemsgentry simplesect spanspec state step street structfield "
		"structname subject subjectset subjectterm subscript substeps subtitle "
		"superscript surname sv symbol synopfragment synopfragmentref synopsis "
		"systemitem table tbody term tertiary tertiaryie textdata textobject tfoot "
		"tgroup thead tip title titleabbrev toc tocback tocchap tocentry tocfront "
		"toclevel1 toclevel2 toclevel3 toclevel4 toclevel5 tocpart token trademark "
		"type ulink userinput varargs variablelist varlistentry varname videodata "
		"videoobject void volumenum warning wordasword xref year"
		"arch condition conformance id lang os remap role revision revisionflag security "
		"userlevel vendor xreflabel "
		"status label endterm linkend space width"
		"ELEMENT DOCTYPE ATTLIST ENTITY NOTATION";

	static Lexer *lexer = new Lexer(QextScintilla::SCLEX_HTML, "docbook");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	lexer->setStreamComment("<!--", "-->");

	lexer->addProperty("text", 0, 0x000000, "courier");
	lexer->addProperty("tag", 1, 0x000080, 0, 0, true);
	lexer->addProperty("unknownTag", 2, 0xFF0000);
	lexer->addProperty("attributes", 3, 0x008080);
	lexer->addProperty("unknownAttributes", 4, 0xFF0000);
	lexer->addProperty("number", 5, numberColor);
	lexer->addProperty("doubleQuotedString", 6, doubleQuotedStringColor);
	lexer->addProperty("singleQuotedString", 7, singleQuotedStringColor);
	lexer->addProperty("insideTag", 8, 0x800080);
	lexer->addProperty("comment", 9, 0x808000, "new century schoolbook", 9);
	lexer->addProperty("entities", 10, 0x800080);
	lexer->addProperty("styleTagEnd", 11, 0x000080);
	lexer->addProperty("identifierStart", 12, 0x0000FF);
	lexer->addProperty("identifierEnd", 13, 0x0000FF);
	lexer->addProperty("script", 14, 0x000080);
	lexer->addProperty("asp1", 15, 0xFFFF00);
	lexer->addProperty("asp2", 16, 0xFFDF00);
	lexer->addProperty("cdata", 17, 0xFFDF00);
	lexer->addProperty("php", 18, 0x0000FF, 0, 0, 0, 0, 0, 0xFFEFBF);
	lexer->addProperty("unquotedValue", 19, 0xFF00FF, 0, 0, 0, 0, 0, 0xFFEFFF);
	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);
	lexer->addKeywords(keywords);

	LexerManager::addLexer(lexer);
}

void initAsmLexer()
{
	static const char *file_ext = "asm";
	static const char *name = "Assembler";
	static const char *cpu_instructions = 
	"aaa aad aam aas adc add and call cbw "
	"clc cld cli cmc cmp cmps cmpsb cmpsw cwd daa das dec div esc hlt "
	"idiv imul in inc int into iret ja jae jb jbe jc jcxz je jg jge jl "
	"jle jmp jna jnae jnb jnbe jnc jne jng jnge jnl jnle jno jnp jns "
	"jnz jo jp jpe jpo js jz lahf lds lea les lods lodsb lodsw loop "
	"loope loopew loopne loopnew loopnz loopnzw loopw loopz loopzw "
	"mov movs movsb movsw mul neg nop not or out pop popf push pushf "
	"rcl rcr ret retf retn rol ror sahf sal sar sbb scas scasb scasw "
	"shl shr stc std sti stos stosb stosw sub test wait xchg xlat "
	"xlatb xor "
	"bound enter ins insb insw leave outs outsb outsw popa pusha pushw "
	"arpl lar lsl sgdt sidt sldt smsw str verr verw clts lgdt lidt lldt lmsw ltr "
	"bsf bsr bt btc btr bts cdq cmpsd cwde insd iretd iretdf  iretf "
	"jecxz lfs lgs lodsd loopd  looped  loopned  loopnzd  loopzd  lss "
	"movsd movsx movzx outsd popad popfd pushad pushd  pushfd scasd seta "
	"setae setb setbe setc sete setg setge setl setle setna setnae setnb "
	"setnbe setnc setne setng setnge setnl setnle setno setnp setns "
	"setnz seto setp setpe setpo sets setz shld shrd stosd "
	"bswap cmpxchg invd  invlpg  wbinvd  xadd "
	"lock rep repe repne repnz repz";
	static const char *fpu_instructions = 
	"f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp "
	"fdisi fdiv fdivp fdivr fdivrp feni ffree fiadd ficom ficomp fidiv "
	"fidivr fild fimul fincstp finit fist fistp fisub fisubr fld fld1 "
	"fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul "
	"fmulp fnclex fndisi fneni fninit fnop fnsave fnsavew fnstcw fnstenv "
	"fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw fsave "
	"fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp fstsw fsub fsubp "
	"fsubr fsubrp ftst fwait fxam fxch fxtract fyl2x fyl2xp1 "
	"fsetpm fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin fsincos "
	"fstenvd fucom fucomp fucompp";
	static const char* registers =
	"ah al ax bh bl bp bx ch cl cr0 cr2 cr3 cs "
	"cx dh di dl dr0 dr1 dr2 dr3 dr6 dr7 ds dx eax ebp ebx ecx edi edx "
	"es esi esp fs gs si sp ss st tr3 tr4 tr5 tr6 tr7 ";
	static const char *directives =
	".186 .286 .286c .286p .287 .386 .386c .386p .387 .486 .486p "
	".8086 .8087 .alpha .break .code .const .continue .cref .data .data?  "
	".dosseg .else .elseif .endif .endw .err .err1 .err2 .errb "
	".errdef .errdif .errdifi .erre .erridn .erridni .errnb .errndef "
	".errnz .exit .fardata .fardata? .if .lall .lfcond .list .listall "
	".listif .listmacro .listmacroall  .model .no87 .nocref .nolist "
	".nolistif .nolistmacro .radix .repeat .sall .seq .sfcond .stack "
	".startup .tfcond .type .until .untilcxz .while .xall .xcref "
	".xlist alias align assume catstr comm comment db dd df dosseg dq "
	"dt dup dw echo else elseif elseif1 elseif2 elseifb elseifdef elseifdif "
	"elseifdifi elseife elseifidn elseifidni elseifnb elseifndef end "
	"endif endm endp ends eq  equ even exitm extern externdef extrn for "
	"forc ge goto group gt high highword if if1 if2 ifb ifdef ifdif "
	"ifdifi ife  ifidn ifidni ifnb ifndef include includelib instr invoke "
	"irp irpc label le length lengthof local low lowword lroffset "
	"lt macro mask mod .msfloat name ne offset opattr option org %out "
	"page popcontext proc proto ptr public purge pushcontext record "
	"repeat rept seg segment short size sizeof sizestr struc struct "
	"substr subtitle subttl textequ this title type typedef union while width";
	static const char *directive_operands =
	"$ ? @b @f addr basic byte c carry? dword "
	"far far16 fortran fword near near16 overflow? parity? pascal qword  "
	"real4  real8 real10 sbyte sdword sign? stdcall sword syscall tbyte "
	"vararg word zero? flat near32 far32 "
	"abs all assumes at casemap common compact "
	"cpu dotname emulator epilogue error export expr16 expr32 farstack flat "
	"forceframe huge language large listing ljmp loadds m510 medium memory "
	"nearstack nodotname noemulator nokeyword noljmp nom510 none nonunique "
	"nooldmacros nooldstructs noreadonly noscoped nosignextend nothing "
	"notpublic oldmacros oldstructs os_dos para private prologue radix "
	"readonly req scoped setif2 smallstack tiny use16 use32 uses";
	
	static Lexer *lexer = new Lexer(QextScintilla::SCLEX_ASM, "asm");
	lexer->setLangName(name);
	lexer->setBlockComment( ";" );
	lexer->setFileExtensions(file_ext);

	lexer->addProperty("default", 0, 0x0, 0, 0, 0, 0);
	lexer->addProperty("comment", 1, 0xadadad, 0, 0, 0, 0);
	lexer->addProperty("number", 2, 0x0000ff, 0, 0, 0, 0);
	lexer->addProperty("string", 3, stringColor, 0, 0, 0, 0);
	lexer->addProperty("operator", 4, operatorColor, 0, 0, true, 0);
	lexer->addProperty("identifier", 5, 0x0, 0, 0, 0, 0);
	lexer->addProperty("CPUInstructions", 6, keywordColor, 0, 0, true, 0);
	lexer->addProperty("FPUInstructions", 7, 0xff0000, 0, 0, 0, 0);
	lexer->addProperty("register", 8, 0x03aa46, 0, 0, true, 0);
	lexer->addProperty("directive", 9, 0xff0000, 0, 0, 0, 0);
	lexer->addProperty("directiveOperand", 10, 0xff0000, 0, 0, 0, 0);
	
	lexer->addKeywords( cpu_instructions );
	lexer->addKeywords( fpu_instructions );
	lexer->addKeywords( registers );
	lexer->addKeywords( directives );
	lexer->addKeywords( directive_operands );

	LexerManager::addLexer(lexer);
}

void initMatlabLexer()
{
	static const char *file_ext = "m";
	static const char *name = "Matlab";
	static const char *keywords = "break case catch continue else elseif end for function "
		"global if otherwise persistent return switch try while";

	Lexer *lexer = new Lexer(QextScintilla::SCLEX_MATLAB, "matlab");
	lexer->setFileExtensions(file_ext);
	lexer->setLangName(name);
	
	lexer->addProperty("whiteSpace", 0, 0x0, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("comment", 1, commentLineColor, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("command", 2, preprocColor, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("number", 3, numberColor, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("keyword", 4, keywordColor, 0, 0, true, 0, 0, 0x000000);
	lexer->addProperty("string", 5, stringColor, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("operator", 6, operatorColor, 0, 0, true, 0, 0, 0x000000);
	lexer->addProperty("identifier", 7, 0x0, 0, 0, 0, 0, 0, 0x000000);
	lexer->addProperty("matchedOperator", 34, 0xFF0000, 0, 0, true);
	lexer->addProperty("unmatchedOperator", 35, 0x0000FF, 0, 0, true);

	lexer->addKeywords(keywords);
	lexer->setBlockComment("%~");
	LexerManager::addLexer(lexer);
}

void LexerManager::initLexers()
{
	property_PropertyType.ob_type = &PyType_Type;
	
	initPlainLexer();
	initAsmLexer();
	initCppLexer();
	initCSharpLexer();
	initCSSLexer();
	initDiffLexer();
	initDocbookLexer();
	initFortranLexer();
	initHTMLLexer();
	initIDLLexer();
	initJavaLexer();
	initJavaScriptLexer();
	initLatexLexer();
	initMakefileLexer();
	initMatlabLexer();
	initPerlLexer();
	initPHPLexer();
	initPythonLexer();
	initShellLexer();
	initSQLLexer();
	initTCLLexer();
	initVBScriptLexer();
	initXMLLexer();
}

}	// namespace Config

