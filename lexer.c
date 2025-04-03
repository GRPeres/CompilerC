#include "compiler.h"
#include <string.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"

#define LEX_GETC_IF(buffer, c, exp) \
    for (c = peekc(); exp; c = peekc()){ \
        buffer_write(buffer, c); \
        nextc(); \
    }

char symbol_array[8] = {'{', '}', ':', ';', '#', '\\', ')', ']'};

struct token* read_next_token();
static struct lex_process* lex_process;
static struct token tmp_token;

//Pegar um caracter do arquivo, sem mudar a posicao de leitura.
static char peekc(){
    return lex_process->function->peek_char(lex_process);
}

static char nextc() {
    char c = lex_process->function->next_char(lex_process);
    lex_process->pos.col += 1;
    if (c == '\n') {
        lex_process->pos.line += 1;
        lex_process->pos.col = 1;
    }
    return c;
}

// Adicionar um caracter no arquivo.
static void pushc(char c){
    lex_process->function->push_char(lex_process, c);
}

static struct pos lex_file_position(){
    return lex_process->pos;
}

struct token* token_create(struct token* _token) {
    memcpy(&tmp_token, _token, sizeof(struct token));
    tmp_token.pos = lex_file_position();

    return &tmp_token;
}

static struct token* lexer_last_token() {
    return vector_back_or_null(lex_process->token_vec);
}

static struct token* handle_whitespace() {
    struct token* last_token = lexer_last_token();
    if (last_token) {
        last_token->whitespace = true;
    }

    nextc();
    return read_next_token();
}

const char* read_number_str() {
    const char* num = NULL;
    struct buffer* buffer = buffer_create();
    char c = peekc();
    LEX_GETC_IF(buffer, c, (c >= '0' && c <= '9'));

    // Finalizar a string
    buffer_write(buffer, 0x00);

    printf("Token: %s\n",buffer->data);
    //Retorna o ponteiro para o buffer.
    return buffer_ptr(buffer);
}

unsigned long long read_number(){
    const char* s = read_number_str();
    return atoll(s);
}

struct token* token_make_number_for_value(unsigned long number){
    return token_create(&(struct token){.type=TOKEN_TYPE_NUMBER, .llnum=number});
}

struct token* token_make_number(){
    return token_make_number_for_value(read_number());
}

const char* read_string() {
    struct buffer* buffer = buffer_create();
    char c = peekc();

    nextc();

    while ((c = peekc()) != '"' && c != EOF) {
        if (c == '\\') {
            nextc(); 
            c = peekc();
            if (c == EOF) break;
        }
        buffer_write(buffer, c);
        nextc();
    }

    if (c != '"') {
        return NULL;
    }
    nextc();

    buffer_write(buffer, 0x00);
    
    printf("Token: %s\n",buffer->data);
    return buffer_ptr(buffer);
}

struct token* token_make_string_for_value(const char* sval) {
    return token_create(&(struct token){
        .type = TOKEN_TYPE_STRING,
        .sval = sval 
    });
}

struct token* token_make_string() {
    const char* s = read_string();
    if (!s) return NULL;
    return token_make_string_for_value(s);
}

const char* read_symbol_str() {
    const char* sym = NULL;
    struct buffer* buffer = buffer_create();
    char c = peekc();
    int bol = 0;
    for (int i = 0; i < sizeof(symbol_array); i++)
    {
        if (c == symbol_array[i])
        {
            bol = 1;
        }
    }
    printf("CHEGUEIIIIIIIIIIII");
    LEX_GETC_IF(buffer, c, bol);

    // Finalizar a string
    buffer_write(buffer, 0x00);

    printf("Token: %s\n",buffer->data);
    //Retorna o ponteiro para o buffer.
    return buffer_ptr(buffer);
}

const char* read_symbol(){
    const char* s = read_symbol_str();
    return s;
}

struct token* token_make_symbol_for_value(const char* symbol){
    return token_create(&(struct token){.type=TOKEN_TYPE_SYMBOL, .sval=symbol});
}

struct token* token_make_symbol(){
    return token_make_symbol_for_value(read_symbol());
}

//Funcao responsavel por ler o proximo token do arquivo.
struct token* read_next_token(){
    struct token* token = NULL;
    char c = peekc();
    printf("Character: %c (ASCII: %d)\n", c, c);
    switch (c)
    {
    case EOF:
        // Fim do arquivo. 
        break;

    NUMERIC_CASE:
        token = token_make_number();
        break;

    case ' ':
    case '\t':
        token = handle_whitespace();
        break;

    SYMBOL_CASE:
        printf("eu entrei juro");
        token = token_make_symbol();
        break;

    case '"':
        token = token_make_string();
        break;

    case '\n':
        token = handle_whitespace();
        break;

    default: 
        // compiler_error(lex_process->compiler, "Token invalido!\n");
        break;
    }
    return token;
}

int lex(struct lex_process* process){
    process -> current_expression_count = 0;
    process -> parentheses_buffer = NULL;
    lex_process = process;
    process -> pos.filename = process->compiler->cfile.abs_path;

    struct token* token = read_next_token();

    //ler todos os token do arquivo do imput
    while (token)
    {
        vector_push(process->token_vec,token);
        token = read_next_token();
    }
    
    return LEXICAL_ANALYSIS_ALL_OK;
}