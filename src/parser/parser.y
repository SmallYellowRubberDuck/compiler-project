%{
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../core/ast.h"


extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);
extern FILE* yyin;

extern int get_current_line(void);
extern int get_current_column(void);
extern void get_token_position(int *line, int *column);


static char current_filename[256] = "";


void set_parser_filename(const char* filename) {
    if (filename) {
        strncpy(current_filename, filename, sizeof(current_filename) - 1);
        current_filename[sizeof(current_filename) - 1] = '\0';
    } else {
        strcpy(current_filename, "unknown");
    }
}

const char* get_parser_filename(void) {
    return current_filename;
}

extern ASTNode* ast_program_root;
%}


%union{
   int ival;
   char* sval;
   ASTNode* node;
}

%token <ival> INT_LITERAL
%token <sval> IDENTIFIER STRING_LITERAL COMPARE

%token IF ELSE WHILE FOR FROM TO STEP
%token GLOBAL LOCAL PRINT
%token AND OR
%token INT STRING
%token UNKNOWN

%type <node> program operation_list operation declaration
%type <node> expr print_opr if_opr while_opr for_opr block_stmt
%type <node> assignment scope_type

%right '='
%left '+' '-'
%left '*' '/' '%'
%left '.'

%start program

%%

program
    : operation_list  { ast_program_root = $1; }
    ;

operation_list
    : operation_list operation 
    { 
        $$ = $1; 
        if ($2) { add_node_to_block($$, $2); }
    }
    | operation 
    { 
        $$ = create_program_node(); 
        if ($1) { add_node_to_block($$, $1); }
    }
    ;

operation
    : declaration ';'    { $$ = $1; }
    | assignment ';'     { $$ = $1; }
    | print_opr ';'      { $$ = $1; }
    | if_opr             { $$ = $1; }
    | while_opr          { $$ = $1; }
    | for_opr            { $$ = $1; }
    | block_stmt         { $$ = $1; }
    ;

scope_type
    : GLOBAL { $$ = create_literal_int(1); } /* global = глобальная */
    | LOCAL  { $$ = create_literal_int(0); } /* local = локальная */
    ;

declaration
    : INT scope_type IDENTIFIER  
    { 
        int is_global = $2->literal.int_value;
        $$ = create_variable_declaration($3, "int", is_global);
        free_node($2);
        free($3);
    }
    | STRING scope_type IDENTIFIER  
    { 
        int is_global = $2->literal.int_value;
        $$ = create_variable_declaration($3, "string", is_global);
        free_node($2);
        free($3);
    }
    | INT scope_type IDENTIFIER '=' expr
    { 
        int is_global = $2->literal.int_value;
        $$ = create_variable_declaration_with_initializer($3, "int", is_global, $5);
        free_node($2);
        free($3);
    }
    | STRING scope_type IDENTIFIER '=' expr
    { 
        int is_global = $2->literal.int_value;
        $$ = create_variable_declaration_with_initializer($3, "string", is_global, $5);
        free_node($2);
        free($3);
    }
    ;

assignment
    : IDENTIFIER '=' expr 
    { 
        $$ = create_assignment($1, $3);
        free($1);
    }
    ;

print_opr
    : PRINT '(' expr ')'
    { 
        $$ = create_print_statement($3);
    }
    ;

if_opr
    : IF '(' expr ')' operation
    { 
        $$ = create_if_statement($3, $5, NULL);
    }
    | IF '(' expr ')' operation ELSE operation
    { 
        $$ = create_if_statement($3, $5, $7);
    }
    ;

while_opr
    : WHILE '(' expr ')' operation
    { 
        $$ = create_while_loop($3, $5);
    }
    ;

for_opr
    : FOR IDENTIFIER FROM expr TO expr STEP expr operation
    { 
        $$ = create_round_loop($2, $4, $6, $8, $9);
        free($2);
    }
    ;

block_stmt
    : '{' operation_list '}'
    { 
        $$ = create_block();
        // Copy child nodes from operation_list to block
        if ($2 && ($2->type == NODE_PROGRAM || $2->type == NODE_BLOCK)) {
            NodeList* children = &$2->block.children;
            for (size_t i = 0; i < children->size; i++) {
                add_node_to_block($$, children->items[i]);
            }
            // Clear parent node but not its children (they now belong to new block)
            children->size = 0;
            free_node($2);
        }
    }
    ;

expr
    : expr '+' expr
    { 
        $$ = create_binary_operation("+", $1, $3);
    }
    | expr '-' expr
    { 
        $$ = create_binary_operation("-", $1, $3);
    }
    | expr '*' expr
    { 
        $$ = create_binary_operation("*", $1, $3);
    }
    | expr '/' expr
    { 
        $$ = create_binary_operation("/", $1, $3);
    }
    | expr '%' expr
    { 
        $$ = create_binary_operation("%", $1, $3);
    }
    | expr '.' expr
    { 
        $$ = create_binary_operation(".", $1, $3);
    }
    | expr COMPARE expr
    { 
        $$ = create_binary_operation($2, $1, $3);
        free($2);
    }
    | expr AND expr
    { 
        $$ = create_binary_operation("and", $1, $3);
    }
    | expr OR expr
    { 
        $$ = create_binary_operation("or", $1, $3);
    }
    | '(' expr ')'
    { 
        $$ = $2;
    }
    | INT_LITERAL
    { 
        $$ = create_literal_int($1);
    }
    | STRING_LITERAL
    { 
        $$ = create_literal_string($1);
        free($1);
    }
    | IDENTIFIER
    { 
        $$ = create_identifier($1);
        free($1);
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parser error at %s:%d:%d: %s\n", 
            current_filename, get_current_line(), get_current_column(), s);
    exit(1);
}

int parser_init(const char* filename) {
    // Open input file
    FILE* input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return 1;
    }
    yyin = input_file;
    
    // Set filename
    set_parser_filename(filename);
    
    // Start parser
    yyparse();
    
    // Close file
    fclose(input_file);
    
    return 0;
}

ASTNode* get_ast_root() {
    return ast_program_root;
}

