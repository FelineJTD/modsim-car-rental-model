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
#define SAMPST_DELAY_PEOPLE_AIR_TERMINAL_1 8
#define SAMPST_DELAY_PEOPLE_AIR_TERMINAL_2 9
#define SAMPST_DELAY_PEOPLE_CAR_RENTAL 10

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

/* Bus location constants */
#define LOCATION_AIR_TERMINAL_1 1
#define LOCATION_AIR_TERMINAL_2 2
#define LOCATION_CAR_RENTAL 3
#define LOCATION_MOVING 0

/*** PROGRAM ***/
/* Declare non-simlib global variables. */
int num_location, num_air_terminal, simulation_length, bus_capacity, bus_location;
double expon_interarrival_rate[3], bus_speed, bus_stop_time, destination_probability[3], uniform_load_time_range[2], uniform_unload_time_range[2], distance[3];
double bus_arrive_time, bus_departure_time, person_arrive_time;
FILE *infile, *outfile;

// converter hour to minutes
double hour_to_minutes(double time)
{
    return time * 60.0;
}

// converter second to minutes
double second_to_minutes(double time)
{
    return time / 60.0;
}

double total_in_bus()
{
    return list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] + list_size[LIST_BUS_TO_CAR_RENTAL];
}

// Input parser
// units: time in minutes, speed in miles per hour, distance in miles
void parse_input(char *file_name)
{
    infile = fopen(file_name, "r");
    fscanf(infile, "%d %d %d", &num_location, &num_air_terminal, &simulation_length);
    simulation_length = hour_to_minutes(simulation_length);
    fscanf(infile, "%lf %d %lf", &bus_speed, &bus_capacity, &bus_stop_time);
    for (int i = 0; i < num_location; i++)
    {
        fscanf(infile, "%lf", &expon_interarrival_rate[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        fscanf(infile, "%lf", &destination_probability[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        fscanf(infile, "%lf", &uniform_unload_time_range[i]);
        uniform_unload_time_range[i] = second_to_minutes(uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        fscanf(infile, "%lf", &uniform_load_time_range[i]);
        uniform_load_time_range[i] = second_to_minutes(uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        fscanf(infile, "%lf", &distance[i]);
    }
    fclose(infile);

    // Debug log
    printf("- Printing input...\n");
    printf("  num_location: %d\n", num_location);
    printf("  num_air_terminal: %d\n", num_air_terminal);
    printf("  simulation_length: %d\n", simulation_length);
    printf("  bus_speed: %lf\n", bus_speed);
    printf("  bus_capacity: %d\n", bus_capacity);
    printf("  bus_stop_time: %lf\n", bus_stop_time);
    for (int i = 0; i < num_location; i++)
    {
        printf("  expon_interarrival_rate[%d]: %lf\n", i, expon_interarrival_rate[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        printf("  destination_probability[%d]: %lf\n", i, destination_probability[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        printf("  uniform_unload_time_range[%d]: %lf\n", i, uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        printf("  uniform_load_time_range[%d]: %lf\n", i, uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        printf("  distance[%d]: %lf\n", i, distance[i]);
    }
    printf("* Input printed.");
}

// Init model
void init_model()
{
    printf("- Initializing model...\n");
    // Init simlib
    printf("- Initializing simlib...\n");
    init_simlib();

    // Reset all sampst and timest variables
    printf("- Resetting all sampst and timest variables...\n");
    sampst(0.0, 0);
    timest(0.0, 0);

    // Init global variable
    bus_departure_time = 0.0;
    bus_location = LOCATION_CAR_RENTAL;

    // Schedule first arrival event of persons on each location
    // The mean interarrival time is 60.0 / expon_interarrival_rate (meaning the time between each person arrival is 60.0 / expon_interarrival_rate minutes)
    printf("- Scheduling first arrival event of persons on each locations...\n");
    // sim_time = 0
    event_schedule(expon(60 / expon_interarrival_rate[0], STREAM_INTERARRIVAL_AIR_TERMINAL_1), EVENT_ARRIVAL_AIR_TERMINAL_1);
    event_schedule(expon(60 / expon_interarrival_rate[1], STREAM_INTERARRIVAL_AIR_TERMINAL_2), EVENT_ARRIVAL_AIR_TERMINAL_2);
    event_schedule(expon(60 / expon_interarrival_rate[2], STREAM_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);

    // Schedule first bus arrival
    event_schedule(0.0, EVENT_BUS_ARRIVAL_CAR_RENTAL);

    // Schedule simulation end
    printf("- Scheduling simulation end...\n");
    event_schedule(simulation_length, EVENT_END_SIMULATION);

    printf("* Model initialized.\n\n");
}

// Arrival event
void arrive(int event_type)
{
    if (event_type == EVENT_ARRIVAL_AIR_TERMINAL_1)
    {
        transfer[1] = sim_time;
        transfer[3] = DESTINATION_CAR_RENTAL;
        list_file(LAST, LIST_AIR_TERMINAL_1);
        printf("[%7.2lf] Person arrived at air terminal 1.\n", sim_time);
        printf("          Number of people waiting at air terminal 1: %d\n", list_size[LIST_AIR_TERMINAL_1]);
        // if bus is available and queue is empty, load the bus immediately
        if (bus_location == LOCATION_AIR_TERMINAL_1 && list_size[LIST_AIR_TERMINAL_1] == 1 && total_in_bus() < bus_capacity)
        { // ini ga ngecek capacity bus total aja
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_1);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            }
        }
        else
        {
            // answer a
            timest(list_size[LIST_AIR_TERMINAL_1], TIMEST_QUEUE_AIR_TERMINAL_1);
        }
        // schedule next arrival
        event_schedule(sim_time + expon(60 / expon_interarrival_rate[0], STREAM_INTERARRIVAL_AIR_TERMINAL_1), EVENT_ARRIVAL_AIR_TERMINAL_1);
    }
    else if (event_type == EVENT_ARRIVAL_AIR_TERMINAL_2)
    {
        transfer[1] = sim_time;
        transfer[3] = DESTINATION_CAR_RENTAL;
        list_file(LAST, LIST_AIR_TERMINAL_2);
        printf("[%7.2lf] Person arrived at air terminal 2.\n", sim_time);
        printf("          Number of people waiting at air terminal 2: %d\n", list_size[LIST_AIR_TERMINAL_2]);
        // if bus is available and queue is empty, load the bus immediately
        if (bus_location == LOCATION_AIR_TERMINAL_2 && list_size[LIST_AIR_TERMINAL_2] == 1 && total_in_bus() < bus_capacity)
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_2);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            }
        }
        else
        {
            // answer a
            timest(list_size[LIST_AIR_TERMINAL_2], TIMEST_QUEUE_AIR_TERMINAL_2);
        }
        // schedule next arrival
        event_schedule(sim_time + expon(60 / expon_interarrival_rate[1], STREAM_INTERARRIVAL_AIR_TERMINAL_2), EVENT_ARRIVAL_AIR_TERMINAL_2);
    }
    else if (event_type == EVENT_ARRIVAL_CAR_RENTAL)
    {
        transfer[1] = sim_time;

        if (random_integer(destination_probability, STREAM_DETERMINE_DESTINATION) == 1)
        {
            transfer[3] = DESTINATION_AIR_TERMINAL_1;
        }
        else
        {
            transfer[3] = DESTINATION_AIR_TERMINAL_2;
        }
        list_file(LAST, LIST_CAR_RENTAL);

        printf("[%7.2lf] Person arrived at car rental.\n", sim_time);
        printf("          Number of people waiting at car rental: %d\n", list_size[LIST_CAR_RENTAL]);

        // if bus is available and queue is empty, load the bus immediately
        if (bus_location == LOCATION_CAR_RENTAL && list_size[LIST_CAR_RENTAL] == 1 && total_in_bus() < bus_capacity)
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_CAR_RENTAL);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_CAR_RENTAL);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
            }
        }
        else
        {
            // answer a
            timest(list_size[LIST_CAR_RENTAL], TIMEST_QUEUE_CAR_RENTAL);
        }
        // schedule next arrival
        event_schedule(sim_time + expon(60 / expon_interarrival_rate[2], STREAM_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);
    }
}

// Bus arrival event
void bus_arrival(int event_type)
{
    if (event_type == EVENT_BUS_ARRIVAL_AIR_TERMINAL_1)
    {
        printf("[%7.2lf] Bus arrived at air terminal 1.\n", sim_time);
        printf("          Number of people departing at air terminal 1: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_1]);
        bus_location = LOCATION_AIR_TERMINAL_1;
        bus_arrive_time = sim_time;
        // unload people wanting to go to air terminal 1
        if (list_size[LIST_BUS_TO_AIR_TERMINAL_1] > 0)
        {
            event_schedule(sim_time + uniform(uniform_unload_time_range[0], uniform_unload_time_range[1], STREAM_UNLOADING), EVENT_UNLOAD_AIR_TERMINAL_1);
        }
        // load people if there is any
        else if (list_size[LIST_AIR_TERMINAL_1] > 0)
        {
            event_schedule(sim_time + uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING), EVENT_LOAD_AIR_TERMINAL_1);
        }
        // schedule departure in 5 minutes
        event_schedule(sim_time + bus_stop_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
    }
    else if (event_type == EVENT_BUS_ARRIVAL_AIR_TERMINAL_2)
    {
        printf("[%7.2lf] Bus arrived at air terminal 2.\n", sim_time);
        printf("          Number of people departing at air terminal 2: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_2]);
        bus_location = LOCATION_AIR_TERMINAL_2;
        bus_arrive_time = sim_time;
        // unload people wanting to go to air terminal 2
        if (list_size[LIST_BUS_TO_AIR_TERMINAL_2] > 0)
        {
            event_schedule(sim_time + uniform(uniform_unload_time_range[0], uniform_unload_time_range[1], STREAM_UNLOADING), EVENT_UNLOAD_AIR_TERMINAL_2);
        }
        // load people if there is any
        else if (list_size[LIST_AIR_TERMINAL_2] > 0)
        {
            event_schedule(sim_time + uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING), EVENT_LOAD_AIR_TERMINAL_2);
        }
        // schedule departure in 5 minutes
        event_schedule(sim_time + bus_stop_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
    }
    else if (event_type == EVENT_BUS_ARRIVAL_CAR_RENTAL)
    {
        printf("[%7.2lf] Bus arrived at car rental.\n", sim_time);
        printf("          Number of people departing at car rental: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        bus_location = LOCATION_CAR_RENTAL;
        bus_arrive_time = sim_time;
        // unload people wanting to go to car rental
        if (list_size[LIST_BUS_TO_CAR_RENTAL] > 0)
        {
            event_schedule(sim_time + uniform(uniform_unload_time_range[0], uniform_unload_time_range[1], STREAM_UNLOADING), EVENT_UNLOAD_CAR_RENTAL);
        }
        // load people if there is any
        else if (list_size[LIST_CAR_RENTAL] > 0)
        {
            event_schedule(sim_time + uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING), EVENT_LOAD_CAR_RENTAL);
        }
        // schedule departure in 5 minutes
        event_schedule(sim_time + bus_stop_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
    }
}

// Bus departure event
void bus_departure(int event_type)
{
    if (event_type == EVENT_BUS_DEPARTURE_AIR_TERMINAL_1)
    {
        printf("[%7.2lf] Bus departed from air terminal 1 to 2...\n", sim_time);
        printf("          Number of people departing from air terminal 1: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        bus_location = LOCATION_MOVING;
        event_schedule(sim_time + hour_to_minutes((distance[1] / bus_speed)), EVENT_BUS_ARRIVAL_AIR_TERMINAL_2);
        // answer d
        sampst(sim_time - bus_arrive_time, SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_1);
    }
    else if (event_type == EVENT_BUS_DEPARTURE_AIR_TERMINAL_2)
    {
        printf("[%7.2lf] Bus departed from air terminal 2 to car rental...\n", sim_time);
        printf("          Number of people departing from air terminal 2: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        bus_location = LOCATION_MOVING;
        event_schedule(sim_time + hour_to_minutes((distance[2] / bus_speed)), EVENT_BUS_ARRIVAL_CAR_RENTAL);
        // answer d
        sampst(sim_time - bus_arrive_time, SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_2);
    }
    else if (event_type == EVENT_BUS_DEPARTURE_CAR_RENTAL)
    {
        printf("[%7.2lf] Bus departed from car rental to air terminal 1...\n", sim_time);
        printf("          Number of people departing from car rental: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2]);
        bus_location = LOCATION_MOVING;
        event_schedule(sim_time + hour_to_minutes((distance[0] / bus_speed)), EVENT_BUS_ARRIVAL_AIR_TERMINAL_1);
        // answer d
        sampst(sim_time - bus_arrive_time, SAMPST_DELAY_BUS_STOP_CAR_RENTAL);
        // answer e
        if (bus_departure_time > 0.0)
        {
            sampst(sim_time - bus_departure_time, SAMPST_DELAY_BUS_LOOP);
        }
        bus_departure_time = sim_time;
    }
    // Answer c
    timest(list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] + list_size[LIST_BUS_TO_CAR_RENTAL], TIMEST_BUS);
}

// Unload event
// When bus arrives at a location, it will unload passengers
void unload(int event_type)
{
    if (event_type == EVENT_UNLOAD_AIR_TERMINAL_1)
    {
        list_remove(FIRST, LIST_BUS_TO_AIR_TERMINAL_1);
        // answer f
        sampst(sim_time - transfer[1], SAMPST_DELAY_PEOPLE_AIR_TERMINAL_1);
        printf("[%7.2lf] Passenger unloaded at air terminal 1.\n", sim_time);
        printf("          Number of people in the bus wanting to go to terminal 1: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_1]);
        // If there are still passengers in the bus, schedule next unloading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_BUS_TO_AIR_TERMINAL_1] > 0)
        {
            double unload_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_UNLOADING);
            event_schedule(sim_time + unload_time, EVENT_UNLOAD_AIR_TERMINAL_1);
            if (sim_time + unload_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
                event_schedule(sim_time + unload_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            }
        }
        // If there are no more passengers, schedule next loading event (and reschedule departure if delay > 5 minutes)
        else
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_1);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            }
        }
    }
    else if (event_type == EVENT_UNLOAD_AIR_TERMINAL_2)
    {
        list_remove(FIRST, LIST_BUS_TO_AIR_TERMINAL_2);
        // answer f
        sampst(sim_time - transfer[1], SAMPST_DELAY_PEOPLE_AIR_TERMINAL_2);
        printf("[%7.2lf] Passenger unloaded at air terminal 2.\n", sim_time);
        printf("          Number of people in the bus wanting to go to terminal 2: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_2]);
        // If there are still passengers in the bus, schedule next unloading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_BUS_TO_AIR_TERMINAL_2] > 0)
        {
            double unload_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_UNLOADING);
            event_schedule(sim_time + unload_time, EVENT_UNLOAD_AIR_TERMINAL_2);
            if (sim_time + unload_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
                event_schedule(sim_time + unload_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            }
        }
        // If there are no more passengers, schedule next loading event (and reschedule departure if delay > 5 minutes)
        else
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_2);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            }
        }
    }
    else if (event_type == EVENT_UNLOAD_CAR_RENTAL)
    {
        list_remove(FIRST, LIST_BUS_TO_CAR_RENTAL);
        // answer f
        sampst(sim_time - transfer[1], SAMPST_DELAY_PEOPLE_CAR_RENTAL);
        printf("[%7.2lf] Passenger unloaded at car rental.\n", sim_time);
        printf("          Number of people in the bus wanting to go to car rental: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        // If there are still passengers in the bus, schedule next unloading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_BUS_TO_CAR_RENTAL] > 0)
        {
            double unload_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_UNLOADING);
            event_schedule(sim_time + unload_time, EVENT_UNLOAD_CAR_RENTAL);
            if (sim_time + unload_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_CAR_RENTAL);
                event_schedule(sim_time + unload_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
            }
        }
        // If there are no more passengers, schedule next loading event (and reschedule departure if delay > 5 minutes)
        else
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_CAR_RENTAL);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_CAR_RENTAL);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
            }
        }
    }
}

// Load event
// When no more passengers to unload, bus will load passengers
// TODO: validate capacity
void load(int event_type)
{
    if (event_type == EVENT_LOAD_AIR_TERMINAL_1)
    {
        // Remove a person from the queue at air terminal 1 and put it in the bus, direction to car rental
        list_remove(FIRST, LIST_AIR_TERMINAL_1);
        person_arrive_time = transfer[1];
        // answer b
        sampst(sim_time - person_arrive_time, SAMPST_DELAY_AIR_TERMINAL_1);
        transfer[3] = LOCATION_AIR_TERMINAL_1;
        list_file(LAST, LIST_BUS_TO_CAR_RENTAL);
        printf("[%7.2lf] Passenger loaded to bus, direction to car rental.\n", sim_time);
        printf("          Number of people waiting at air terminal 1: %d\n", list_size[LIST_AIR_TERMINAL_1]);
        printf("          Number of people in the bus: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        // If there are still passengers in the queue and there is still capacity, schedule next loading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_AIR_TERMINAL_1] > 0 && list_size[LIST_BUS_TO_CAR_RENTAL] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] < bus_capacity)
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_1);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            }
        }
        // If there is no more capacity
        else if (list_size[LIST_BUS_TO_CAR_RENTAL] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] == bus_capacity)
        {
            event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            if (sim_time < bus_arrive_time + bus_stop_time)
                event_schedule(bus_arrive_time + bus_stop_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            else
                event_schedule(sim_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
        }
    }
    else if (event_type == EVENT_LOAD_AIR_TERMINAL_2)
    {
        // Remove a person from the queue at air terminal 2 and put it in the bus, direction to car rental
        list_remove(FIRST, LIST_AIR_TERMINAL_2);
        person_arrive_time = transfer[1];
        // answer b
        sampst(sim_time - person_arrive_time, SAMPST_DELAY_AIR_TERMINAL_2);
        transfer[3] = LOCATION_AIR_TERMINAL_2;
        list_file(LAST, LIST_BUS_TO_CAR_RENTAL);
        printf("[%7.2lf] Passenger loaded to bus, direction to car rental.\n", sim_time);
        printf("          Number of people waiting at air terminal 2: %d\n", list_size[LIST_AIR_TERMINAL_2]);
        printf("          Number of people in the bus: %d\n", list_size[LIST_BUS_TO_CAR_RENTAL]);
        // If there are still passengers in the queue and there is still capacity, schedule next loading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_AIR_TERMINAL_2] > 0 && list_size[LIST_BUS_TO_CAR_RENTAL] < bus_capacity)
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_AIR_TERMINAL_2);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            }
        }
        // If there is no more capacity
        else if (list_size[LIST_BUS_TO_CAR_RENTAL] == bus_capacity)
        {
            event_cancel(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            if (sim_time < bus_arrive_time + bus_stop_time)
                event_schedule(bus_arrive_time + bus_stop_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            else
                event_schedule(sim_time, EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
        }
    }
    else if (event_type == EVENT_LOAD_CAR_RENTAL)
    {
        // Remove a person from the queue at car rental and put it in the bus, direction to air terminal 1 and 2
        list_remove(FIRST, LIST_CAR_RENTAL);
        person_arrive_time = transfer[1];
        // answer b
        sampst(sim_time - person_arrive_time, SAMPST_DELAY_CAR_RENTAL);
        if (transfer[3] == DESTINATION_AIR_TERMINAL_1)
        {
            transfer[3] = LOCATION_CAR_RENTAL;
            list_file(LAST, LIST_BUS_TO_AIR_TERMINAL_1);
            printf("[%7.2lf] Passenger loaded to bus, direction to air terminal 1.\n", sim_time);
        }
        else
        {
            transfer[3] = LOCATION_CAR_RENTAL;
            list_file(LAST, LIST_BUS_TO_AIR_TERMINAL_2);
            printf("[%7.2lf] Passenger loaded to bus, direction to air terminal 2.\n", sim_time);
        }
        printf("          Number of people waiting at car rental: %d\n", list_size[LIST_CAR_RENTAL]);
        printf("          Number of people in the bus: %d\n", list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2]);
        // If there are still passengers in the queue and there is still capacity, schedule next loading event (and reschedule departure if delay > 5 minutes)
        if (list_size[LIST_CAR_RENTAL] > 0 && list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] < bus_capacity)
        {
            double load_time = uniform(uniform_load_time_range[0], uniform_load_time_range[1], STREAM_LOADING);
            event_schedule(sim_time + load_time, EVENT_LOAD_CAR_RENTAL);
            if (sim_time + load_time > bus_arrive_time + bus_stop_time)
            {
                event_cancel(EVENT_BUS_DEPARTURE_CAR_RENTAL);
                event_schedule(sim_time + load_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
            }
        }
        // If there is no more capacity
        else if (list_size[LIST_BUS_TO_AIR_TERMINAL_1] + list_size[LIST_BUS_TO_AIR_TERMINAL_2] == bus_capacity)
        {
            event_cancel(EVENT_BUS_DEPARTURE_CAR_RENTAL);
            if (sim_time < bus_arrive_time + bus_stop_time)
                event_schedule(bus_arrive_time + bus_stop_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
            else
                event_schedule(sim_time, EVENT_BUS_DEPARTURE_CAR_RENTAL);
        }
    }
}

// print report
void print_report(char *file_name)
{
    outfile = fopen(file_name, "w");
    fprintf(outfile, "num_location: %d\n", num_location);
    fprintf(outfile, "num_air_terminal: %d\n", num_air_terminal);
    fprintf(outfile, "simulation_length: %d\n", simulation_length);
    fprintf(outfile, "bus_speed: %.3lf\n", bus_speed);
    fprintf(outfile, "bus_capacity: %d\n", bus_capacity);
    fprintf(outfile, "bus_stop_time: %.3lf\n", bus_stop_time);
    for (int i = 0; i < num_location; i++)
    {
        fprintf(outfile, "expon_interarrival_rate[%d]: %.3lf\n", i, expon_interarrival_rate[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        fprintf(outfile, "destination_probability[%d]: %.3lf\n", i, destination_probability[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        fprintf(outfile, "uniform_unload_time_range[%d]: %.3lf\n", i, uniform_unload_time_range[i]);
    }
    for (int i = 0; i < 2; i++)
    {
        fprintf(outfile, "uniform_load_time_range[%d]: %.3lf\n", i, uniform_load_time_range[i]);
    }
    for (int i = 0; i < num_location; i++)
    {
        fprintf(outfile, "distance[%d]: %.3lf\n", i, distance[i]);
    }
    fprintf(outfile, "\n\n------------Statistics Report in Minutes------------\n\n");
    // Answer a
    fprintf(outfile, "a. Average and maximum number in each queue\n");
    for (int i = TIMEST_QUEUE_AIR_TERMINAL_1; i <= TIMEST_QUEUE_CAR_RENTAL; i++)
    {
        timest(0.0, -i);
        if (i == TIMEST_QUEUE_CAR_RENTAL)
        {
            fprintf(outfile, "  Car Rental:\n");
        }
        else
        {
            fprintf(outfile, "  Air Terminal %d:\n", i);
        }
        fprintf(outfile, "    Average: %.3lf\n", transfer[1]);
        fprintf(outfile, "    Maximum: %.3lf\n", transfer[2]);
    }

    // Answer b
    fprintf(outfile, "\nb. Average and maximum delay in each queue\n");
    for (int i = SAMPST_DELAY_AIR_TERMINAL_1; i <= SAMPST_DELAY_CAR_RENTAL; i++)
    {
        sampst(0.0, -i);
        if (i == SAMPST_DELAY_CAR_RENTAL)
        {
            fprintf(outfile, "  Car Rental:\n");
        }
        else
        {
            fprintf(outfile, "  Air Terminal %d:\n", i);
        }
        fprintf(outfile, "    Average: %.3lf\n", transfer[1]);
        fprintf(outfile, "    Maximum: %.3lf\n", transfer[3]);
    }

    // Answer c
    fprintf(outfile, "\nc. Average and maximum number on the bus\n");
    timest(0.0, -TIMEST_BUS);
    fprintf(outfile, "  Average: %.3lf\n", transfer[1]);
    fprintf(outfile, "  Maximum: %.3lf\n", transfer[2]);

    // Answer d
    fprintf(outfile, "\nd. Average, maximum, and minimum time the bus is stopped at each location\n");
    for (int i = SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_1; i <= SAMPST_DELAY_BUS_STOP_CAR_RENTAL; i++)
    {
        sampst(0.0, -i);
        if (i == SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_1)
        {
            fprintf(outfile, "  Air Terminal 1:\n");
        }
        else if (i == SAMPST_DELAY_BUS_STOP_AIR_TERMINAL_2)
        {
            fprintf(outfile, "  Air Terminal 2:\n");
        }
        else if (i == SAMPST_DELAY_BUS_STOP_CAR_RENTAL)
        {
            fprintf(outfile, "  Car Rental:\n");
        }
        fprintf(outfile, "    Average: %.3lf\n", transfer[1]);
        fprintf(outfile, "    Maximum: %.3lf\n", transfer[3]);
        fprintf(outfile, "    Minimum: %.3lf\n", transfer[4]);
    }

    // Answer e
    fprintf(outfile, "\ne. Average, maximum, and minimum time for the bus to make a loop \n");
    sampst(0.0, -SAMPST_DELAY_BUS_LOOP);
    fprintf(outfile, "  Average: %.3lf\n", transfer[1]);
    fprintf(outfile, "  Maximum: %.3lf\n", transfer[3]);
    fprintf(outfile, "  Minimum: %.3lf\n", transfer[4]);

    // Answer f
    fprintf(outfile, "\nf. Average, maximum, and minimum time a person spends in the system\n");
    for (int i = SAMPST_DELAY_PEOPLE_AIR_TERMINAL_1; i <= SAMPST_DELAY_PEOPLE_CAR_RENTAL; i++)
    {
        sampst(0.0, -i);
        if (i == SAMPST_DELAY_PEOPLE_AIR_TERMINAL_1)
        {
            fprintf(outfile, "  Air Terminal 1:\n");
        }
        else if (i == SAMPST_DELAY_PEOPLE_AIR_TERMINAL_2)
        {
            fprintf(outfile, "  Air Terminal 2:\n");
        }
        else if (i == SAMPST_DELAY_PEOPLE_CAR_RENTAL)
        {
            fprintf(outfile, "  Car Rental:\n");
        }
        fprintf(outfile, "    Average: %.3lf\n", transfer[1]);
        fprintf(outfile, "    Maximum: %.3lf\n", transfer[3]);
        fprintf(outfile, "    Minimum: %.3lf\n", transfer[4]);
    }

    fclose(outfile);
}

// Main program
int main()
{
    parse_input("car_rental.in");

    // Init model and initialize first events
    init_model();

    printf("- Starting simulation...\n");

    // Invoke timing and determine action based on next event type
    while (next_event_type != EVENT_END_SIMULATION)
    {
        // printf("Next event type: %d\n", next_event_type);
        timing();
        switch (next_event_type)
        {
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
        case EVENT_BUS_DEPARTURE_AIR_TERMINAL_1:
            bus_departure(EVENT_BUS_DEPARTURE_AIR_TERMINAL_1);
            break;
        case EVENT_BUS_DEPARTURE_AIR_TERMINAL_2:
            bus_departure(EVENT_BUS_DEPARTURE_AIR_TERMINAL_2);
            break;
        case EVENT_BUS_DEPARTURE_CAR_RENTAL:
            bus_departure(EVENT_BUS_DEPARTURE_CAR_RENTAL);
            break;
        case EVENT_LOAD_AIR_TERMINAL_1:
            load(EVENT_LOAD_AIR_TERMINAL_1);
            break;
        case EVENT_LOAD_AIR_TERMINAL_2:
            load(EVENT_LOAD_AIR_TERMINAL_2);
            break;
        case EVENT_LOAD_CAR_RENTAL:
            load(EVENT_LOAD_CAR_RENTAL);
            break;
        case EVENT_UNLOAD_AIR_TERMINAL_1:
            unload(EVENT_UNLOAD_AIR_TERMINAL_1);
            break;
        case EVENT_UNLOAD_AIR_TERMINAL_2:
            unload(EVENT_UNLOAD_AIR_TERMINAL_2);
            break;
        case EVENT_UNLOAD_CAR_RENTAL:
            unload(EVENT_UNLOAD_CAR_RENTAL);
            break;
        }
    }

    // Event end simulation
    printf("[%7.2lf] Simulation ended.\n", sim_time);
    print_report("car_rental.out");

    return 0;
}