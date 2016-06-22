# (C) Copyright 2003, Heiko Koeöhler

Name:         cute
License:      GPL
Group:        Productivity/Editors
Summary:      a scintilla based, scriptable code editor using python
BuildRoot:    %{_tmppath}/%{name}-%{version}-build
Version:      0.2.9
Release:      0
Source:       cute-%{version}.tar.gz
Distribution: SuSE 8.2
url:          http://cute.sourceforge.net
Packager:     Heiko Koeöhler <heicom@users.sourceforge.net>
Provides:     cute
Requires:     qt3
Vendor:       Heiko Koeöhler

%description
The CUTE User-friendly Text Editor is a Qt and Scintilla based text editor 
which can be easily extended using  python. 
Its main purpose is to be an user-friendly source code editor 
with a common graphical user interface, unlike VI and co. 
The editor supports projects, ctags, grep, bookmarks, keyboard macros 
and many programmming languages.

%prep
%setup -q -n %name-%version
#%patch

%build
export QTDIR=/usr/lib/qt3/
export QMAKESPEC=/usr/lib/qt3/mkspecs/linux-g++/
export QTDIR_LIB=$RPM_BUILD_ROOT/$QTDIR/%_lib
export PATH=$QTDIR/bin/:$PATH
qmake -o Makefile cute.pro
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/X11R6/bin
install bin/cute $RPM_BUILD_ROOT/usr/X11R6/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/cute/lib
install cute/scripts/*.py $RPM_BUILD_ROOT/usr/share/cute/lib/
mkdir -p $RPM_BUILD_ROOT/usr/share/cute/langs
mkdir -p $RPM_BUILD_ROOT/usr/share/icons
install cute/langs/*.py $RPM_BUILD_ROOT/usr/share/cute/langs/
install cute/icons/cute.xpm $RPM_BUILD_ROOT/usr/share/icons/
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/cute
if test ! -e cute/doc/doc/index.html 
then
	ln cute/doc/doc/book1.html cute/doc/doc/index.html 
fi
install cute/doc/doc/*.html $RPM_BUILD_ROOT/usr/share/doc/cute

%clean
make distclean

%files
%defattr(-,root,root)
/usr/X11R6/bin/cute
/usr/share/cute/
/usr/share/icons/cute.xpm
/usr/share/doc/cute/
