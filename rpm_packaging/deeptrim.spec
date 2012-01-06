%define sourceversion 0.0.9
%define relcount 2

Name:    deeptrim
Version: %{sourceversion}
Release: %{relcount}%{dist}
Summary: Text editor

License: GPLv2
Group:   Applications/Productivity
Packager: Bernd Stramm <bernd.stramm@gmail.com>
Url:     http://%{name}.sourceforge.net/
Source: %{name}_%{version}.tar.gz
Source2: %{name}.desktop
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# TODO: patches (?)


#Obsoletes:
#Provides: 

BuildRequires: pkgconfig(QtGui) >= 4.6
BuildRequires: pkgconfig(QtWebKit) >= 4.6

%if X%{_vendor} == Xmeego
%define assume_meego 1
%else
%define assume_meego 0
%endif

%if 0%{?suse_version}
BuildRequires: libqscintilla-devel
%else
%if %{assume_meego}
BuildRequires: libqscintilla-devel
%else
BuildRequires:  qscintilla-devel
%endif
%endif
BuildRequires:  gcc-c++
BuildRequires: desktop-file-utils

%description
A text editor using QScintilla.


%prep
%setup

%build
sh configure
make 

%install

rm -rf $RPM_BUILD_ROOT
install -p -d $RPM_BUILD_ROOT%{_bindir}
install -p -m 755 bin/%{name} $RPM_BUILD_ROOT%{_bindir}
install -p -d $RPM_BUILD_ROOT%{_datadir}/pixmaps
install -p -m 755 %{name}.png $RPM_BUILD_ROOT%{_datadir}/pixmaps
desktop-file-install                                    \
--dir=${RPM_BUILD_ROOT}%{_datadir}/applications         \
%{SOURCE2}

%clean
rm -rf %{buildroot}


%post 

%postun 


%files
%defattr(-,root,root,-)
%doc README
%doc COPYRIGHT
%doc LICENSE.txt
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png

%changelog
* Wed May 04 2011 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.9-1
- filter out middle-button mouse clicks, so they do not paste in editor

* Tue Mar 29 2011 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.8-1
- update packaging

* Mon Dec 06 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.7-1
- even more improved shortcuts

* Sun Dec 05 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.5-1
- correct line number to display 1-based
- handle ambiguous shortcut keys better

* Thu Dec 02 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.5-1
- add files from command line to already running instance
- cosmetic stuff

* Wed Nov 03 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.4-1
- jump to line
- misc cosmetic details

* Sun Oct 31 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.3-1
- file load/save/refresh
- search/replace
- various window management issues 
- docking window issues
- font configuration for file types
- lexers for file types

* Thu Oct 28 2010 Bernd Stramm <bernd.stramm@gmail.com> - 0.0.1-1
- Initial RPM build

