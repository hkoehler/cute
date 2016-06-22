TEMPLATE	= app

CONFIG	= qt release thread 

#change these variables to your system specific paths on your machine

#where your QextScintilla source is installed (http://www.riverbankcomputing.co.uk/)
QEXTSCINTILLADIR = ../qscintilla/
#where your python header files are installed (http://www.python.org)
PYTHON_INCLUDE_DIR = /usr/include/python2.2/
#where your python libs are installed 
PYTHON_LIB_DIR = /usr/lib/python2.2/

unix:INCLUDEPATH= 	$$QEXTSCINTILLADIR/qt/ $$QEXTSCINTILLADIR/include \
			$$PYTHON_INCLUDE_DIR 
unix:LIBS	=  -Xlinker -export-dynamic -L$$PYTHON_LIB_DIR/config -L$$QEXTSCINTILLADIR/lib -lqscintilla \
			-lpython2.2 -lutil 

HEADERS	= cute.h \
		  cuteview.h \
		  cutescintilla.h\
		  icons/pixmaps.h \
		  resource.h \
		  config.h \
		  finddlg.h \
		  replacedlg.h \
		  windowdlg.h \
		  savedlg.h \
		  outputview.h \
		  dirview.h \
		  qcompletionbox.h \
		  cmdinterpreter.h \
		  py_buffer.h \
		  py_config.h \
		  python_api.h \
		  findinfilesdlg.h \
		  lexer.h \
		  settingsdlg.h \
		  shortcutsdlg.h \
		  shortcutdlg.h \
		  readtags.h \
		  shortcuts.h \
		  general.h \
		  cuteshell.h \
		  helpviewer.h \
		  tagsview.h \
		  tagsdlg.h \
		  project.h \
		  projectdlg.h \
		  projectview.h \
		  bookmark.h \
		  mdi.h \
          cutestatusbar.h \
          cutehook.h

SOURCES	= cute.cpp \
		  cuteview.cpp \
		  cutescintilla.cpp \
		  main.cpp \
		  config.cpp \
		  finddlg.cpp \
		  replacedlg.cpp \
		  windowdlg.cpp \
		  savedlg.cpp \
		  outputview.cpp \
		  dirview.cpp \
		  qcompletionbox.cpp \
		  cmdinterpreter.cpp \
		  python_api.cpp \
		  py_buffer.cpp \
		  py_config.cpp \
		  findinfilesdlg.cpp \
		  lexer.cpp \
		  settingsdlg.cpp \
		  shortcutsdlg.cpp \
		  shortcutdlg.cpp \
		  readtags.c \
		  shortcuts.cpp \
		  cuteshell.cpp \
		  helpviewer.cpp \
		  tagsview.cpp \
		  tagsdlg.cpp \
		  project.cpp \
		  projectdlg.cpp \
		  projectview.cpp \
		  bookmark.cpp \
		  mdi.cpp \
        cutestatusbar.cpp \
        cutehook.cpp \
        pyqt_api.cpp

INTERFACES_DIR = ui

INTERFACES	= ui/aboutdlg.ui ui/finddialog.ui ui/replacedialog.ui ui/licensedlg.ui \ 
		  ui/windowdialog.ui ui/savedialog.ui ui/outputviewbase.ui ui/findinfilesdialog.ui \
		  ui/settings.ui ui/shortcutsdialog.ui ui/shortcutdialog.ui ui/tagsdialog.ui \
		  ui/projectdialog.ui ui/statusbar.ui

OBJECTS_DIR = obj

MOC_DIR = moc

TARGET = ../bin/cute
