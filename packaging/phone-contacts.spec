Name:       org.tizen.contacts
Summary:    Phone Application and gadgets
Version:    0.1.189
Release:    1
Group:      devel
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

Requires(post): /sbin/ldconfig
Requires(post): /bin/chown
#Requires(post):  signing-client

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

%description
Phone application.

#package -n org.tizen.vcard-importer-lite
#Summary:    vcard importer
#Group:      Development/Libraries

#%description -n org.tizen.vcard-importer-lite
#vcard importer

%prep
%setup -q

%build
%define _app_home_dir   /usr/apps/%{name}
%define _app_icon_dir   /usr/share/icons/default/small

LDFLAGS+="-Wl,--rpath=%{_app_home_dir}/lib"; export LDFLAGS
cmake . -DPKGNAME=%{name} \
        -DCMAKE_APP_HOME_DIR=%{_app_home_dir} \
        -DCMAKE_APP_ICON_DIR=%{_app_icon_dir} \
%if "%{?sec_product_feature_display_resolution}" == "480x800"
-D_ENABLE_WVGA:BOOL=ON \
-D_ENABLE_HD:BOOL=OFF \
%else
-D_ENABLE_HD:BOOL=ON \
-D_ENABLE_WVGA:BOOL=OFF \
%endif


make %{?jobs:-j%jobs} \
2>&1 | sed \
-e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' \
-e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'

%install
rm -rf %{buildroot}

%make_install

###### for package signing step ######
%define tizen_sign 1
#%define tizen_sign_base /usr/apps/%{name};/usr/apps/org.tizen.vcard-importer
%define tizen_sign_base /usr/apps/%{name}
%define tizen_sign_level platform
%define tizen_author_sign 1
%define tizen_dist_sign 1

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/org.tizen.contacts

#%post -n org.tizen.vcard-importer-lite
#/usr/bin/signing-client/hash-signer-client.sh -a -d -p platform /usr/apps/org.tizen.vcard-importer


%post
#/usr/bin/signing-client/hash-signer-client.sh -a -d -p platform /usr/apps/%{name}
# Automatically added by dh_makeshlibs
if [ "$1" = "configure" ]; then
	ldconfig
fi
# End automatically added section

# 5000 is inhouse user id
# do not use relative path

mkdir -p /opt/usr/apps/%{name}/data
mkdir -p /opt/usr/apps/%{name}/shared/data

chown -R 5000:5000 /opt/usr/apps/%{name}/data
chown -R 5000:5000 /opt/usr/apps/%{name}/shared/data

#/sbin/ldconfig

%files
%manifest main-app/%{name}.manifest
%defattr(-,root,root,-)
%{_app_home_dir}/*
%{_app_icon_dir}/%{name}.png
%{_app_icon_dir}/org.tizen.phone.png
/usr/share/packages/%{name}.xml
/usr/share/license/%{name}
/etc/smack/accesses.d/%{name}.efl

#%files -n org.tizen.vcard-importer-lite
#%manifest main-app/org.tizen.vcard-importer.manifest
#%defattr(-,root,root,-)
#/usr/apps/org.tizen.vcard-importer/*
#/usr/share/packages/org.tizen.vcard-importer.xml
#/etc/smack/accesses2.d/org.tizen.vcard-importer.rule

