#!/bin/sh

for i in `find $1 -name "*.mof"`
do
  ./mofc -v $MOFINCLUDE $i 2>&1
done