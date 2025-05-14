Grupo 2: 

Gabriel Mousinho Daia
Gabriela Alves Da Silva Pinto
Gabriel Ribeiro Peres

Lab 3:

compiler.zip:

    main.c
    lex_process.c
    helpers/vector.c
    helpers/buffer.h
    helpers/buffer.c
    helpers/__MACOSX/._buffer.h
    helpers/__MACOSX/._vector.c
    helpers/__MACOSX/._buffer.c
    helpers/__MACOSX/._vector.h
    helpers/vector.h
    test.c
    lexer.c
    .vscode/settings.json
    node.c
    makefile
    outtest
    cprocess.c
    Testes.txt
    compiler.h
    build/lexer.o
    build/helpers/buffer.o
    build/helpers/vector.o
    build/compiler.o
    build/lex_process.o
    build/node.o
    build/cprocess.o
    build/parser.o
    compiler.c
    parser.c
    main

Realizados:

Atividade 1
    Tratamento de caso para identificação de Tokens:
        TOKEN_TYPE_KEYWORD 
        TOKEN_TYPE_NUMBER 
        TOKEN_TYPE_IDENTIFIER 
        TOKEN_TYPE_OPERATOR 
        TOKEN_TYPE_SYMBOL 
        TOKEN_TYPE_STRING 
        TOKEN_TYPE_COMMENT 
        TOKEN_TYPE_NEWLINE 

Atividade 2
    Realizar os testes que seguem
        //teste1

        #include <stdio.h>

        //teste de comentario

        int main() {
            printf("Hello World!");

            return 0;
        }

        /* teste de comentario 2 */

        ----------------------------
        Resultado:

        Compiladores - TURMA A - GRUPO 2

        SYMBL: `
        KEYWD: include
        STRNG: "stdio.h"
        NEWLN
        NEWLN
        CMNT:
        NEWLN
        NEWLN
        KEYWD: int
        NEWLN
        IDENT: main
        OPER: (
        SYMBL: �
        NEWLN
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        IDENT: printf
        OPER: (
        STRNG: "Hello World!"
        SYMBL: �
        SYMBL: 
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: return
        NEWLN
        NUMBR: 0
        SYMBL:  
        NEWLN
        SYMBL: �
        NEWLN
        NEWLN
        CMNT:
        Todos os arquivos foram compilados com sucesso!

        //teste 2

        #include <stdio.h>

        int main() {
            int a = 1;
            float b = 2;
            double c = 3;

            return a + b * c;
        }

        ----------------------------
        Resultado:

        Compiladores - TURMA A - GRUPO 2

        SYMBL: `
        KEYWD: include
        STRNG: "stdio.h"
        NEWLN
        NEWLN
        KEYWD: int
        NEWLN
        IDENT: main
        OPER: (
        SYMBL: �
        NEWLN
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: int
        NEWLN
        IDENT: a
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 1
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: float
        NEWLN
        IDENT: b
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 2
        SYMBL: p
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: double
        NEWLN
        IDENT: c
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 3
        SYMBL: 
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: return
        NEWLN
        IDENT: a
        NEWLN
        OPER: +
        NEWLN
        IDENT: b
        NEWLN
        OPER: *
        NEWLN
        IDENT: c
        SYMBL: �
        NEWLN
        SYMBL: �
        Todos os arquivos foram compilados com sucesso!

        //teste 3

        #include <stdio.h>
        #include <stdlib.h>

        int main() {
            int a = 1;
            float b = 2;
            double c = 3;

            //teste de comentario

            return (a + b) / c;
        }

        ----------------------------
        Resultado:

        Compiladores - TURMA A - GRUPO 2

        SYMBL: `
        KEYWD: include
        STRNG: "stdio.h"
        NEWLN
        SYMBL: `
        KEYWD: include
        STRNG: "stdlib.h"
        NEWLN
        NEWLN
        KEYWD: int
        NEWLN
        IDENT: main
        OPER: (
        SYMBL: �
        NEWLN
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: int
        NEWLN
        IDENT: a
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 1
        SYMBL:  
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: float
        NEWLN
        IDENT: b
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 2
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: double
        NEWLN
        IDENT: c
        NEWLN
        OPER: =
        NEWLN
        NUMBR: 3
        SYMBL: �
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        CMNT:
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        NEWLN
        KEYWD: return
        NEWLN
        OPER: (
        IDENT: a
        NEWLN
        OPER: +
        NEWLN
        IDENT: b
        SYMBL: p
        NEWLN
        OPER: /
        NEWLN
        IDENT: c
        SYMBL: �
        NEWLN
        SYMBL: p
        Todos os arquivos foram compilados com sucesso!

Atividade 3
    Implementar e converter:
    ❑ Números hexadecimais. Ex: int a = 0xFFFF.
    ❑ Números binários: Ex: int a = 0b1010.

Atividade 4
    ❑ Criar os arquivos parser.c e node.c
    ❑ Adicionar os arquivos no Makefile e compilar corretamente