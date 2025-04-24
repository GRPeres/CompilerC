#ifndef COMPILER_H
#define COMPILER_H
#define NUMERIC_CASE \
    case '0':\
    case '1':\
    case '2':\
    case '3':\
    case '4':\
    case '5':\
    case '6':\
    case '7':\
    case '8':\
    case '9'

#define SYMBOL_CASE \
    case 0x7B: /* '{' */ \
    case 0x7D: /* '}' */ \
    case 0x3A: /* ':' */ \
    case 0x3B: /* ';' */ \
    case 0x23: /* '#' */ \
    case 0x5C: /* '\\' */ \
    case 0x29: /* ')' */ \
    case 0x5D /* ']' */

#define OPERATOR_CASE \
    case '+':\
    case '-':\
    case '*':\
    case '>':\
    case '<':\
    case '^':\
    case '%':\
    case '!':\
    case '=':\
    case '~':\
    case '|':\
    case '&':\
    case '(':\
    case '[':\
    case ',':\
    case '.':\
    case '?'

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//LAB 1 - DEF
enum{
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};

//LAB 2 - DEF
struct pos {
    int line;
    int col;
    const char* filename;
};

enum {
    LEXICAL_ANALYSIS_ALL_OK,
    LEXICAL_ANALYSIS_IMPUT_ERROR
};

enum {
    TOKEN_TYPE_KEYWORD , // Ribeiro
    TOKEN_TYPE_NUMBER , // Feito
    TOKEN_TYPE_IDENTIFIER , // Peres
    TOKEN_TYPE_OPERATOR , // Mousinho
    TOKEN_TYPE_SYMBOL , // Feito
    TOKEN_TYPE_STRING , // Feito
    TOKEN_TYPE_COMMENT , // Gabriela
    TOKEN_TYPE_NEWLINE //Feito
};

struct compile_process {
    // Como o arquivo deve ser compilado
    int flags;

    struct pos pos;

    struct compile_process_input_file{
        FILE* fp;
        const char* abs_path;
    } cfile;

    FILE* ofile;
};

int compile_file(const char* filename, const char* out_filename, int flags);
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags);

#endif

// BEGIN - LAB 2


struct token{
    int type;
    int flags;

    struct pos pos;//Indentifica local do token no arquivo

    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;

    };
    // Sera 'true' se tiver um espaço entre um toaken e o proximo token
    bool whitespace;

    // Retira a string que estiver dentro de parenteses. Ex: (1+2+3) resulta em 1+2+3
    const char* between_brackets;
};

struct lex_process;

// Definicao de ponteiros para funcoes.
typedef char (*LEX_PROCESS_NEXT_CHAR) (struct lex_process* process);
typedef char (*LEX_PROCESS_PEEK_CHAR) (struct lex_process* process);
typedef char (*LEX_PROCESS_PUSH_CHAR) (struct lex_process* process, char c);

struct lex_process_functions {
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
    struct pos pos;
    struct vector* token_vec;
    struct compile_process* compiler;


    int current_expression_count; //Qts parenteses existem no momento.

    struct buffer* parentheses_buffer;
    struct lex_process_functions* function;

    void* private; //Dados privados que o lexer nao entende mas o programador entende
};

//Funcoes do arquivo cprocess.c
char compile_process_next_char(struct lex_process* lex_process);
char compile_process_peek_char(struct lex_process* lex_process);
char compile_process_push_char(struct lex_process* lex_process, char c);

//Funcoes do arquivo lex_process.c
struct lex_process* lex_process_create(struct compile_process* compiler, struct lex_process_functions* functions, void *private);
void lex_process_free(struct lex_process* process);
void* lex_process_private(struct lex_process* process);
struct vector* lex_process_tokens(struct lex_process* process);

//Funcoes do arquivo lexer.c
int lex(struct lex_process* process);

// Error handling functions
void compiler_error(struct compile_process* compiler, const char* msg, ...);
void compiler_warning(struct compile_process* compiler, const char* msg, ...);
