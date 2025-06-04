#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *ast_program_root = NULL;

void init_ast_node_list(NodeList *node_list) {
    node_list->items = NULL;
    node_list->size = 0;
    node_list->capacity = 0;
}

void add_ast_node_to_list(NodeList *node_list, ASTNode *node) {
    if (node_list->size >= node_list->capacity) {
        size_t new_capacity = node_list->capacity == 0 ? 4 : node_list->capacity * 2;
        ASTNode **new_items = (ASTNode **) realloc(node_list->items, new_capacity * sizeof(ASTNode *));
        if (new_items) {
            node_list->items = new_items;
            node_list->capacity = new_capacity;
        } else {
            return;
        }
    }
    node_list->items[node_list->size++] = node;
}

char *allocate_string_copy(const char *source) {
    if (!source) return NULL;
    size_t len = strlen(source);
    char *new_str = (char *) malloc(len + 1);
    if (new_str) {
        strcpy(new_str, source);
    }
    return new_str;
}

ASTNode *create_program_node() {
    ASTNode *program_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (program_node) {
        program_node->type = NODE_PROGRAM;
        init_ast_node_list(&program_node->block.children);
    }
    return program_node;
}

ASTNode *create_variable_declaration(const char *var_name, const char *var_type, int is_global) {
    ASTNode *var_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (var_node) {
        var_node->type = NODE_VARIABLE_DECLARATION;
        var_node->variable.name = allocate_string_copy(var_name);
        var_node->variable.var_type = allocate_string_copy(var_type);
        var_node->variable.is_global = is_global;
        var_node->variable.initializer = NULL;
    }
    return var_node;
}

ASTNode *create_variable_declaration_with_initializer(const char *var_name, const char *var_type, int is_global, ASTNode *initializer) {
    ASTNode *var_node = create_variable_declaration(var_name, var_type, is_global);
    if (var_node) {
        var_node->variable.initializer = initializer;
    }
    return var_node;
}

ASTNode *create_binary_operation(const char *operator, ASTNode *left_operand, ASTNode *right_operand) {
    ASTNode *op_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (op_node) {
        op_node->type = NODE_BINARY_OPERATION;
        op_node->binary_op.op_type = allocate_string_copy(operator);
        op_node->binary_op.left = left_operand;
        op_node->binary_op.right = right_operand;
    }
    return op_node;
}

ASTNode *create_literal_int(int value) {
    ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
    if (node) {
        node->type = NODE_LITERAL;
        node->literal.int_value = value;
        node->literal.type = allocate_string_copy("int");
    }
    return node;
}

ASTNode *create_literal_float(float value) {
    ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
    if (node) {
        node->type = NODE_LITERAL;
        node->literal.float_value = value;
        node->literal.type = allocate_string_copy("float");
    }
    return node;
}

ASTNode *create_literal_string(const char *value) {
    ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
    if (node) {
        node->type = NODE_LITERAL;
        node->literal.string_value = allocate_string_copy(value);
        node->literal.type = allocate_string_copy("string");
    }
    return node;
}

ASTNode *create_identifier(const char *identifier_name) {
    ASTNode *id_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (id_node) {
        id_node->type = NODE_IDENTIFIER;
        id_node->identifier.name = allocate_string_copy(identifier_name);
    }
    return id_node;
}

ASTNode *create_assignment(const char *target_var, ASTNode *value_expr) {
    ASTNode *assign_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (assign_node) {
        assign_node->type = NODE_ASSIGNMENT;
        assign_node->assignment.target = allocate_string_copy(target_var);
        assign_node->assignment.value = value_expr;
    }
    return assign_node;
}

ASTNode *create_if_statement(ASTNode *condition_expr, ASTNode *then_block, ASTNode *else_block) {
    ASTNode *if_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (if_node) {
        if_node->type = NODE_IF_STATEMENT;
        if_node->if_stmt.condition = condition_expr;
        if_node->if_stmt.then_branch = then_block;
        if_node->if_stmt.else_branch = else_block;
    }
    return if_node;
}

ASTNode *create_while_loop(ASTNode *condition_expr, ASTNode *loop_body) {
    ASTNode *while_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (while_node) {
        while_node->type = NODE_WHILE_LOOP;
        while_node->while_loop.condition = condition_expr;
        while_node->while_loop.body = loop_body;
    }
    return while_node;
}

ASTNode *create_round_loop(const char *iterator_var, ASTNode *start_expr, ASTNode *end_expr, ASTNode *step_expr, ASTNode *loop_body) {
    ASTNode *round_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (round_node) {
        round_node->type = NODE_ROUND_LOOP;
        round_node->round_loop.variable = allocate_string_copy(iterator_var);
        round_node->round_loop.start = start_expr;
        round_node->round_loop.end = end_expr;
        round_node->round_loop.step = step_expr;
        round_node->round_loop.body = loop_body;
    }
    return round_node;
}

ASTNode *create_block() {
    ASTNode *block_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (block_node) {
        block_node->type = NODE_BLOCK;
        init_ast_node_list(&block_node->block.children);
    }
    return block_node;
}

ASTNode *create_print_statement(ASTNode *print_expr) {
    ASTNode *print_node = (ASTNode *) malloc(sizeof(ASTNode));
    if (print_node) {
        print_node->type = NODE_PRINT;
        print_node->print.expression = print_expr;
    }
    return print_node;
}

void add_node_to_block(ASTNode *parent_block, ASTNode *child_node) {
    if (parent_block && child_node && (parent_block->type == NODE_BLOCK || parent_block->type == NODE_PROGRAM)) {
        add_ast_node_to_list(&parent_block->block.children, child_node);
    }
}

void free_node(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
            for (size_t i = 0; i < node->block.children.size; i++) {
                free_node(node->block.children.items[i]);
            }
            free(node->block.children.items);
            break;

        case NODE_VARIABLE_DECLARATION:
            free(node->variable.name);
            free(node->variable.var_type);
            if (node->variable.initializer) {
                free_node(node->variable.initializer);
            }
            break;

        case NODE_BINARY_OPERATION:
            free(node->binary_op.op_type);
            free_node(node->binary_op.left);
            free_node(node->binary_op.right);
            break;

        case NODE_LITERAL:
            if (strcmp(node->literal.type, "string") == 0) {
                free(node->literal.string_value);
            }
            free(node->literal.type);
            break;

        case NODE_IDENTIFIER:
            free(node->identifier.name);
            break;

        case NODE_ASSIGNMENT:
            free(node->assignment.target);
            free_node(node->assignment.value);
            break;

        case NODE_IF_STATEMENT:
            free_node(node->if_stmt.condition);
            free_node(node->if_stmt.then_branch);
            if (node->if_stmt.else_branch) {
                free_node(node->if_stmt.else_branch);
            }
            break;

        case NODE_WHILE_LOOP:
            free_node(node->while_loop.condition);
            free_node(node->while_loop.body);
            break;

        case NODE_ROUND_LOOP:
            free(node->round_loop.variable);
            free_node(node->round_loop.start);
            free_node(node->round_loop.end);
            free_node(node->round_loop.step);
            free_node(node->round_loop.body);
            break;

        case NODE_PRINT:
            free_node(node->print.expression);
            break;
    }

    free(node);
} 