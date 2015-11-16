# create a working directory for installation
cd $HOME
mkdir src
cd $HOME/src

# download required apt packages
sudo apt-get install axel bison flex make gcc texinfo g++

# create a directory for ftp packages
mkdir raw
cd $HOME/src/raw

# download required ftp packages
axel ftp://ftp.gnu.org/gnu/gcc/gcc-5.2.0/gcc-5.2.0.tar.gz -a -n 100
axel ftp://ftp.gnu.org/gnu/binutils/binutils-2.25.1.tar.gz -a -n 100
axel http://isl.gforge.inria.fr/isl-0.12.tar.gz -a -n 100

# unarchive the downloaded compressed files
tar -xvf gcc-5.2.0.tar.gz
tar -xvf binutils-2.25.1.tar.gz
tar -xvf isl-0.12.tar.gz

# download additions for gcc
gcc-5.2.0/contrib/download_prerequisites

# delete junk folders created by download_prerequisites
rm -rf gmp isl mpc mpfr isl-0.14.tar.gz isl-0.14

# move downloaded folders out of the raw directory into the correct folders
mv gmp-4.3.2 ../gcc-5.2.0/gmp
mv mpc-0.8.1 ../gcc-5.2.0/mpc
mv mpfr-2.4.2 ../gcc-5.2.0/mpfr

# leave the raw directory
cd $HOME/src

# move the isl folder into gcc then duplicate it and move a copy into binutils
mv isl-0.12 gcc-5.2.0/isl
tar -xvf /raw/isl-0.12.tar.gz
mv isl-0.12 binutils-2.25.1/isl

# create the build folders
mkdir build-gcc
mkdir build-bintools

# export some variables for the bintools and gcc install
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# enter the build-bintools folder
cd $HOME/src/build-bintools

# generate the bintools makefile and run it
../binutils-2.25.1/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

# leave the build bintools directory and enter the build gcc directory to begin gcc install
cd $HOME/src/build-gcc

# generate the makefile for gcc and run it
../gcc-5.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

# tell the user that this worked
echo 'Installed Q OS Cross-Compiler on your Linux System Successfully!'
