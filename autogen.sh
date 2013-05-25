#!/bin/sh

rm -rf ./autom4te.cache Makefile.in aclocal.m4 config.guess \
    config.h.in config.sub configure depcomp install-sh \
    config.status config.log ltmain.sh missing


echo "Generating configure files... may take a while."

autoreconf --install --force && \
  echo "Preparing was successful if there was no error messages above." && \
  echo "Now type:" && \
  echo "  ./configure && make"  && \
  echo "Run './configure --help' for more information"
