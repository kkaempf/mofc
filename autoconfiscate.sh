#!/bin/sh
aclocal --force &&
autoheader --force &&
libtoolize --force &&
automake -af &&
autoconf --force
