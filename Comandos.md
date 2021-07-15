# Comandos de Compilação e Execução

Importar / Clonar repositorio CVS do projeto (Máquina Virtual Ubuntu):

    $ cd Projeto

    $ export CVS_RSH=/bin/ssh
    $ sudo apt-get install cvs
    $ cvs -d ist1xxxxx@sigma.ist.utl.pt:/afs/ist.utl.pt/groups/leic-co/co21/a/cvs/yyy co fir

    x -> número aluno
    y -> número grupo

Obter bibliotecas necessárias ao desenvolvimento do projeto (fora da pasta "fir" do projeto, por exemplo, pasta anterior):

    Obter zips:
        $ wget https://web.tecnico.ulisboa.pt/~david.matos/w/pt/images/9/9c/Libcdk16-202103030011.tar.bz2
        $ wget https://web.tecnico.ulisboa.pt/~david.matos/w/pt/images/b/be/Librts5-202103031636.tar.bz2

    Extrair zips:
        $ tar xvf Libcdk16-202103030011.tar.bz2
        $ tar xvf Librts5-202103031636.tar.bz2

    Instalar cdk (requer obtenção de doxygen):
        $ sudo apt install doxygen
        $ cd libcdk16-202103030011
        $ make clean
        $ make
        $ make install

    $ cd ..

    Installar rts (requer obtenção de yasm):
        $ sudo apt install yasm
        $ cd librts5-202103031636
        $ make clean
        $ make
        $ make install

    Obter flex:
        $ sudo apt install flex

    Obter bison:
        $ sudo apt install bison

Definir classpath do projeto para o cdk obtido:

    $ cd fir
    $ PATH=$HOME/compiladores/root/usr/bin:$PATH


Compilar projeto:

    $ cd fir
    $ make clean
    $ make

Correr um teste em XML:

    $ ./fir tests/A-02-2-N-ok.fir --target xml
    
    
Correr um teste em ASM:

    $ ./fir ../test.fir
    $ yasm -felf32 ../test.asm
    $ ld -melf_i386 -o test test.o -L$HOME/compiladores/root/usr/lib -lrts

Ativar debug parser y:

    Primeira linha depois de %% (linha 26) para a definição dos tokens
    { yydebug=1; }

Ativar debug parser y com tokens scanner l:

    Primeira linha depois de %% (linha 26) para a definição dos tokens
    { yydebug=1; }

Ativar debug scanner l:

    $ export YYDEBUG=1
    Primeira linha fir_scanner.l depois de %% (linha 26) para a definição dos tokens
        { yydebug=1; set_debug(true); }
    Segunda linha fir_scanner.l adicionar debug no fim
        %option stack noyywrap yylineno 8bit debug
