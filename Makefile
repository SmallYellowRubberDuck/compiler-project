CC = gcc
CFLAGS = -Wall -g -Wno-unused-function
LOCAL_INCLUDES = -I. -Isrc

FLEX = flex
BISON = bison
FLEX_FLAGS = 
BISON_FLAGS = -d -v

SRCS = src/core/compiler.c \
       src/core/ast.c \
       src/core/error.c \
       src/codegen/codegen.c \
       src/utils/visualizer.c \
       src/parser/parser.tab.c \
       src/lexer/lex.yy.c

OBJS = $(SRCS:.c=.o)
EXECUTABLE = compiler.exe

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(LOCAL_INCLUDES) -c $< -o $@

src/lexer/lex.yy.c: src/lexer/lexer.l
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

src/parser/parser.tab.c src/parser/parser.tab.h: src/parser/parser.y
	$(BISON) $(BISON_FLAGS) -o src/parser/parser.tab.c $<

src/core/compiler.o: src/parser/parser.tab.h src/core/error.h
src/parser/parser.tab.o: src/parser/parser.tab.c
src/lexer/lex.yy.o: src/lexer/lex.yy.c
src/codegen/codegen.o: src/codegen/codegen.c src/codegen/codegen.h src/core/ast.h src/core/error.h
src/utils/visualizer.o: src/utils/visualizer.c src/utils/visualizer.h src/core/ast.h
src/core/error.o: src/core/error.c src/core/error.h

clean:
	-rm -f $(OBJS) $(EXECUTABLE) src/parser/parser.tab.c src/parser/parser.tab.h src/lexer/lex.yy.c src/parser/parser.output 