Name: org.tizen.contacts
Version: 0.1
Release: 1
License: Apache-2.0
Summary: Contacts and Phone Applications
Group: Applications/Social
Source: %{name}-%{version}.tar.gz

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: pkgconfig(badge)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-preference)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)

%description
Contacts and Phone Reference Applications.

%define _pkgdir /usr/apps/%{name}
%define _bindir %{_pkgdir}/bin
%define _libdir %{_pkgdir}/lib
%define _resdir %{_pkgdir}/res
%define _manifestdir /usr/share/packages
%define _icondir /usr/share/icons/default/small
%define _smackdir /etc/smack/accesses.d

%prep
%setup -q

%build
cmake \
	%{!?RELEASE: -DCMAKE_BUILD_TYPE=Debug }\
	'-DPACKAGE=%{name}'\
	'-DCMAKE_INSTALL_PREFIX=%{_pkgdir}'\
	'-DBINDIR=%{_bindir}'\
	'-DLIBDIR=%{_libdir}'\
	'-DRESDIR=%{_resdir}'\
	'-DMANIFESTDIR=%{_manifestdir}'\
	'-DICONDIR=%{_icondir}'\
	'-DSMACKDIR=%{_smackdir}'

make %{?jobs:-j%jobs}

%install
%make_install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_bindir}/*
%{_libdir}/*.so
%{_manifestdir}/%{name}.xml
%{_icondir}/*.png
%{_smackdir}/%{name}.efl
