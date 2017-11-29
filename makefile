target: main.o parse.o
	@gcc $^ -o $@

target-debug: main-debug.o
	@gcc $< -o $@

main.o: main.c 
	@gcc -c $<

parse.o: parse.c
	@gcc -c $<

main-debug.o: main.c
	@gcc -g -c $< -o $@

parse-debug.o: parse.c
	@gcc -g -c $< -o $@

run: target
	@./$<

debug: target-debug
	@valgrind --leak-check=yes --track-origins=yes ./$<

clean:
	@rm *o
