# set some export variables to quickly naviagate between files
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# make and enter the src directory
cd /home/raph
sudo mkdir src
cd src

# download packages to support 32 bit headers 
sudo dnf -y install alsa-lib.i686 alsa-plugins-oss.i686 alsa-plugins-pulseaudio.i686 alsa-plugins-pulseaudio.i686 arts.i686 audiofile.i686 bzip2-libs.i686 cairo.i686 cdk.i686 compat-expat1.i686 compat-libstdc++-33.i686 cyrus-sasl-lib.i686 dbus-libs.i686 esound-libs.i686 fltk.i686 freeglut.i686 glibc.i686 gtk2.i686 imlib.i686 lcms-libs.i686 lesstif.i686 libacl.i686 libao.i686 libattr.i686 libcap.i686 libdrm.i686 libexif.i686 libgnomecanvas.i686 libICE.i686 libieee1284.i686 libsigc++20.i686 libSM.i686 libtool-ltdl.i686 libusb.i686 libwmf-lite.i686 libwmf.i686 libX11.i686 libXau.i686 libXaw.i686 libXcomposite.i686 libXdamage.i686 libXdmcp.i686 libXext.i686 libXfixes.i686 libxkbfile.i686 libxml2.i686 libXmu.i686 libXp.i686 libXpm.i686 libXScrnSaver.i686 libXScrnSaver.i686 libxslt.i686 libXt.i686 libXtst.i686 libXv.i686 libXv.i686 libXxf86vm.i686 lzo.i686 mesa-libGL.i686 mesa-libGLU.i686 nas-libs.i686 nspluginwrapper.i686 openal-soft.i686 openldap.i686 pam.i686 popt.i686 pulseaudio-libs-glib2.i686 pulseaudio-libs.i686 pulseaudio-libs.i686 qt-x11.i686 qt.i686 redhat-lsb.i686 sane-backends-libs.i686 SDL.i686 svgalib.i686 unixODBC.i686 zlib.i686

# download packages for gcc and binutils
sudo dnf -y install bison flex make gcc texinfo nano kate insight

# download binutils and gcc from the ftp mirrors
wget ftp://ftp.gnu.org/gnu/gcc/gcc-5.2.0/gcc-5.2.0.tar.gz
wget ftp://ftp.gnu.org/gnu/binutils/binutils-2.25.1.tar.gz

# unarchive the downloaded binutils and gcc files
tar -xvf gcc-5.2.0.tar.gz
tar -xvf binutils-2.25.1.tar.gz

# download gmp, mpfr, mpc and isl from the file included in the gcc download
gcc-5.2.0/contrib/download_prerequisites

# delete link files created by download_prerequisites
sudo rm isl mpc mpfr gmp

# move the isl-0.14 folder into binutils-2.25.1/isl so that it can be detected by the makefile
mv isl-0.14 binutils-2.25.1/isl

# duplicate the isl folder because we need it for bintools and gcc
tar -xvf isl-0.14.tar.bz2

# move gmp, mpc, mpfr and isl packages into the gcc-5.2.0 folder so the makefile detects them
mv isl-0.14 gcc-5.2.0/isl
mv mpc-0.8.1 gcc-5.2.0/mpc
mv mpfr-2.4.2 gcc-5.2.0/mpfr
mv gmp-4.3.2 gcc-5.2.0/gmp

# delete the unarchived versions of isl, mpc, mpfr, gcc, gmp and binutils to save space
rm binutils-2.25.1.tar.gz
rm gcc-5.2.0.tar.gzz
rm gmp-4.3.2.tar.bz2
rm isl-0.14.tar.bz2
rm mpc-0.8.1.tar.gz
rm mpfr-2.4.2.tar.bz2

# ensure we are in the correct directory before installing binutils
cd $HOME/src

# make a folder to store binutils installation data and enter it
mkdir build-binutils
cd build-binutils

# generate the binutils makefile
../binutils-2.25.1/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

# run the makefile and install binutils
make
make install

# set the some variables for the gcc installation
which -- $TARGET-as || echo $TARGET-as is not in the PATH

# create and enter the directory for installing gcc
mkdir build-gcc
cd build-gcc

# generate the makefile for the gcc installation
../gcc-5.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers

# run only the parts of the gcc makefile that we need for Q OS
make all-gcc
make all-target-libgcc

# install the now-built makefiles
make install-gcc
make install-target-libgcc
