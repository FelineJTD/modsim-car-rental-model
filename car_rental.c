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

/* Lists needed */
#define LIST_AIR_TERMINAL_1 1
#define LIST_AIR_TERMINAL_2 2
#define LIST_CAR_RENTAL 3
#define LIST_BUS_TO_AIR_TERMINAL_1 4
#define LIST_BUS_TO_AIR_TERMINAL_2 5
#define LIST_BUS_TO_CAR_RENTAL 6

/* Destination constants */
#define DESTINATION_AIR_TERMINAL_1 1
#define DESTINATION_AIR_TERMINAL_2 2
#define DESTINATION_CAR_RENTAL 3

/*** PROGRAM ***/
/* Declare non-simlib global variables. */
int num_location, num_air_terminal, simulation_length;
double expon_interarrival_rate[3], bus_speed, bus_capacity, bus_stop_time, destination_probability[2], uniform_load_time_range[2], uniform_unload_time_range[2], distance[3];
FILE *infile, *outfile;

// converter hour to minutes
double hour_to_minutes(double time) {
    return time * 60.0;
}

// converter second to minutes
double second_to_minutes(double time) {
    return time / 60.0;
}

// Input parser
void parse_input(char* file_name) {
    infile = fopen(file_name, "r");
    fscanf(infile, "%d %d %d", &num_location, &num_air_terminal, &simulation_length);
    for (int i = 0; i < num_location; i++) {
        fscanf(infile, "%lf", &expon_interarrival_rate[i]);
    }
    fscanf(infile, "%lf %lf %lf", &bus_speed, &bus_capacity, &bus_stop_time);
    for (int i = 0; i < num_location; i++) {
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
    for (int i = 0; i < num_location; i++) {
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

    // Schedule first arrival event of persons on each terminal
    // The mean interarrival time is 60.0 / expon_interarrival_rate (meaning the time between each person arrival is 60.0 / expon_interarrival_rate minutes)
    event_schedule(expon(hour_to_minutes(1 / expon_interarrival_rate[0]), STREAM_INTERARRIVAL_AIR_TERMINAL_1), EVENT_ARRIVAL_AIR_TERMINAL_1);
    event_schedule(expon(hour_to_minutes(1 / expon_interarrival_rate[1]), STREAM_INTERARRIVAL_AIR_TERMINAL_2), EVENT_ARRIVAL_AIR_TERMINAL_2);
    event_schedule(expon(hour_to_minutes(1 / expon_interarrival_rate[2]), STREAM_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);

    // Schedule first bus arrival event from terminal 3 to 1. The time is distance to terminal 1 / bus_speed * 60.0 (convert to minutes)
    event_schedule(hour_to_minutes((distance[0] / bus_speed)), EVENT_BUS_ARRIVAL_AIR_TERMINAL_1);

    // Schedule simulation end
    event_schedule(hour_to_minutes(simulation_length), EVENT_END_SIMULATION);
}

// Arrival event
void arrive(int event_type) {
    if (event_type == EVENT_ARRIVAL_AIR_TERMINAL_1) {
        // preload list data on transfer
        transfer[1] = sim_time;
        transfer[3] = DESTINATION_AIR_TERMINAL_1;
        list_file(LAST, LIST_AIR_TERMINAL_1);
        timest(list_size[LIST_AIR_TERMINAL_1], TIMEST_QUEUE_AIR_TERMINAL_1);
        event_schedule(sim_time + expon(hour_to_minutes(1 / expon_interarrival_rate[0]), STREAM_INTERARRIVAL_AIR_TERMINAL_1), EVENT_ARRIVAL_AIR_TERMINAL_1);
    } else if (event_type == EVENT_ARRIVAL_AIR_TERMINAL_2) {
        // preload list data on transfer
        transfer[1] = sim_time;
        transfer[3] = DESTINATION_AIR_TERMINAL_2;
        list_file(LAST, LIST_AIR_TERMINAL_2);
        timest(list_size[LIST_AIR_TERMINAL_2], TIMEST_QUEUE_AIR_TERMINAL_2);
        event_schedule(sim_time + expon(hour_to_minutes(1 / expon_interarrival_rate[1]), STREAM_INTERARRIVAL_AIR_TERMINAL_2), EVENT_ARRIVAL_AIR_TERMINAL_2);
    } else if (event_type == EVENT_ARRIVAL_CAR_RENTAL) {
        transfer[1] = sim_time;
        
        if (random_integer(destination_probability, STREAM_DETERMINE_DESTINATION) == 1) {
            transfer[3] = DESTINATION_AIR_TERMINAL_1;
            
        } else {
            transfer[3] = DESTINATION_AIR_TERMINAL_2;
        }
        list_file(LAST, LIST_CAR_RENTAL);
        timest(list_size[LIST_CAR_RENTAL], TIMEST_QUEUE_CAR_RENTAL);
        event_schedule(sim_time + expon(hour_to_minutes(1 / expon_interarrival_rate[2]), STREAM_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);
    }
}

// Bus arrival event
void bus_arrival(int event_type) {

}

// print report
void print_report(char* file_name) {
    outfile = fopen(file_name, "w");
    fprintf(outfile, "num_location: %d\n", num_location);
    fprintf(outfile, "num_air_terminal: %d\n", num_air_terminal);
    fprintf(outfile, "simulation_length: %d\n", simulation_length);
    for (int i = 0; i < num_location; i++) {
        fprintf(outfile, "expon_interarrival_rate[%d]: %.3lf\n", i, expon_interarrival_rate[i]);
    }
    fprintf(outfile, "bus_speed: %.3lf\n", bus_speed);
    fprintf(outfile, "bus_capacity: %.3lf\n", bus_capacity);
    fprintf(outfile, "bus_stop_time: %.3lf\n", bus_stop_time);
    for (int i = 0; i < num_location; i++) {
        fprintf(outfile, "destination_probability[%d]: %.3lf\n", i, destination_probability[i]);
    }
    for (int i = 0; i < 2; i++) {
        fprintf(outfile, "uniform_unload_time_range[%d]: %.3lf\n", i, uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++) {
        fprintf(outfile, "uniform_load_time_range[%d]: %.3lf\n", i, uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++) {
        fprintf(outfile, "distance[%d]: %.3lf\n", i, distance[i]);
    }
}

// Main program
int main() {
    parse_input("car_rental.in");

    // Init model and initialize first events
    init_simlib();

    // Invoke timing and determine action based on next event type
    while (next_event_type != EVENT_END_SIMULATION) {
        timing();
        switch (next_event_type) {
            case EVENT_ARRIVAL_AIR_TERMINAL_1:
                arrive(EVENT_ARRIVAL_AIR_TERMINAL_1);
                break;
            case EVENT_ARRIVAL_AIR_TERMINAL_2:
                arrive(EVENT_ARRIVAL_AIR_TERMINAL_2);
                break;
            case EVENT_ARRIVAL_CAR_RENTAL:
                arrive(EVENT_ARRIVAL_CAR_RENTAL);
                break;
            case EVENT_BUS_ARRIVAL_AIR_TERMINAL_1:
                bus_arrival(EVENT_BUS_ARRIVAL_AIR_TERMINAL_1);
                break;
            case EVENT_BUS_ARRIVAL_AIR_TERMINAL_2:
                bus_arrival(EVENT_BUS_ARRIVAL_AIR_TERMINAL_2);
                break;
            case EVENT_BUS_ARRIVAL_CAR_RENTAL:
                bus_arrival(EVENT_BUS_ARRIVAL_CAR_RENTAL);
                break;
        }
    }

    print_report("car_rental.out");

    return 0;
}