#!/bin/sh

for i in `find $1 -name "*.mof"`
do
  echo $i 2>&1
  ./testmoflex < $i 2>&1
done