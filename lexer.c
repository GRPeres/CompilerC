#include "compiler.h"
#include "helpers/vector.h"
#include "helpers/buffer.h"

#include <string.h>
#include <ctype.h>

#define S_EQ(a, b) (strcmp((a), (b)) == 0)

bool is_keyword(const char* str) {
    return S_EQ(str, "unsigned") ||
           S_EQ(str, "signed") ||
           S_EQ(str, "char") ||
           S_EQ(str, "short") ||
           S_EQ(str, "int") ||
           S_EQ(str, "long") ||
           S_EQ(str, "float") ||
           S_EQ(str, "double") ||
           S_EQ(str, "void") ||
           S_EQ(str, "struct") ||
           S_EQ(str, "union") ||
           S_EQ(str, "static") ||
           S_EQ(str, "__ignore_typecheck") ||
           S_EQ(str, "return") ||
           S_EQ(str, "include") ||
           S_EQ(str, "sizeof") ||
           S_EQ(str, "if") ||
           S_EQ(str, "else") ||
           S_EQ(str, "while") ||
           S_EQ(str, "for") ||
           S_EQ(str, "do") ||
           S_EQ(str, "break") ||
           S_EQ(str, "continue") ||
           S_EQ(str, "switch") ||
           S_EQ(str, "case") ||
           S_EQ(str, "default") ||
           S_EQ(str, "goto") ||
           S_EQ(str, "typedef") ||
           S_EQ(str, "const") ||
           S_EQ(str, "extern") ||
           S_EQ(str, "restrict");
}

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

static struct token* make_identifier_or_keyword()
{
    struct buffer* buffer = buffer_create();
    char c;
    LEX_GETC_IF(buffer, c, (c>= 'a' && c<='z') || (c>= 'A' && c<='Z') || (c>= '0' && c<='9') || c == '_')

    buffer_write(buffer, 0x00);

    if (is_keyword(buffer_ptr(buffer))) {
        return token_create(&(struct token){.type=TOKEN_TYPE_KEYWORD,.sval=buffer_ptr(buffer)});
    }

    return token_create(&(struct token){.type=TOKEN_TYPE_IDENTIFIER,.sval=buffer_ptr(buffer)});
}

struct token* read_special_token()
{
    char c = peekc();
    if (isalpha(c) || c == 'c')
    {
        return make_identifier_or_keyword();
    }
    
    return NULL;
}

