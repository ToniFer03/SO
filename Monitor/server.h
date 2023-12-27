#ifndef SERVER_H
#define SERVER_H

#define PORT 9370 // Defines the port that will listen for the connection

/*
    Struct that will hold the info on the statistics of the park
    Note: The ones that start with sum, are to be divided by the number of people that used the atraction/park
*/
struct statistics_time_simulator
{
    int timeScale;
    int sum_time_on_line_park_seconds; // Sum of the time people spent on line to the park
    int sum_time_on_line_park_microseconds;
    int sum_time_inside_park_seconds; // Sum of the time people used the park
    int sum_time_inside_park_microseconds;
    int sum_time_on_line_toboggan_seconds; // Sum of the time people waited on line toboggan
    int sum_time_on_line_toboggan_microseconds;
    int sum_time_on_line_snack_seconds; // Sum of the time people waited on line for the snack
    int sum_time_on_line_snack_microseconds;
    int sum_time_child_famwaterslide_seconds; // Sum of the time children waited on line for the family waterslide
    int sum_time_child_famwaterslide_microseconds;
    int sum_time_elder_famwaterslide_seconds; // Sum of the time elders waited on line for the family waterslide
    int sum_time_elder_famwaterslide_microseconds;
    int sum_time_adult_famwaterslide_seconds; // Sum of the time adults waited on line for the family waterslide
    int sum_time_adult_famwaterslide_microseconds;
    int sum_time_on_line_waterpolo_seconds; // Sum of the time people waited for waterpolo
    int sum_time_on_line_waterpolo_microseconds;

};

struct statistics_number_people
{
    int used_park_today;
    int park_closed_before_entry;
    int quit_Toboggan;
    int quit_Snack_bar;
    int used_Toboggan;
    int used_Snack_bar;
    int children_used_Familywaterslide;
    int elders_used_Familywaterslide;
    int adults_used_Familywaterslide;
    int played_waterpolo;
    int not_played_waterpolo;
};

struct statistics_live
{
    int inline_park;
    int inside_park;
    int family_waterslide;
    int toboggan;
    int Snackbar;
    int waterpolo;
};

struct personalized_time
{
    int hours;
    int minutes;
    int seconds;
};

struct calculated_statistics{
    struct personalized_time avg_time_on_line_park;
    struct personalized_time avg_time_inside_park;
    struct personalized_time avg_time_on_line_toboggan;
    struct personalized_time avg_time_on_line_snack;
    struct personalized_time avg_time_child_famWaterslide;
    struct personalized_time avg_time_elder_famWaterslide;
    struct personalized_time avg_time_adult_famWaterslide;
    struct personalized_time avg_time_on_line_waterpolo;
    

};


// Definition of functions
void server_socket_create();
int connect_client();
int check_client_disconnect();
void decode_message(int code[4]);

#endif
