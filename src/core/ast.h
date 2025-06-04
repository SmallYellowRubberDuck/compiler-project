#ifndef AST_H
#define AST_H

#include <stddef.h>

// Node types for Abstract Syntax Tree
typedef enum {
    NODE_PROGRAM,
    NODE_VARIABLE_DECLARATION,
    NODE_BINARY_OPERATION,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_ASSIGNMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_LOOP,
    NODE_ROUND_LOOP,
    NODE_BLOCK,
    NODE_PRINT
} NodeType;

typedef struct ASTNode ASTNode;

typedef struct {
    ASTNode **items;
    size_t size;
    size_t capacity;
} NodeList;

struct ASTNode {
    NodeType type;
    union {
        struct {
            char *name;
            char *var_type;
            int is_global;
            ASTNode *initializer;
        } variable;

        struct {
            char *op_type;
            ASTNode *left;
            ASTNode *right;
        } binary_op;

        struct {
            union {
                int int_value;
                float float_value;
                char *string_value;
            };
            char *type;
        } literal;

        struct {
            char *name;
        } identifier;

        struct {
            char *target;
            ASTNode *value;
        } assignment;

        struct {
            ASTNode *condition;
            ASTNode *then_branch;
            ASTNode *else_branch;
        } if_stmt;

        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_loop;

        struct {
            char *variable;
            ASTNode *start;
            ASTNode *end;
            ASTNode *step;
            ASTNode *body;
        } round_loop;

        struct {
            NodeList children;
        } block;

        struct {
            ASTNode *expression;
        } print;
    };
};

// AST node creation functions
ASTNode *create_program_node();
ASTNode *create_variable_declaration(const char *var_name, const char *var_type, int is_global);
ASTNode *create_variable_declaration_with_initializer(const char *var_name, const char *var_type, int is_global, ASTNode *initializer);
ASTNode *create_binary_operation(const char *operator, ASTNode *left_operand, ASTNode *right_operand);
ASTNode *create_literal_int(int value);
ASTNode *create_literal_float(float value);
ASTNode *create_literal_string(const char *value);
ASTNode *create_identifier(const char *identifier_name);
ASTNode *create_assignment(const char *target_var, ASTNode *value_expr);
ASTNode *create_if_statement(ASTNode *condition_expr, ASTNode *then_block, ASTNode *else_block);
ASTNode *create_while_loop(ASTNode *condition_expr, ASTNode *loop_body);
ASTNode *create_round_loop(const char *iterator_var, ASTNode *start_expr, ASTNode *end_expr, ASTNode *step_expr, ASTNode *loop_body);
ASTNode *create_block();
ASTNode *create_print_statement(ASTNode *print_expr);

// AST manipulation functions
void add_node_to_block(ASTNode *parent_block, ASTNode *child_node);
void free_node(ASTNode *node);

// Global AST root
extern ASTNode *ast_program_root;

#endif /* AST_H */ 