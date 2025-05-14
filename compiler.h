#ifndef COMPILER_H
#define COMPILER_H

#define S_EQ(a, b) (strcmp((a), (b)) == 0)

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

enum{
NODE_TYPE_EXPRESSION, 
NODE_TYPE_EXPRESSION_PARENTHESES, 
NODE_TYPE_NUMBER, 
NODE_TYPE_IDENTIFIER,
NODE_TYPE_STRING, 
NODE_TYPE_VARIABLE,
NODE_TYPE_VARIABLE_LIST,
NODE_TYPE_FUNCTION ,
NODE_TYPE_BODY, 
NODE_TYPE_STATEMENT_RETURN,
NODE_TYPE_STATEMENT_IF,
NODE_TYPE_STATEMENT_ELSE, 
NODE_TYPE_STATEMENT_WHILE, 
NODE_TYPE_STATEMENT_DO_WHILE,
NODE_TYPE_STATEMENT_FOR, 
NODE_TYPE_STATEMENT_BREAK, 
NODE_TYPE_STATEMENT_CONTINUE, 
NODE_TYPE_STATEMENT_SWITCH,
NODE_TYPE_STATEMENT_CASE,
NODE_TYPE_STATEMENT_DEFAULT,
NODE_TYPE_STATEMENT_GOTO ,
NODE_TYPE_UNARY ,
NODE_TYPE_TENARY ,
NODE_TYPE_LABEL ,
NODE_TYPE_STRUCT,
NODE_TYPE_UNION ,
NODE_TYPE_BRACKET, 
NODE_TYPE_CAST ,
NODE_TYPE_BLANK,

};

enum{
    PARSE_ALL_OK, 
    PARSE_GENERAL_ERROR
 };

// Cada nó uma parte do inputfile.
struct node {int type;
    int flags;
    struct pos pos;
    struct node_binded {
        // Ponteiro para o body node.
        struct node* owner;

        // Ponteiro para a funcao que o node esta.
        struct node* funtion;
        } binded;

        // Estrutura similar ao token
        union { char cval;
            const char *sval;
            unsigned int inum;
            unsigned long lnum;
            unsigned long long llnum;
            void* any;
        };
        union { struct exp {
            struct node* left;
            struct node* right;
            const char* op;
        } exp;
    };
};

struct compile_process {
    // Como o arquivo deve ser compilado
    int flags;

    struct pos pos;

    struct compile_process_input_file{
        FILE* fp;
        const char* abs_path;
    } cfile;

    struct vector* token_vec;
    struct vector* node_vec;
    struct vector* node_tree_vec;
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

enum {
    NODE_FLAG_INSIDE_EXPRESSION = 0b00000001
};
/* FUNCOES DO ARQUIVO PARSER.C */
int parse(struct compile_process* process);

/* FUNCOES DO ARQUIVO TOKEN.C */
bool token_is_keyword(struct token* token, const char* value);
bool token_is_symbol(struct token* token, const char value);
bool discart_token(struct token* token);

/* FUNCOES DO ARQUIVO NODE.C */
void node_set_vector(struct vector* vec, struct vector* root_vec);
void node_push(struct node* node);

struct node* node_peek_or_null();
struct node* node_peek();
struct node* node_pop();
struct node* node_peek_expressionable_or_null();

bool node_is_expressionable(struct node* node);
void make_exp_node(struct node* node_left, struct node* node_right, const char* op);
struct node* node_create(struct node* _node);

#define TOTAL_OPERADOR_GROUPS 14
#define MAX_OPERATORS_IN_GROUP 12

enum { ASSOCIATIVITY_LEFT_TO_RIGTH,
    ASSOCIATIVITY_RIGHT_TO_LEFT
};
struct expressionable_op_precedence_group {
    char* operators[MAX_OPERATORS_IN_GROUP];
    int associativity;
};