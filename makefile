target: main.o
	@gcc main.o -o $@

main.o: main.c
	@gcc -c $<

run: target
	@./$<

clean:
	@rm *o
