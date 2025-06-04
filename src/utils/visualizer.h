#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "../core/ast.h"
#include <stdio.h>

/**
 * Визуализирует AST в текстовом виде в указанный файл
 * @param ast_root Корень AST
 * @param output_file Файл для вывода (например, stdout)
 */
void visualize_ast(ASTNode *ast_root, FILE *output_file);

/**
 * Визуализирует AST в текстовом виде и сохраняет в файл
 * @param ast_root Корень AST
 * @param output_filename Имя файла для вывода
 * @return 0 при успехе, -1 при ошибке
 */
int save_ast_to_file(ASTNode *ast_root, const char *output_filename);

#endif /* VISUALIZER_H */ 