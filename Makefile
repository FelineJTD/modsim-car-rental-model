compile: car_rental.c simlib.c
	gcc car_rental.c simlib.c -o car_rental -lm

clean: car_rental
	rm car_rental