const char* read_number_str() {
    const char* num = NULL;
    struct buffer* buffer = buffer_create();
    char c = peekc();
    LEX_GETC_IF(buffer, c, (c >= '0' && c <= '9'));

    // Finalizar a string
    buffer_write(buffer, 0x00);

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
    for (int i = 0; i < sizeof(symbol_array); i++)
    {
        LEX_GETC_IF(buffer, c, c == symbol_array[i]);
    }
    // Finalizar a string
    buffer_write(buffer, 0x00);

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

struct token* token_make_one_line_comment(){
    struct buffer* buffer = buffer_create();
    char c = 0;
    LEX_GETC_IF(buffer, c, c != '\n' && c != EOF);

    return token_create(&(struct token){.type=TOKEN_TYPE_COMMENT,.sval=buffer_ptr(buffer)});
}

struct token* token_make_multiline_comment(){
    struct buffer* buffer = buffer_create();
    char c = 0;
    while(1) {
        LEX_GETC_IF(buffer, c, c != '*' && c != EOF);
        
        if (c == EOF) {
            compiler_error(lex_process->compiler, "0 comentario não foi fechado!\n");
            buffer_free(buffer);
            return NULL;
        } else if (c == '*') {
            nextc(); //Pula para o proximo caracter do arquivo.

            
            if (peekc() == '/') {
                buffer_pop(buffer); // Remove o '*' do buffer.
                nextc(); // Pula o '/'.
                nextc();
                break;
            }
            buffer_write(buffer, '*'); // Adiciona o '*' no buffer.
        }
    }
    
    return token_create(&(struct token){.type=TOKEN_TYPE_COMMENT,.sval=buffer_ptr(buffer)});
}
struct token* handle_comment(){
    char c = peekc();
    if (c == '/') {
        nextc(); // Pula o '/'.
        if (peekc() == '/') {
            nextc(); // Pula o '/'.
            return token_make_one_line_comment();
        } else if (peekc() == '*') {
            nextc(); // Pula o '*'.
            return token_make_multiline_comment();
        }

        pushc('/'); // Se não for um comentario, adiciona o '/' de volta.
        return token_make_operator_or_string();
    
    }

    return NULL;
}


int token_is_keyword(struct token* token, const char* keyword) {
    return strcmp(token -> sval, keyword) == 0;
}

static void lex_new_expression() {
    lex_process->current_expression_count++;
    if (lex_process->current_expression_count == 1) {
        lex_process->parentheses_buffer = buffer_create();
    }
}

int is_operator_char(char c) {
    switch (c) {
        OPERATOR_CASE:
            return 1;
        default:
            return 0;
    }
}

char* read_op() {
    char op[2] = {0}; // Temporary buffer
    int len = 0;

    char current = nextc();
    op[len++] = current;

    while (len < 2 - 1) {
        char next = peekc();

        if (is_operator_char(next)) {
            op[len++] = nextc();
        } else {
            break;
        }
    }

    op[len] = '\0';

    // Copy to heap and return (caller should free)
    char* result = malloc(len + 1);
    strcpy(result, op);
    return result;
}

static struct token *token_make_operator_or_string(){
    char op = peekc();
    // tratar o caso do #include <abc.h>
    if (op == '<') {
        struct token* last_token = lexer_last_token();
        if (token_is_keyword(last_token, "include")) {
            return token_make_string('<', '>');
        }
    }

    struct token* token = token_create(&(struct token){.type = TOKEN_TYPE_STRING,.sval=read_op()});
    if (op == '(') {
        lex_new_expression();
    }

    return token;
}

void print_token_list(struct lex_process *process){
    struct token *token = NULL;     

    for (int i = 0; i < process->token_vec->count; i++)
    {
        token = vector_at(process-> token_vec,i);

        switch (token->type) {
            case TOKEN_TYPE_IDENTIFIER:
                printf("IDENT: %s\n", token->sval);
                break;
            case TOKEN_TYPE_KEYWORD:
                printf("KEYWD: %s\n", token->sval);
                break;
            case TOKEN_TYPE_STRING:
                printf("STRNG: \"%s\"\n", token->sval);
                break;
            case TOKEN_TYPE_NUMBER:
                printf("NUMBR: %lld\n", token->llnum);
                break;
            case TOKEN_TYPE_SYMBOL:
                printf("SYMBL: %c\n", token->cval);
                break;
            case TOKEN_TYPE_NEWLINE:
                printf("NEWLN\n");
                break;
            case TOKEN_TYPE_OPERATOR:
                printf("OPER: %s\n", token->sval);
                break;
            case TOKEN_TYPE_COMMENT:
                printf("CMNT:\n");
                break;
            default:
                break;
        }
    }
    
}

//Funcao responsavel por ler o proximo token do arquivo.
struct token* read_next_token(){
    struct token* token = NULL;
    char c = peekc();
    //printf("Character: %c (ASCII: %d)\n", c, c);
    switch (c)
    {
    case EOF:
        // Fim do arquivo. 
        break;

    NUMERIC_CASE:
        token = token_make_number();
        break;

    SYMBOL_CASE:
        token = token_make_symbol();
        break;
    OPERATOR_CASE:
        token = token_make_operator_or_string();
        break;

    case '"':
        token = token_make_string();
        break;
    case '\t':
        token = handle_whitespace();
        break;
    case ' ':
    case '\n':
        token = token_create(&(struct token){.type=TOKEN_TYPE_NEWLINE});
        nextc();
        break;
    default: 
        token =read_special_token();
        if (!token)
        {
            compiler_error(lex_process->compiler, "Token invalido!\n");
        }
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
    print_token_list(process);
    return LEXICAL_ANALYSIS_ALL_OK;
}