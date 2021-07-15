#! /bin/bash

defaultDir=../tests
files=($(ls -p $defaultDir/$1 | grep ".*\.fir$"))

for file in "${files[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/$1/$filename.fir  -g
    yasm -felf32 $defaultDir/$1/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/$1/$filename.out
    diff -b -B -w $defaultDir/$1/$filename.out $defaultDir/$1-expected/$filename.out
    echo

done