#!/bin/sh
#This is a SAMPLE and is part of the git repo.  Make a copy for your own use.
#It is to help you get started with building in a BoringSSL environment.
#
# To obtain picotls and run it with boringssl
#git clone https://github.com/h2o/picotls.git
# or
#git clone https://github.com/rperper/picotls.git
#
#cd picotls
#git submodule init
#git submodule update
rm CMakeCache.txt
rm -rf CMakeFiles
cmake -DBORING_ROOT=/home/user/proj/thirdparty/boringssl -DBORING_LIBRARY_DIR=/home/user/proj/thirdparty/lib -DBROTLI_ROOT=/home/user/proj/thirdparty/brotli -DBROTLI_DEC=/home/user/proj/thirdparty/lib/libbrotlidec-static.a -DBROTLI_ENC=/home/user/proj/thirdparty/lib/libbrotlienc-static.a .
make

