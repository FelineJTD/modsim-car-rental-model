/* External definitions for car rental problem */
#include "simlib.h"

/* Defining constants */
/* Event types */
#define EVENT_ARRIVAL_AIR_TERMINAL_1 1
#define EVENT_ARRIVAL_AIR_TERMINAL_2 2
#define EVENT_ARRIVAL_CAR_RENTAL 3
#define EVENT_BUS_ARRIVAL_AIR_TERMINAL_1 4
#define EVENT_BUS_ARRIVAL_AIR_TERMINAL_2 5
#define EVENT_BUS_ARRIVAL_CAR_RENTAL 6
#define EVENT_BUS_DEPARTURE_AIR_TERMINAL_1 7
#define EVENT_BUS_DEPARTURE_AIR_TERMINAL_2 8
#define EVENT_BUS_DEPARTURE_CAR_RENTAL 9
#define EVENT_LOAD_AIR_TERMINAL_1 10
#define EVENT_LOAD_AIR_TERMINAL_2 11
#define EVENT_LOAD_CAR_RENTAL 12
#define EVENT_UNLOAD_AIR_TERMINAL_1 13
#define EVENT_UNLOAD_AIR_TERMINAL_2 14
#define EVENT_UNLOAD_CAR_RENTAL 15
#define EVENT_END_SIMULATION 16

/* Random number stream */
#define STREAM_INTERARRIVAL_AIR_TERMINAL_1 1
#define STREAM_INTERARRIVAL_AIR_TERMINAL_2 2
#define STREAM_INTERARRIVAL_CAR_RENTAL 3
#define STREAM_UNLOADING 4
#define STREAM_LOADING 5
#define STREAM_DETERMINE_DESTINATION 6

/* Statistic sampst */
#define SAMPST_DELAY_AIR_TERMINAL_1 1
#define SAMPST_DELAY_AIR_TERMINAL_2 2
#define SAMPST_DELAY_CAR_RENTAL 3
#define SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_1 4
#define SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_2 5
#define SAMPST_DELAY_BUS_STOP_CAR_RENTAL 6
#define SAMPST_DELAY_BUS_LOOP 7
#define SAMPST_DELAY_PEOPLE 8

/* Statistic timest */
#define TIMEST_QUEUE_AIR_TERMINAL_1 1
#define TIMEST_QUEUE_AIR_TERMINAL_2 2
#define TIMEST_QUEUE_CAR_RENTAL 3
#define TIMEST_BUS 4

/*** PROGRAM ***/
/* Declare non-simlib global variables. */
int num_location, num_air_terminal, simulation_length;
double expon_interarrival_rate[3], bus_speed, bus_capacity, bus_stop_time, destination_probability[2], uniform_load_time_range[2], uniform_unload_time_range[2], distance[3];
FILE *infile, *outfile;

// Input parser
void parse_input(char* file_name) {
    infile = fopen(file_name, "r");
    fscanf(infile, "%d %d %d", &num_location, &num_air_terminal, &simulation_length);
    for (int i = 0; i < num_location; i++) {
        fscanf(infile, "%lf", &expon_interarrival_rate[i]);
    }
    fscanf(infile, "%lf %lf %lf", &bus_speed, &bus_capacity, &bus_stop_time);
    for (int i = 0; i < num_air_terminal; i++) {
        fscanf(infile, "%lf", &destination_probability[i]);
    }
    for (int i = 0; i < 2; i++) {
        fscanf(infile, "%lf", &uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++) {
        fscanf(infile, "%lf", &uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++) {
        fscanf(infile, "%lf", &distance[i]);
    }
    fclose(infile);

    // Debug log
    printf("num_location: %d\n", num_location);
    printf("num_air_terminal: %d\n", num_air_terminal);
    printf("simulation_length: %d\n", simulation_length);
    for (int i = 0; i < num_location; i++) {
        printf("expon_interarrival_rate[%d]: %lf\n", i, expon_interarrival_rate[i]);
    }
    printf("bus_speed: %lf\n", bus_speed);
    printf("bus_capacity: %lf\n", bus_capacity);
    printf("bus_stop_time: %lf\n", bus_stop_time);
    for (int i = 0; i < num_air_terminal; i++) {
        printf("destination_probability[%d]: %lf\n", i, destination_probability[i]);
    }
    for (int i = 0; i < 2; i++) {
        printf("uniform_unload_time_range[%d]: %lf\n", i, uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++) {
        printf("uniform_load_time_range[%d]: %lf\n", i, uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++) {
        printf("distance[%d]: %lf\n", i, distance[i]);
    }
}

// Init model
void init_model() {
    // Init simlib
    init_simlib();

    // Reset all sampst and timest variables
    sampst(0.0, 0);
    timest(0.0, 0);

}

// print report
void print_report() {

}

// Main program
int main() {
    parse_input("car_rental.in");

    // Init model
    init_simlib();

    return 0;
}