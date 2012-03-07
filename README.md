kcss
====

Description and Usage
---------------------
http://vishesh-yadav.com/blog/2012/02/23/change-kde-colorschemes-by-shortcuts/

Compile and Install
-------------------

kcss uses cmake. The compilation steps are similar to traditional configure,
make, install.

    $ cd ~/src/kcss
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install

We build under subdirectory 'build' to keep source tree clean from intermediate
binary and configuration files. You can build it directly on source tree as
well. 

