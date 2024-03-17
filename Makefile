compile: car_rental.c simlib.c
	gcc car_rental.c simlib.c -o car_rental -lm

run: car_rental
	./car_rental

compile-run: compile run

clean: car_rental
	rm car_rental