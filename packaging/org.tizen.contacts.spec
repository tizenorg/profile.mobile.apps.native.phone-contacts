Name:       org.tizen.contacts
Summary:    Phone Application and gadgets
Version:    0.1.0
Release:    0
Group:      Applications/Social
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: embryo-bin
BuildRequires: gettext

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
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(storage)
BuildRequires: pkgconfig(widget_service)

%description
The Phone Contacts application provides ability to manage contacts, logs, and
to start new calls.
Also, it provides ability to launch Contact and Call Settings applications.

%prep
%setup -q

%build
%define _app_exe_name             contacts
%define _app_home_dir             %{TZ_SYS_RO_APP}/%{name}
%define _app_icon_dir             %{TZ_SYS_RO_ICONS}/default/small
%define _app_share_packages_dir   %{TZ_SYS_RO_PACKAGES}
%define _app_license_dir          %{TZ_SYS_SHARE}/license

LDFLAGS+="-Wl,--rpath=%{_app_home_dir}/lib"; export LDFLAGS
cmake . \
        -DPKG_NAME=%{name} \
        -DEXE_NAME=%{_app_exe_name} \
        -DCMAKE_APP_HOME_DIR=%{_app_home_dir} \
        -DCMAKE_APP_ICON_DIR=%{_app_icon_dir} \
        -DCMAKE_APP_SHARE_PACKAGES_DIR=%{_app_share_packages_dir}


make %{?_smp_mflags} \
2>&1 | sed \
-e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' \
-e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'

%install
%make_install
%find_lang %{_app_exe_name}

%files -f %{_app_exe_name}.lang
%manifest main-app/%{name}.manifest
%license LICENSE

%{_app_home_dir}/bin
%{_app_home_dir}/lib
%{_app_home_dir}/shared

%{_app_home_dir}/res/common
%{_app_home_dir}/res/contacts
%{_app_home_dir}/res/dialer
%{_app_home_dir}/res/edje
%{_app_home_dir}/res/phone
%{_app_home_dir}/res/speeddial
%{_app_home_dir}/res/widget

%{_app_icon_dir}/%{name}.png
%{_app_icon_dir}/org.tizen.phone.png

%{_app_share_packages_dir}/%{name}.xml
