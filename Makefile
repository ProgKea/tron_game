CC?=clang
LIBS=-lraylib -lGL -lm -lpthread -ldl -lrt
FLAGS=-std=c99 -Wextra -Wall -Wshadow -Wstrict-aliasing -Wstrict-overflow -pedantic -O2

tron: main.c
	${CC} ${FLAGS} -o $@ $< ${LIBS}
