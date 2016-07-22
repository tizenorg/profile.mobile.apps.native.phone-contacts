Name:       org.tizen.contacts
#VCS_FROM:   profile/mobile/apps/native/phone-contacts#86ebb7f44d0ddfed0e3df50fb482a47f9b9afc66
#RS_Ver:    20160722_2 
Summary:    Contacts and Phone Applications
Version:    2.0.0
Release:    1
Group:      Applications/Social
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

ExcludeArch:  aarch64 x86_64
BuildRequires:  pkgconfig(libtzplatform-config)
Requires(post):  /usr/bin/tpk-backend

%define internal_name org.tizen.contacts
%define preload_tpk_path %{TZ_SYS_RO_APP}/.preload-tpk 

%define build_mode %{nil}

%ifarch i386 i486 i586 i686 x86_64
%define target i386
%else
%ifarch arm armv7l aarch64
%define target arm
%else
%define target noarch
%endif
%endif

%description
profile/mobile/apps/native/phone-contacts#86ebb7f44d0ddfed0e3df50fb482a47f9b9afc66
This is a container package which have preload TPK files

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/%{preload_tpk_path}
install %{internal_name}-%{version}-%{target}%{build_mode}.tpk %{buildroot}/%{preload_tpk_path}/

%post

%files
%defattr(-,root,root,-)
%{preload_tpk_path}/*
