Name:           openstructure
Version:        1.1 
Release:        0.1d%{?dist}
Summary:        Open-source computational structural biology framework


# The entire source code is LGPLv3 except the following files:
# modules/db/src/sqlite3.h, modules/db/src/sqlite.c
# which have no license
License:        LGPLv3 and unlicensed    
URL:            www.openstructure.org 
# The source for this package was pulled from upstream's git.  Use the
# following commands to generate the tarball:
#  git clone https://dng.biozentrum.unibas.ch/git/ost.git openstructure-1.1d
#  cp /import/bc2/home/schwede/GROUP/OpenStructure/compounds.chemlib \
#     openstructure-1.1d/
#  tar -cJvf openstructure-1.1d.tar.xz openstructure-1.1d
Source0: %{name}-%{version}d.tar.xz       
Source1: %{name}.desktop

BuildRequires: boost-devel,fftw-devel,eigen2-devel,libtiff-devel,libpng-devel,sip-devel,PyQt4-devel,cmake,desktop-file-utils
Requires: sip,PyQt4     

%description
The Openstructure  project aims to provide an open-source, modular, flexible,
molecular modelling and visualization environment. It is targeted at
interested method developers in the field of structural bioinformatics.

%prep
%setup -n openstructure-1.1d

%build
%cmake . -DPREFIX=%{buildroot}/usr -DOPTIMIZE=ON -DCOMPOUND_LIB=compounds.chemlib
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install 
mkdir -p %{buildroot}/%{_defaultdocdir}/%{name}-%{version}
mkdir -p %{buildroot}/%{_datadir}/pixmaps
cp LICENSE.txt %{buildroot}/%{_defaultdocdir}/%{name}-%{version}/
cp ReadMe.txt %{buildroot}/%{_defaultdocdir}/%{name}-%{version}/
cp modules/gui/share/images/logo-small.png %{buildroot}/%{_datadir}/pixmaps/openstructure.png
rm %{buildroot}/%{_bindir}/ldt
desktop-file-install --dir=%{buildroot}/%{_datadir}/applications %{SOURCE1}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/libost*
%{_libdir}/openstructure
%{_datadir}/openstructure
%{_datadir}/applications/*
%{_datadir}/pixmaps/*

%doc
%{_defaultdocdir}/%{name}-%{version}/LICENSE.txt
%{_defaultdocdir}/%{name}-%{version}/ReadMe.txt

%changelog
* Wed Jul 20 2011 Valerio Mariani <valerio.mariani@unibas.ch> 1.1-1d
- Initial release




%package devel
Summary:        Open-source computational structural biology framework
Requires:       openstructure

%description devel
The Openstructure  project aims to provide an open-source, modular, flexible,
molecular modelling and visualization environment. It is targeted at
interested method developers in the field of structural bioinformatics.

%files devel
%{_includedir}/*

