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
    
    or `su -c make install`

We build under subdirectory 'build' to keep source tree clean from intermediate
binary and configuration files. You can build it directly on source tree as
well. 

If you have trouble with cmake searching for KDE4 libraries, install `kdelibs-dev`.

Usage
-----
Create a .kcss-profilerc file in your `home` directory. You can use the one provided as an example.

  * `$ kcss list` Lists all the available schemes
  * `$ kcss toggle` Toggle from dark/light scheme
  * `$ kcss current` Displays the current scheme
  * `$ kcss switch <scheme-name>` Switch to <scheme-name>
