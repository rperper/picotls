#!/bin/sh
#This is a SAMPLE and is part of the git repo.  Make a copy for your own use.
#It is to help you get started with building in a BoringSSL environment.
#
rm CMakeCache.txt
rm -rf CMakeFiles
cmake -DOPENSSL_ROOT_DIR=/home/user/proj2/openssl -DOPENSSL_USE_STATIC_LIBS=TRUE .
make

