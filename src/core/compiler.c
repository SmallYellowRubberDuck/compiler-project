#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "../codegen/codegen.h"
#include "../utils/visualizer.h"
#include "error.h"

extern int parser_init(const char *source_filename);
extern ASTNode *get_ast_root();

void print_usage_help(const char *program_name) {
    fprintf(stderr, "Usage: %s <source_file> [options]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <output_file>    Save RISC code to file\n");
    fprintf(stderr, "  -ast                Show AST\n");
    fprintf(stderr, "  -ast-file <ast_file>  Save AST to file\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage_help(argv[0]);
        return 1;
    }

    const char *source_filename = argv[1];
    const char *risc_output_file = NULL;
    const char *ast_output_file = NULL;
    int should_display_ast = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            risc_output_file = argv[++i];
        } else if (strcmp(argv[i], "-ast") == 0) {
            should_display_ast = 1;
        } else if (strcmp(argv[i], "-ast-file") == 0 && i + 1 < argc) {
            ast_output_file = argv[++i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage_help(argv[0]);
            return 1;
        }
    }

    error_system_init();

    if (parser_init(source_filename) != 0) {
        fprintf(stderr, "Error parsing file %s\n", source_filename);
        error_system_cleanup();
        return 1;
    }

    ASTNode *ast_program_root = get_ast_root();
    if (!ast_program_root) {
        fprintf(stderr, "Failed to build AST for file %s\n", source_filename);
        error_system_cleanup();
        return 1;
    }

    if (should_display_ast) {
        printf("#AST:\n");
        visualize_ast(ast_program_root, stdout);
        printf("\n");
    }

    if (ast_output_file) {
        if (save_ast_to_file(ast_program_root, ast_output_file) != 0) {
            fprintf(stderr, "Error saving AST to file %s\n", ast_output_file);
        } else {
            printf("AST saved to file %s\n", ast_output_file);
        }
    }

    if (error_has_any_errors()) {
        fprintf(stderr, "\nCompilation aborted due to errors.\n");
        error_print_all_errors(stderr);
        error_system_cleanup();
        return 1;
    }

    set_risc_generator_filename(source_filename);

    char *generated_risc_code = generate_risc_code(ast_program_root);
    if (!generated_risc_code) {
        fprintf(stderr, "Error generating RISC code\n");
        error_system_cleanup();
        return 1;
    }

    printf("#RISC-code:\n%s\n", generated_risc_code);

    if (risc_output_file) {
        FILE *output_file = fopen(risc_output_file, "w");
        if (output_file) {
            fprintf(output_file, "%s", generated_risc_code);
            fclose(output_file);
            printf("RISC code saved to file %s\n", risc_output_file);
        } else {
            fprintf(stderr, "Failed to open file %s for writing\n", risc_output_file);
        }
    }

    free_risc_code(generated_risc_code);
    error_system_cleanup();

    return 0;
} 