#include "compiler.h"
#include "helpers/vector.h"
#include <stdio.h>  // Required for fprintf()
#include <string.h>
#include <assert.h> // LAB4

static struct compile_process* current_process;
static struct token* parser_last_token;

// Estrutura para passar comandos atraves de funcoes recursivas.
struct history {
    int flags;
};

void parse_expressionable(struct history* history);
int parse_expressionable_single(struct history* history);

static bool parser_left_op_has_priority(const char* op_left, const char* op_right); // LAB4
extern struct expressionable_op_precedence_group op_precedence[TOTAL_OPERADOR_GROUPS]; // LAB4

struct history* history_begin(int flags) {
    struct history* history = calloc(1, sizeof(struct history));
    history->flags = flags;
    return history;
}

struct history* history_down(struct history* history, int flags) {
    struct history* new_history = calloc(1, sizeof(struct history));
    memcpy(new_history, history, sizeof(struct history));
    new_history->flags = flags;
    return new_history;
}

static void parser_ignore_nl_or_comment(struct token* token) {
    while (token && discart_token(token) ) {
        // Pula o token que deve ser descartado no vetor.
        vector_peek(current_process->token_vec);
        // Pega o proximo token para ver se ele tambem sera descartado.
        token = vector_peek_no_increment(current_process->token_vec);
    }
}

static struct token* token_next() {
    struct token* next_token = vector_peek_no_increment(current_process->token_vec);
    parser_ignore_nl_or_comment(next_token);
    current_process->pos = next_token->pos; // Atualiza a posicao do arquivo de compilacao.
    parser_last_token = next_token;
    return vector_peek(current_process->token_vec);
}

static struct token* token_peek_next() {
    struct token* next_token = vector_peek_no_increment(current_process->token_vec);
    parser_ignore_nl_or_comment(next_token);
    return vector_peek_no_increment(current_process->token_vec);
}

void parse_single_token_to_node() {
    struct token* token = token_next();
    struct node* node = NULL;
    switch (token->type) {
        case TOKEN_TYPE_NUMBER:
            node = node_create(&(struct node){.type=NODE_TYPE_NUMBER, .llnum=token->llnum});
        break;
        case TOKEN_TYPE_IDENTIFIER:
            node = node_create(&(struct node){.type=NODE_TYPE_IDENTIFIER, .sval=token->sval});
        break;
        case TOKEN_TYPE_STRING:
            node = node_create(&(struct node){.type=NODE_TYPE_STRING, .sval=token->sval});
        break;
        default: compiler_error(current_process, "Esse token nao pode ser convertido para node!\n");
        break;
    }
}

void parse_expressionable_for_op(struct history* history, const char* op) {
    parse_expressionable(history);
}

void parser_node_shift_children_left(struct node* node) {
    assert(node->type == NODE_TYPE_EXPRESSION);
    assert(node->exp.right == NODE_TYPE_EXPRESSION);
    const char* right_op = node->exp.right->exp.op;
    struct node* new_exp_left_node = node->exp.left;
    struct node* new_exp_right_node = node->exp.right->exp.left;
    make_exp_node(new_exp_left_node, new_exp_right_node, node->exp.op);
    // EX: 50*E(20+50) -> E(50*20)+50
    struct node* new_left_operand = node_pop();
    struct node* new_right_operand = node->exp.right->exp.right;
    node->exp.left = new_left_operand;
    node->exp.right = new_right_operand;
    node->exp.op = right_op;
}

void parser_reorder_expression(struct node** node_out) {
    struct node* node = *node_out;

    // Only reorder expression nodes
    if (!node || node->type != NODE_TYPE_EXPRESSION)
        return;

    // Recurse into sub-expressions first
    if (node->exp.left && node->exp.left->type == NODE_TYPE_EXPRESSION)
        parser_reorder_expression(&node->exp.left);

    if (node->exp.right && node->exp.right->type == NODE_TYPE_EXPRESSION)
        parser_reorder_expression(&node->exp.right);

    // Now check if the right expression has higher precedence
    if (node->exp.right && node->exp.right->type == NODE_TYPE_EXPRESSION) {
        const char* current_op = node->exp.op;
        const char* right_op = node->exp.right->exp.op;

        if (parser_left_op_has_priority(current_op, right_op)) {
        
            struct node* A = node->exp.left;
            struct node* B = node->exp.right->exp.left;
            struct node* C = node->exp.right->exp.right;
        
            const char* new_op = node->exp.right->exp.op;
        
            // Build new left subtree: (A op B)
            make_exp_node(A, B, current_op);
            struct node* new_left = node_pop();
        
            // Update current node to new structure
            node->exp.left = new_left;
            node->exp.right = C;
            node->exp.op = new_op;
        
            // Recurse again, in case more fixes are needed
            if (parser_left_op_has_priority(current_op, right_op)) {
                struct node* A = node->exp.left;
                struct node* B = node->exp.right->exp.left;
                struct node* C = node->exp.right->exp.right;
            
                const char* new_op = node->exp.right->exp.op;
            
                // Build new left subtree: (A op B)
                make_exp_node(A, B, current_op);
                struct node* new_left = node_pop();
            
                node->exp.left = new_left;
                node->exp.right = C;
                node->exp.op = new_op;
            
                // Only reorder if necessary
                if (new_left != node->exp.left) {
                    parser_reorder_expression(&node->exp.left);
                }
                if (C != node->exp.right) {
                    parser_reorder_expression(&node->exp.right);
                }
            }            
        }        
    }
}

