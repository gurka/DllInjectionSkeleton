# DllInjectionSkeleton
Just a very simple dll injection skeleton, including an injector and a dll to inject


# Building
This assumes that you have TDM-GCC (GCC 5.1.0), CMake (>= 3.0) and a healthy Bash environment (TDM-GCC and CMake in $PATH):

    $ cd DllInjectorSkeleton
    $ ./cmake.sh all
    
    ### To build debug
    $ cd build_debug
    $ mingw32-make.exe
    
    ### To build release
    $ cd build_release
    $ mingw32-make.exe
