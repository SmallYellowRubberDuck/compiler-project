#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern int get_current_line(void);
extern int get_current_column(void);
extern const char* get_parser_filename(void);

#define MAX_ERRORS 3

static CompilerError error_list[MAX_ERRORS];
static int total_error_count = 0;
static int is_initialized = 0;
static int has_critical_error = 0;

static int current_context_is_global = 1;

typedef struct {
    char *symbol_name;
    char *symbol_type;
    int is_global_scope;
    int is_defined;
} SymbolTableEntry;

#define MAX_SYMBOLS 1000
static SymbolTableEntry symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

void error_system_init(void) {
    if (!is_initialized) {
        total_error_count = 0;
        symbol_count = 0;
        memset(error_list, 0, sizeof(error_list));
        memset(symbol_table, 0, sizeof(symbol_table));
        is_initialized = 1;
    }
}

void error_system_cleanup(void) {
    if (is_initialized) {
        for (int i = 0; i < symbol_count; i++) {
            free(symbol_table[i].symbol_name);
            free(symbol_table[i].symbol_type);
        }
        total_error_count = 0;
        symbol_count = 0;
        is_initialized = 0;
    }
}

void error_report_issue(CompilerErrorType error_type, int line_number, int column_number, 
                       const char *source_filename, const char *message_format, ...) {
    va_list args;
    va_start(args, message_format);

    if (line_number == 0 && column_number == 0) {
        line_number = get_current_line();
        column_number = get_current_column();
    }
    
    if (source_filename == NULL || strcmp(source_filename, "unknown") == 0) {
        source_filename = get_parser_filename();
    }

    switch (error_type) {
        case ERROR_UNDEFINED_VARIABLE:
            fprintf(stderr, "Error: Undefined variable in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
        case ERROR_TYPE_MISMATCH:
            fprintf(stderr, "Error: Type mismatch in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
        case ERROR_DIVISION_BY_ZERO:
            fprintf(stderr, "Error: Division by zero in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
        case ERROR_SCOPE:
            fprintf(stderr, "Error: Scope violation in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
        case ERROR_REDECLARATION:
            fprintf(stderr, "Error: Variable redeclaration in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
        default:
            fprintf(stderr, "Error: Unknown error in %s:%d:%d: ", source_filename, line_number, column_number);
            break;
    }

    vfprintf(stderr, message_format, args);
    fprintf(stderr, "\n");
    va_end(args);

    if (error_type == ERROR_UNDEFINED_VARIABLE || 
        error_type == ERROR_TYPE_MISMATCH || 
        error_type == ERROR_DIVISION_BY_ZERO ||
        error_type == ERROR_SCOPE ||
        error_type == ERROR_REDECLARATION) {
        error_set_critical_error();
    }

    total_error_count++;
}

int error_has_any_errors(void) {
    return total_error_count > 0;
}

int error_get_total_count(void) {
    return total_error_count;
}

void error_clear_all(void) {
    total_error_count = 0;
}

void error_print_all_errors(FILE *output_stream) {
    if (total_error_count == 0) {
        fprintf(stderr, "No errors detected.\n");
        return;
    }

    fprintf(stderr, "%d compilation errors detected.\n", total_error_count);
}

static int add_symbol_to_table(const char *symbol_name, const char *symbol_type, 
                             int is_global_scope, int is_defined) {
    if (symbol_count >= MAX_SYMBOLS) {
        return -1;
    }

    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].symbol_name, symbol_name) == 0) {
            symbol_table[i].is_defined = is_defined;
            return i;
        }
    }

    SymbolTableEntry *entry = &symbol_table[symbol_count];
    entry->symbol_name = strdup(symbol_name);
    entry->symbol_type = strdup(symbol_type);
    entry->is_global_scope = is_global_scope;
    entry->is_defined = is_defined;

    return symbol_count++;
}

static SymbolTableEntry *find_symbol_in_table(const char *symbol_name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].symbol_name, symbol_name) == 0) {
            return &symbol_table[i];
        }
    }
    return NULL;
}

