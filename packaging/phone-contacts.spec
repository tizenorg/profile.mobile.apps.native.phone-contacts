Name:       org.tizen.contacts
Summary:    Phone Application and gadgets
Version:    0.1.189
Release:    1
Group:      Applications/Social
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

Requires(post): /sbin/ldconfig
Requires(post): /bin/chown

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: embryo-bin
BuildRequires: gettext-devel
BuildRequires: hash-signer
BuildRequires: sqlite

BuildRequires: pkgconfig(accounts-svc)
BuildRequires: pkgconfig(badge)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-preference)
BuildRequires: pkgconfig(capi-appfw-widget-application)
BuildRequires: pkgconfig(capi-base-utils-i18n)
BuildRequires: pkgconfig(capi-media-metadata-extractor)
BuildRequires: pkgconfig(capi-telephony)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(storage)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(widget_service)
BuildRequires: pkgconfig(libtzplatform-config)

%description
Phone-Contacts application.

%prep
%setup -q

%build
%define _app_home_dir             %{TZ_SYS_RO_APP}/%{name}
%define _app_icon_dir             %{TZ_SYS_RO_ICONS}/default/small
%define _app_share_packages_dir   %{TZ_SYS_RO_PACKAGES}
%define _app_smack_dir            %{TZ_SYS_SMACK}/accesses.d
%define _app_license_dir          %{TZ_SYS_SHARE}/license

LDFLAGS+="-Wl,--rpath=%{_app_home_dir}/lib"; export LDFLAGS
cmake . -DPKGNAME=%{name} \
        -DCMAKE_APP_HOME_DIR=%{_app_home_dir} \
        -DCMAKE_APP_ICON_DIR=%{_app_icon_dir} \
        -DCMAKE_APP_SHARE_PACKAGES_DIR=%{_app_share_packages_dir} \
        -DCMAKE_APP_SMACK_DIR=%{_app_smack_dir}


make %{?jobs:-j%jobs} \
2>&1 | sed \
-e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' \
-e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'

%install
rm -rf %{buildroot}

%make_install

mkdir -p %{buildroot}%{_app_license_dir}
cp LICENSE %{buildroot}%{_app_license_dir}/%{name}


%post
# Automatically added by dh_makeshlibs
if [ "$1" = "configure" ]; then
	ldconfig
fi
# End automatically added section

#/sbin/ldconfig

%files
%manifest main-app/%{name}.manifest
%defattr(-,root,root,-)
%{_app_home_dir}/*
%{_app_icon_dir}/%{name}.png
%{_app_icon_dir}/org.tizen.phone.png
%{_app_share_packages_dir}/%{name}.xml
%{_app_license_dir}/%{name}
%{_app_smack_dir}/%{name}.efl

