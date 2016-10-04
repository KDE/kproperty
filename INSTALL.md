# Installation Instructions

1. Extract the source
    tar -xf kproperty-{version}.tar.xz

2. Create a build directory
    mkdir kproperty-build
    cd kproperty-build

3. Run cmake to configure the build
    cmake ../kproperty-{version}

4. Run make to build the library
    make

5. Run make install to install (as root)
    sudo make install
