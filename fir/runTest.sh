#! /bin/bash

./fir ../test.fir -g
yasm -felf32 ../test.asm 
ld -melf_i386 -o test test.o -L$HOME/compiladores/root/usr/lib -lrts
./test