# Define a DEBUG variable. If DEBUG is set to 1, include -g for debugging.
DEBUG ?= 0

# Object files
OBJECTS= ./build/compiler.o ./build/cprocess.o ./build/lexer.o ./build/token.o \
         ./build/lex_process.o ./build/parser.o ./build/node.o ./build/expressionable.o \
         ./build/helpers/buffer.o ./build/helpers/vector.o

# Include directories
INCLUDES= -I./

# If DEBUG is 1, include debugging flags (-g)
CFLAGS = $(if $(findstring 1, $(DEBUG)), -g, )

# Default target
all: ${OBJECTS}
	gcc main.c ${INCLUDES} ${OBJECTS} $(CFLAGS) -o ./main

# Object file rules
./build/compiler.o: ./compiler.c
	gcc ./compiler.c ${INCLUDES} $(CFLAGS) -o ./build/compiler.o -c

./build/cprocess.o: ./cprocess.c
	gcc ./cprocess.c ${INCLUDES} $(CFLAGS) -o ./build/cprocess.o -c

./build/lexer.o: ./lexer.c
	gcc ./lexer.c ${INCLUDES} $(CFLAGS) -o ./build/lexer.o -c

./build/token.o: ./token.c
	gcc ./token.c ${INCLUDES} $(CFLAGS) -o ./build/token.o -c

./build/lex_process.o: ./lex_process.c
	gcc ./lex_process.c ${INCLUDES} $(CFLAGS) -o ./build/lex_process.o -c

./build/parser.o: ./parser.c
	gcc ./parser.c ${INCLUDES} $(CFLAGS) -o ./build/parser.o -c

./build/node.o: ./node.c
	gcc ./node.c ${INCLUDES} $(CFLAGS) -o ./build/node.o -c

./build/expressionable.o: ./expressionable.c
	gcc ./expressionable.c ${INCLUDES} $(CFLAGS) -o ./build/expressionable.o -c

./build/helpers/buffer.o: ./helpers/buffer.c
	gcc ./helpers/buffer.c ${INCLUDES} $(CFLAGS) -o ./build/helpers/buffer.o -c

./build/helpers/vector.o: ./helpers/vector.c
	gcc ./helpers/vector.c ${INCLUDES} $(CFLAGS) -o ./build/helpers/vector.o -c

# Clean rule
clean:
	rm ./main
	rm -rf ${OBJECTS}

# Debug rule to build with debugging symbols
debug: DEBUG=1
debug: clean all
