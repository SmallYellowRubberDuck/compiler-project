%{
#include "../core/ast.h"
#include "../parser/parser.tab.h" 
#include <string.h>

int current_line_number = 1;
int current_column_number = 1;
char current_line_text[1024] = "";
int current_line_length = 0;

// Функция для обновления позиции столбца
void update_column_position() {
    current_column_number += yyleng;
}

// Функция для обновления номера строки
void update_line_number() {
    current_line_number++;
    current_column_number = 1;
}

void get_token_position(int *line, int *column) {
    if (line) *line = current_line_number;
    if (column) *column = current_column_number;
}

int get_current_line() { return current_line_number; }
int get_current_column() { return current_column_number; }
%}

%x COMMENT

%%

"if"      {update_column_position(); return IF; }
"else"    {update_column_position(); return ELSE; }
"while"   {update_column_position(); return WHILE; }
"for"     {update_column_position(); return FOR; }
"from"    {update_column_position(); return FROM; }
"to"      {update_column_position(); return TO; }
"step"    {update_column_position(); return STEP; }
"global"  {update_column_position(); return GLOBAL; }
"local"   {update_column_position(); return LOCAL; }
"int"     {update_column_position(); return INT; }
"string"  {update_column_position(); return STRING; }
"and"     {update_column_position(); return AND; }
"or"      {update_column_position(); return OR; }
"print"   {update_column_position(); return PRINT; }

[0-9]+                { update_column_position(); yylval.ival = atoi(yytext); return INT_LITERAL; }
\"[^\"]*\"            { update_column_position(); yylval.sval = strdup(yytext); return STRING_LITERAL; }

[a-zA-Z_][a-zA-Z0-9_]* { update_column_position(); yylval.sval = strdup(yytext); return IDENTIFIER; }

"=="                  { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }
"!="                  { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }
"<="                  { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }
">="                  { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }
"<"                   { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }
">"                   { update_column_position(); yylval.sval = strdup(yytext); return COMPARE; }

"+"                   { update_column_position(); return '+'; }
"-"                   { update_column_position(); return '-'; }
"*"                   { update_column_position(); return '*'; }
"/"                   { update_column_position(); return '/'; }
"%"                   { update_column_position(); return '%'; }
"="                   { update_column_position(); return '='; }
"."                   { update_column_position(); return '.'; }

"("                   { update_column_position(); return '('; }
")"                   { update_column_position(); return ')'; }
"{"                   { update_column_position(); return '{'; }
"}"                   { update_column_position(); return '}'; }
";"                   { update_column_position(); return ';'; }
","                   { update_column_position(); return ','; }

[ \t]+                { update_column_position(); /* Пробелы — игнорировать*/ }
[\r]                  { current_column_number = 1; /* Возврат каретки */ }
[\n]                  { update_line_number(); /* Перенос строки */ }

.                     { update_column_position(); return UNKNOWN;}

"//".*                { update_column_position(); /* игнорируем однострочный комментарий */ }
"/*"                  { update_column_position(); BEGIN(COMMENT); }
<COMMENT>"*/"         { update_column_position(); BEGIN(INITIAL); }
<COMMENT>.            { update_column_position(); /* внутри комментария */ }
<COMMENT>\n           { update_line_number(); /* внутри комментария */ }

%%

int yywrap(void) {
    return 1;
}