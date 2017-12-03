# Makefile

CC = gcc

OBJS = main.o parse.o
DEBUG_OBJS = main-debug.o parse-debug.o

EXEC = target
DEBUG_EXEC = target-debug

$(EXEC): $(OBJS)
	@$(CC) $(OBJS) -o $@

$(DEBUG_EXEC): $(DEBUG_OBJS)
	@$(CC) $(DEBUG_OBJS) -o $@

main.o: main.c parse.h
	@$(CC) -c $<

parse.o: parse.c
	@$(CC) -c $<

main-debug.o: main.c parse.h
	@$(CC) -g -c $< -o $@

parse-debug.o: parse.c
	@$(CC) -g -c $< -o $@

run: $(EXEC)
	@./$<

test: $(EXEC)
	@./$< < test_file

debug: $(DEBUG_EXEC)
	@valgrind --leak-check=yes --track-origins=yes ./$<

clean:
	@rm *o