void set_current_scope(int is_global_scope) {
    current_context_is_global = is_global_scope;
}

int declare_variable(const char *variable_name, const char *variable_type, 
                    int is_global_scope, int line_number, int column_number, 
                    const char *source_filename) {
    SymbolTableEntry *existing = find_symbol_in_table(variable_name);
    
    if (existing && existing->is_global_scope == is_global_scope) {
        error_report_issue(ERROR_REDECLARATION, line_number, column_number, source_filename, 
                          "Variable '%s' already declared in this scope", variable_name);
        return -1;
    }
    
    int idx = add_symbol_to_table(variable_name, variable_type, is_global_scope, 1);
    if (idx < 0) {
        error_report_issue(ERROR_UNKNOWN, line_number, column_number, source_filename, 
                          "Symbol table limit exceeded");
        return -1;
    }
    
    return idx;
}

int error_check_division_by_zero(int divisor_value, int line_number, int column_number, 
                                const char *source_filename) {
    if (divisor_value == 0) {
        error_report_issue(ERROR_DIVISION_BY_ZERO, line_number, column_number, source_filename, 
                          "Division by zero");
        return 1;
    }
    return 0;
}

int error_check_variable_exists(const char *variable_name, int line_number, int column_number, 
                               const char *source_filename) {
    SymbolTableEntry *entry = find_symbol_in_table(variable_name);
    if (!entry) {
        error_report_issue(ERROR_UNDEFINED_VARIABLE, line_number, column_number, source_filename, 
                          "Variable '%s' is not defined", variable_name);
        return 0;
    }
    return 1;
}

int error_check_type_compatibility(const char *first_type, const char *second_type, 
                                  const char *operation_type, int line_number, int column_number, 
                                  const char *source_filename) {
    if (strcmp(first_type, second_type) == 0) {
        if (strcmp(first_type, "string") == 0) {
            return error_check_string_operation_validity(operation_type, line_number, 
                                                       column_number, source_filename);
        }
        
        if (strcmp(operation_type, "%") == 0) {
            if (strcmp(first_type, "int") == 0) {
                return 1;
            } else {
                error_report_issue(ERROR_TYPE_MISMATCH, line_number, column_number, source_filename, 
                                 "Operation '%s' requires integer operands", operation_type);
                has_critical_error = 1;
                return 0;
            }
        }
        
        return 1;
    }
    
    if (strcmp(operation_type, ".") == 0) {
        if (strcmp(first_type, "string") == 0 && strcmp(second_type, "string") == 0) {
            return 1;
        }
    }
    
    error_report_issue(ERROR_TYPE_MISMATCH, line_number, column_number, source_filename, 
                       "Incompatible types: '%s' and '%s' for operation '%s'", 
                       first_type, second_type, operation_type);
    has_critical_error = 1;
    return 0;
}

int error_check_string_operation_validity(const char *operation_type, int line_number, 
                                         int column_number, const char *source_filename) {
    if (strcmp(operation_type, ".") == 0 || strcmp(operation_type, "=") == 0) {
        return 1;
    }
    
    error_report_issue(ERROR_INVALID_STRING_OP, line_number, column_number, source_filename, 
                       "Invalid operation '%s' for strings", operation_type);
    return 0;
}

int error_check_variable_scope_access(const char *variable_name, int is_global_scope, 
                                     int current_context_is_global, int line_number, 
                                     int column_number, const char *source_filename) {
    SymbolTableEntry *entry = find_symbol_in_table(variable_name);
    if (!entry) {
        return 0;
    }
    
    if (!current_context_is_global && entry->is_global_scope) {
        return 1;
    }
    
    if (entry->is_global_scope == is_global_scope) {
        return 1;
    }
    
    error_report_issue(ERROR_SCOPE, line_number, column_number, source_filename,
                       "Cannot access variable '%s' from current scope", variable_name);
    return 0;
}

int error_is_critical_error(void) {
    return has_critical_error;
}

void error_set_critical_error(void) {
    has_critical_error = 1;
} 