#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

typedef enum {
    ERROR_NONE = 0,
    ERROR_SYNTAX,               // Синтаксическая ошибка
    
    ERROR_DIVISION_BY_ZERO,     // Деление на ноль
    ERROR_UNDEFINED_VARIABLE,   // Попытка использовать неопределенную переменную
    ERROR_TYPE_MISMATCH,        // Несовместимые типы данных
    ERROR_INVALID_STRING_OP,    // Недопустимая операция со строкой
    ERROR_SCOPE,                // Ошибка области видимости
    ERROR_UNKNOWN,              // Неизвестная ошибка
    ERROR_REDECLARATION         // Переопределение переменной
} CompilerErrorType;

typedef struct {
    CompilerErrorType error_type;
    int line_number;
    int column_number;
    char error_message[256];
    char source_filename[256];
} CompilerError;

void error_system_init(void);

void error_system_cleanup(void);

void error_report_issue(CompilerErrorType error_type, int line_number, int column_number, 
                       const char *source_filename, const char *message_format, ...);

int error_has_any_errors(void);

int error_get_total_count(void);

void error_print_all_errors(FILE *output_stream);

void error_clear_all(void);

int error_check_division_by_zero(int divisor_value, int line_number, int column_number, 
                                const char *source_filename);

int error_check_variable_exists(const char *variable_name, int line_number, int column_number, 
                               const char *source_filename);

int error_check_type_compatibility(const char *first_type, const char *second_type, 
                                  const char *operation_type, int line_number, int column_number, 
                                  const char *source_filename);

int error_check_string_operation_validity(const char *operation_type, int line_number, 
                                        int column_number, const char *source_filename);

int error_check_variable_scope_access(const char *variable_name, int is_global_scope, 
                                     int current_context_is_global, int line_number, 
                                     int column_number, const char *source_filename);

int error_is_critical_error(void);

void error_set_critical_error(void);

#endif /* ERROR_H */ 