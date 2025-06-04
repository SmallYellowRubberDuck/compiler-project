#include "visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Вспомогательная функция для отображения отступов
static void print_tree_indentation(int indentation_level, FILE *output_file) {
    for (int i = 0; i < indentation_level; i++) {
        fprintf(output_file, "  ");
    }
}

// Рекурсивная функция для визуализации AST с отступами
static void visualize_ast_node_recursive(ASTNode *ast_node, int indentation_level, FILE *output_file) {
    if (!ast_node) {
        print_tree_indentation(indentation_level, output_file);
        fprintf(output_file, "NULL\n");
        return;
    }

    switch (ast_node->type) {
        case NODE_PROGRAM:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "PROGRAM\n");
            for (size_t i = 0; i < ast_node->block.children.size; i++) {
                visualize_ast_node_recursive(ast_node->block.children.items[i], indentation_level + 1, output_file);
            }
            break;

        case NODE_VARIABLE_DECLARATION:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "VAR_DECL: %s (type: %s, global: %s)\n",
                   ast_node->variable.name,
                   ast_node->variable.var_type,
                   ast_node->variable.is_global ? "yes" : "no");
            if (ast_node->variable.initializer) {
                print_tree_indentation(indentation_level + 1, output_file);
                fprintf(output_file, "INIT:\n");
                visualize_ast_node_recursive(ast_node->variable.initializer, indentation_level + 2, output_file);
            }
            break;

        case NODE_BINARY_OPERATION:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "BIN_OP: %s\n", ast_node->binary_op.op_type);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "LEFT:\n");
            visualize_ast_node_recursive(ast_node->binary_op.left, indentation_level + 2, output_file);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "RIGHT:\n");
            visualize_ast_node_recursive(ast_node->binary_op.right, indentation_level + 2, output_file);
            break;

        case NODE_LITERAL:
            print_tree_indentation(indentation_level, output_file);
            if (strcmp(ast_node->literal.type, "int") == 0) {
                fprintf(output_file, "LITERAL: %d (type: int)\n", ast_node->literal.int_value); 
            } else if (strcmp(ast_node->literal.type, "string") == 0) {
                fprintf(output_file, "LITERAL: \"%s\" (type: string)\n", ast_node->literal.string_value);
            } else {
                fprintf(output_file, "LITERAL: (unknown type)\n");
            }
            break;

        case NODE_IDENTIFIER:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "ID: %s\n", ast_node->identifier.name);
            break;

        case NODE_ASSIGNMENT:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "ASSIGN: %s\n", ast_node->assignment.target);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "VALUE:\n");
            visualize_ast_node_recursive(ast_node->assignment.value, indentation_level + 2, output_file);
            break;

        case NODE_IF_STATEMENT:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "IF\n");
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "CONDITION:\n");
            visualize_ast_node_recursive(ast_node->if_stmt.condition, indentation_level + 2, output_file);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "THEN:\n");
            visualize_ast_node_recursive(ast_node->if_stmt.then_branch, indentation_level + 2, output_file);
            if (ast_node->if_stmt.else_branch) {
                print_tree_indentation(indentation_level + 1, output_file);
                fprintf(output_file, "ELSE:\n");
                visualize_ast_node_recursive(ast_node->if_stmt.else_branch, indentation_level + 2, output_file);
            }
            break;

        case NODE_WHILE_LOOP:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "WHILE\n");
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "CONDITION:\n");
            visualize_ast_node_recursive(ast_node->while_loop.condition, indentation_level + 2, output_file);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "BODY:\n");
            visualize_ast_node_recursive(ast_node->while_loop.body, indentation_level + 2, output_file);
            break;

        case NODE_ROUND_LOOP:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "ROUND: %s\n", ast_node->round_loop.variable);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "START:\n");
            visualize_ast_node_recursive(ast_node->round_loop.start, indentation_level + 2, output_file);
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "END:\n");
            visualize_ast_node_recursive(ast_node->round_loop.end, indentation_level + 2, output_file);
            if (ast_node->round_loop.step) {
                print_tree_indentation(indentation_level + 1, output_file);
                fprintf(output_file, "STEP:\n");
                visualize_ast_node_recursive(ast_node->round_loop.step, indentation_level + 2, output_file);
            }
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "BODY:\n");
            visualize_ast_node_recursive(ast_node->round_loop.body, indentation_level + 2, output_file);
            break;

        case NODE_BLOCK:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "BLOCK\n");
            for (size_t i = 0; i < ast_node->block.children.size; i++) {
                visualize_ast_node_recursive(ast_node->block.children.items[i], indentation_level + 1, output_file);
            }
            break;

        case NODE_PRINT:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "PRINT\n");
            print_tree_indentation(indentation_level + 1, output_file);
            fprintf(output_file, "EXPRESSION:\n");
            visualize_ast_node_recursive(ast_node->print.expression, indentation_level + 2, output_file);
            break;

        default:
            print_tree_indentation(indentation_level, output_file);
            fprintf(output_file, "UNKNOWN NODE TYPE: %d\n", ast_node->type);
            break;
    }
}

void visualize_ast(ASTNode *ast_root, FILE *output_file) {
    visualize_ast_node_recursive(ast_root, 0, output_file);
}

int save_ast_to_file(ASTNode *ast_root, const char *output_filename) {
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        return -1;
    }

    visualize_ast(ast_root, output_file);
    fclose(output_file);
    return 0;
} 