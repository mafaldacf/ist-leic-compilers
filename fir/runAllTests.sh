#! /bin/bash

defaultDir=../tests
filesA=($(ls -p $defaultDir/A | grep ".*\.fir$"))
filesB=($(ls -p $defaultDir/B | grep ".*\.fir$"))
filesC=($(ls -p $defaultDir/C | grep ".*\.fir$"))
filesD=($(ls -p $defaultDir/D | grep ".*\.fir$"))
filesE=($(ls -p $defaultDir/E | grep ".*\.fir$"))
filesF=($(ls -p $defaultDir/F | grep ".*\.fir$"))
filesG=($(ls -p $defaultDir/G | grep ".*\.fir$"))
filesH=($(ls -p $defaultDir/H | grep ".*\.fir$"))
filesI=($(ls -p $defaultDir/H | grep ".*\.fir$"))
filesJ=($(ls -p $defaultDir/J | grep ".*\.fir$"))
filesK=($(ls -p $defaultDir/K | grep ".*\.fir$"))
filesM=($(ls -p $defaultDir/M | grep ".*\.fir$"))
filesO=($(ls -p $defaultDir/O | grep ".*\.fir$"))
filesP=($(ls -p $defaultDir/P | grep ".*\.fir$"))

for file in "${filesA[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/A/$filename.fir  -g
    yasm -felf32 $defaultDir/A/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/A/$filename.out
    diff -b -B -w $defaultDir/A/$filename.out $defaultDir/A-expected/$filename.out
    echo

done

for file in "${filesB[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/B/$filename.fir  -g
    yasm -felf32 $defaultDir/B/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/B/$filename.out
    diff -b -B -w $defaultDir/B/$filename.out $defaultDir/B-expected/$filename.out
    echo

done

for file in "${filesC[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/C/$filename.fir  -g
    yasm -felf32 $defaultDir/C/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/C/$filename.out
    diff -b -B -w $defaultDir/C/$filename.out $defaultDir/C-expected/$filename.out
    echo

done

for file in "${filesD[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/D/$filename.fir  -g
    yasm -felf32 $defaultDir/D/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/D/$filename.out
    diff -b -B -w $defaultDir/D/$filename.out $defaultDir/D-expected/$filename.out
    echo

done

for file in "${filesE[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/E/$filename.fir  -g
    yasm -felf32 $defaultDir/E/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/E/$filename.out
    diff -b -B -w $defaultDir/E/$filename.out $defaultDir/E-expected/$filename.out
    echo

done

for file in "${filesF[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/F/$filename.fir  -g
    yasm -felf32 $defaultDir/F/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/F/$filename.out
    diff -b -B -w $defaultDir/F/$filename.out $defaultDir/F-expected/$filename.out
    echo

done

for file in "${filesG[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/G/$filename.fir  -g
    yasm -felf32 $defaultDir/G/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/G/$filename.out
    diff -b -B -w $defaultDir/G/$filename.out $defaultDir/G-expected/$filename.out
    echo

done

for file in "${filesH[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/H/$filename.fir  -g
    yasm -felf32 $defaultDir/H/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/H/$filename.out
    diff -b -B -w $defaultDir/H/$filename.out $defaultDir/H-expected/$filename.out
    echo

done

for file in "${filesI[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/I/$filename.fir  -g
    yasm -felf32 $defaultDir/I/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/I/$filename.out
    diff -b -B -w $defaultDir/I/$filename.out $defaultDir/I-expected/$filename.out
    echo

done

for file in "${filesJ[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/J/$filename.fir  -g
    yasm -felf32 $defaultDir/J/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/J/$filename.out
    diff -b -B -w $defaultDir/J/$filename.out $defaultDir/J-expected/$filename.out
    echo

done

for file in "${filesK[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/K/$filename.fir  -g
    yasm -felf32 $defaultDir/K/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/K/$filename.out
    diff -b -B -w $defaultDir/K/$filename.out $defaultDir/K-expected/$filename.out
    echo

done

for file in "${filesM[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/M/$filename.fir  -g
    yasm -felf32 $defaultDir/M/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/M/$filename.out
    diff -b -B -w $defaultDir/M/$filename.out $defaultDir/M-expected/$filename.out
    echo

done

for file in "${filesO[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/O/$filename.fir  -g
    yasm -felf32 $defaultDir/O/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/O/$filename.out
    diff -b -B -w $defaultDir/O/$filename.out $defaultDir/O-expected/$filename.out
    echo

done

for file in "${filesP[@]}"
do  
    filename="${file%.*}"
    tput setaf 3; echo $filename
    tput sgr0;
    ./fir $defaultDir/P/$filename.fir  -g
    yasm -felf32 $defaultDir/P/$filename.asm
    ld -melf_i386 -o test $filename.o -L$HOME/compiladores/root/usr/lib -lrts
    ./test > $defaultDir/P/$filename.out
    diff -b -B -w $defaultDir/P/$filename.out $defaultDir/P-expected/$filename.out
    echo

done