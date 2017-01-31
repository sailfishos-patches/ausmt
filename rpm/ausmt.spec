Name:       ausmt

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    AUSMT
Version:    1.3.0
Release:    1
Group:      Qt/Qt
License:    TODO
URL:        http://github.com/sailfishos-patches/ausmt
Source0:    %{name}-%{version}.tar.bz2
Requires:   rpm >= 4.9.0
Requires:   patchutils
Requires:   patch
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Test)

%description
AUSMT stands for Auto-Update System Modification Technology. AUSMT enables
patching of webOS system safely, handling the situations like OTA updates
etc.

This modified AUSMT is used by patchmanager in SailfishOS to perform system
file patching, just like with Preware on webOS.


%package tests
Summary:    Tests for AUSMT
License:    TODO
Group:      Qt/Qt
Requires:   %{name} = %{version}-%{release}

%description tests
A set of tests for AUSMT

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%postun
rm -rf /opt/ausmt/tests/var

%files
%defattr(-,root,root,-)
/opt/ausmt/ausmt-install
/opt/ausmt/ausmt-remove
/opt/ausmt/ausmt-verify

%files tests
%defattr(-,root,root,-)
/opt/ausmt/tests
