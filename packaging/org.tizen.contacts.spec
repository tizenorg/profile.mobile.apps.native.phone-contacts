Name: org.tizen.contacts
Version: 0.1
Release: 1
License: Apache-2.0
Summary: Contacts and Phone Applications
Group: Applications/Social
Source: %{name}-%{version}.tar.gz

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: gettext

BuildRequires: pkgconfig(badge)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-preference)
BuildRequires: pkgconfig(capi-base-utils-i18n)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(notification)

%description
Contacts and Phone Reference Applications.

%define _pkg_dir                %{TZ_SYS_RO_APP}/%{name}
%define _bin_dir                %{_pkg_dir}/bin
%define _lib_dir                %{_pkg_dir}/lib
%define _res_dir                %{_pkg_dir}/res
%define _locale_dir             %{_res_dir}/locale
%define _manifest_dir           %{TZ_SYS_RO_PACKAGES}
%define _icon_dir               %{TZ_SYS_RO_ICONS}/default/small

%prep
%setup -q

%build
cmake \
	%{!?RELEASE: -DCMAKE_BUILD_TYPE=Debug } \
	-DPACKAGE=%{name} \
	-DCMAKE_INSTALL_PREFIX=%{_pkg_dir} \
	-DBIN_DIR=%{_bin_dir} \
	-DLIB_DIR=%{_lib_dir} \
	-DRES_DIR=%{_res_dir} \
	-DLOCALE_DIR=%{_locale_dir} \
	-DMANIFEST_DIR=%{_manifest_dir} \
	-DICON_DIR=%{_icon_dir}

make %{?jobs:-j%jobs}

%install
%make_install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%license LICENSE
%{_pkg_dir}/*
%{_manifest_dir}/%{name}.xml
%{_icon_dir}/*.png