void parse_exp_normal(struct history* history) {
    struct token* op_token = token_peek_next();
    if (!op_token) return;
    const char *op = op_token->sval;
    struct node* node_left = node_peek_expressionable_or_null();
    if (!node_left) return;
    // Retira da lista de tokens o token de operador. Ex: "123+456", retira o token "+".
    token_next();
    // Retira da lista de nodes, o ultimo node inserido.
    node_pop();
    node_left->flags |= NODE_FLAG_INSIDE_EXPRESSION;
    // Nesse momento, temos o node da esquerda e o operador. Essa funcao ira criar o node da direita.
    parse_expressionable_for_op(history_down(history, history->flags), op);
    struct node* node_right = node_pop();
    node_right->flags |= NODE_FLAG_INSIDE_EXPRESSION;
    // Cria o node de expressao, passando o node da esquerda, node da direita e operador.
    make_exp_node(node_left, node_right, op);
    struct node* exp_node = node_pop();

    parser_reorder_expression(&exp_node);
    node_push(exp_node);
}

int parse_exp(struct history* history) {
    parse_exp_normal(history);
    return 0;
}

int parse_expressionable_single(struct history* history) {
    struct token* token = token_peek_next();
    if (!token) return -1;
    history->flags |= NODE_FLAG_INSIDE_EXPRESSION;
    int res = -1;
    switch (token->type){
        case TOKEN_TYPE_NUMBER:
            parse_single_token_to_node();
            res = 0;
            break;
        case TOKEN_TYPE_OPERATOR:
            parse_exp(history);
            res = 0;
            break;
        default: break;
        }
        return res;
}

void parse_expressionable(struct history* history){
    while (parse_expressionable_single(history) == 0) {}
}

int parse_next(){
    struct token* token = token_peek_next();
    if (!token) return -1;
    int res = 0;
    switch (token->type) {
        case TOKEN_TYPE_NUMBER:
        case TOKEN_TYPE_IDENTIFIER:
        case TOKEN_TYPE_STRING:
            parse_expressionable(history_begin(0));
            break;  
        default:
            break;
    }
    return 0;
}

void print_node_type(struct node* node) {
    if (!node) {
        printf("NULL");
        return;
    }

    switch (node->type) {
        case NODE_TYPE_NUMBER:
            printf("NUMBER (%u)", node->inum);
            break;
        case NODE_TYPE_IDENTIFIER:
            printf("IDENTIFIER (%s)", node->sval ? node->sval : "null");
            break;
        case NODE_TYPE_STRING:
            printf("STRING (\"%s\")", node->sval ? node->sval : "null");
            break;
        case NODE_TYPE_EXPRESSION:
            printf("EXPRESSION (op: %s)", node->exp.op ? node->exp.op : "null");
            break;
        case NODE_TYPE_UNARY:
            printf("UNARY (op: %s)", node->exp.op ? node->exp.op : "null");
            break;
        case NODE_TYPE_EXPRESSION_PARENTHESES:
            printf("PAREN_EXPR");
            break;
        default:
            printf("UNKNOWN (type %d)", node->type);
            break;
    }
}

void print_node_tree(struct node* node, int indent, bool is_last) {
    if (!node) {
        for (int i = 0; i < indent; ++i) {
            printf("│   ");
        }
        printf("%sNULL\n", is_last ? "└── " : "├── ");
        return;
    }

    for (int i = 0; i < indent; ++i) {
        printf("│   ");
    }

    printf("%s", is_last ? "└── " : "├── ");
    print_node_type(node);
    printf("\n");

    if (node->type == NODE_TYPE_EXPRESSION ||
        node->type == NODE_TYPE_UNARY ||
        node->type == NODE_TYPE_EXPRESSION_PARENTHESES) {
        print_node_tree(node->exp.left, indent + 1, false);
        print_node_tree(node->exp.right, indent + 1, true);
    }
}


int parse(struct compile_process* process) {
    if (!process || !process->node_tree_vec || !process->token_vec || !process->node_vec) {
        return PARSE_GENERAL_ERROR;
    }

    current_process = process;
    parser_last_token = NULL;

    node_set_vector(process->node_vec, process->node_tree_vec);
    vector_set_peek_pointer(process->token_vec, 0);

    struct node* node = node_peek();

    printf("\n Arvore Nodes \n");

    while (parse_next() == 0) {
        node = node_peek();

        // if (!node) {
        //     printf("Warning: node_peek() returned NULL\n");
        //     break;
        // }

        vector_push(process->node_tree_vec, node);

        print_node_tree(node, 0,true);
        printf("\n\n");
    }

    return PARSE_ALL_OK;
}

static int parser_get_precedence_for_operator(const char* op, struct expressionable_op_precedence_group** group_out) {
    *group_out = NULL;
    for (int i = 0; i < TOTAL_OPERADOR_GROUPS; ++i) {
        for (int j = 0; op_precedence[i].operators[j]; j++) {
            const char* _op = op_precedence[i].operators[j];
            if (S_EQ(op, _op)) {
                *group_out = &op_precedence[i];
                return i;
            }
        }
    }
    return -1;
}


static bool parser_left_op_has_priority(const char* op_left, const char* op_right) {
    struct expressionable_op_precedence_group* group_left = NULL;
    struct expressionable_op_precedence_group* group_right = NULL;

    // Get precedence for left operator
    int precedence_left = parser_get_precedence_for_operator(op_left, &group_left);
    // Get precedence for right operator
    int precedence_right = parser_get_precedence_for_operator(op_right, &group_right);

    // Compare precedence
    if (precedence_left != precedence_right) {
        return precedence_left < precedence_right;
    }

    // For operators with the same precedence, check associativity
    if (group_left && group_left->associativity == ASSOCIATIVITY_LEFT_TO_RIGTH) {
        return false;
    }

    // If right-to-left associativity, left operator has priority
    if (group_left && group_left->associativity == ASSOCIATIVITY_RIGHT_TO_LEFT) {
        return true;
    }

    return false;
}
