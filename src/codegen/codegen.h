#ifndef CODEGEN_H
#define CODEGEN_H

#include "../core/ast.h"

char *generate_risc_code(ASTNode *ast_program_root);

void free_risc_code(char *code);

void set_risc_generator_filename(const char *filename);

#endif /* CODEGEN_H */ 