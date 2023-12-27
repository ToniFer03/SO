#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"


// Global Variables
int server_socket;                                 // Declare server_socket at the file scope
int client_socket;                                 // Declare client_socket at the file scope
struct sockaddr_in server_addr, client_addr;       // Store information about sockets
socklen_t client_addr_len;                         // Store the size of the client address
struct statistics_time_simulator stats_time = {0}; // Struct that will hold info from the simulator
struct statistics_number_people stats_num = {0};   // Struct that will hold info from the simulator
struct statistics_live live_stats = {0};           // Struct that will hold statistics of live park
struct calculated_statistics final_stats = {0};         // Struct that will hold the final stats to present to the user


/*
    This function is responsible for creating a socket server to be able to communicate with
    other processes
*/
void server_socket_create()
{
    // Creates a new socket with an IPv4 address
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed"); // If it's -1, socket creation failed
        exit(1);
    }

    server_addr.sin_family = AF_INET;                     // It's an IPv4 address
    server_addr.sin_port = htons(PORT);                   // Define the port you want to use
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Set the IP address to the loopback address

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 1) == 0)
    {
        printf("Server created successfully and waiting for a connection...\n");
    }
    else
    {
        perror("Listen failed");
        exit(1);
    }
}


/*
    Funtion responsible for calculating the average time, and converting it to the simulated time
    in seconds.
*/
double calculate_avg_time(int time_seconds, int time_microseconds, int total_number_people){
    double total_time = time_seconds + time_microseconds / 1.0e6;
    return ((total_time/total_number_people) * stats_time.timeScale);
}


/*
    Function that receives a value of seconds and turns it into the personalized time
    struct to present to the user, it receives the total time in seconds and the adress
    of the struct to put the info into
*/
void convert_personalized_time(double time, struct personalized_time * pers_time)
{
    //Calculate hours
    pers_time->hours = (int)(time / 3600);

    // Calculate remaining minutes
    double remaining_minutes = time - pers_time->hours * 3600;
    pers_time->minutes = (int)(remaining_minutes / 60);

    // Calculate remaining seconds
    pers_time->seconds = (int)(remaining_minutes - pers_time->minutes * 60);
}


/*
    Function that will calculate all the averages when the simulator ends
*/
void calculate_final_statistics()
{
    double temp_seconds = 0; // Temp variable

    // Calculate average time online to use the park (doesnt count the ones that couldnt use the park)
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_park_seconds, stats_time.sum_time_on_line_park_microseconds, stats_num.used_park_today);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_park);

    // Calculate average time inside the park
    temp_seconds = calculate_avg_time(stats_time.sum_time_inside_park_seconds, stats_time.sum_time_inside_park_microseconds, stats_num.used_park_today);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_inside_park);

    // Calculate average time waiting fot the toboggan
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_toboggan_seconds, stats_time.sum_time_on_line_toboggan_microseconds, stats_num.used_Toboggan);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_toboggan);

    // Calculate the average time waiting for the snackbar
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_snack_seconds, stats_time.sum_time_on_line_snack_microseconds, stats_num.used_Snack_bar);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_snack);

    // Calculate the average time children waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_child_famwaterslide_seconds, stats_time.sum_time_child_famwaterslide_microseconds, stats_num.children_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_child_famWaterslide);

    // Calculate the average time elders waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_elder_famwaterslide_seconds, stats_time.sum_time_elder_famwaterslide_microseconds, stats_num.elders_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_elder_famWaterslide);

    // Calculate the average time adults waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_adult_famwaterslide_seconds, stats_time.sum_time_adult_famwaterslide_microseconds, stats_num.adults_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_adult_famWaterslide);
    
}


/*
    Decode message will call this function too handle info related to the number of people who did something
    Code[0] == 0
*/
void decode_number_people(int code[4])
{
    switch (code[1]) // Switch to differenciate what atraction the info relates
        {
        case 0:
            if (code[2] == 0) // Person used the park
            { 
                stats_num.used_park_today += 1;
            } 
            else // Person couldnt enter in time
            {
                stats_num.park_closed_before_entry += 1;
            }
            break;
        
        case 1:
            if(code[2] == 0) 
            {
                printf("A person (%d) used the Toboggan                                               \n", code[3]); // Code 3 refers to the person ID
                stats_num.used_Toboggan += 1;
            } else { 
                printf("A person (%d) waited too much time and quit using the Toboggan                \n", code[3]);
                stats_num.quit_Toboggan += 1;
            }
            break;
        
        case 2:
            if(code[2] == 0)
            {
                printf("A child (%d) used the Family Waterslide                                      \n", code[3]); // Code 3 refers to the person ID
                stats_num.children_used_Familywaterslide += 1;
            } 
            else if (code[2] == 1)
            {
                printf("An elder (%d) used the Family Waterslide                                     \n", code[3]); // Code 3 refers to the person ID
                stats_num.elders_used_Familywaterslide += 1;
            }
            else 
            {
                printf("An adult (%d) used the Family Waterslide                                     \n", code[3]); // Code 3 refers to the person ID
                stats_num.adults_used_Familywaterslide += 1;
            }
            break;
        
        case 3:
            if(code[2] == 0)
            {
                printf("A person (%d) used the Snackbar                                               \n", code[3]); // Code 3 refers to the person ID
                stats_num.used_Snack_bar += 1;
            } else { 
                printf("A person (%d) waited too much time and quit using the Snackbar                \n", code[3]);
                stats_num.quit_Snack_bar += 1;
            }
            break;

        default:
            break;
        }
}


/*
    Decode message will call this funtion to handle info related to the time statistics of people
    Code[0] == 1
*/
void decode_time_people(int code[4])
{
    switch(code[1]) // Switch to differenciate what atraction the info relates
        {
            case 0:
                if(code[2] == 0) // Time person was on line to the park (seconds)
                {
                    stats_time.sum_time_on_line_park_seconds += code[3];
                } 
                if(code[2] == 1) // Time person was on line to the park (microseconds)
                {
                    stats_time.sum_time_on_line_park_microseconds += code[3];
                }
                break;   

            case 1:
                if(code[2] == 0) // Time person used the park (seconds)
                {
                    stats_time.sum_time_inside_park_seconds += code[3];
                } 
                if(code[2] == 1) // Time person used the park (microseconds)
                {
                    stats_time.sum_time_inside_park_microseconds += code[3];
                }
                break;   

            case 2:
                if(code[2] == 0) // Time person was on line toboggan (seconds)
                {
                    stats_time.sum_time_on_line_toboggan_seconds += code[3];
                } 
                if(code[2] == 1) // Time person was on line toboggan (microseconds)
                {
                    stats_time.sum_time_on_line_toboggan_microseconds += code[3];
                }
                break; 

            case 4:
                if(code[2] == 0) // Time person was on line snackbar (seconds)
                {
                    stats_time.sum_time_on_line_snack_seconds += code[3];
                } 
                if(code[2] == 1) // Time person was on line snackbar (microseconds)
                {
                    stats_time.sum_time_on_line_snack_microseconds += code[3];
                }
                break; 

            case 5: // Child
                if(code[2] == 0) // Time child was on line family waterslide (seconds)
                {
                    stats_time.sum_time_child_famwaterslide_seconds += code[3];
                } 
                if(code[2] == 1) // Time child was on line family waterslide (microseconds)
                {
                    stats_time.sum_time_child_famwaterslide_microseconds += code[3];
                }
                break; 

            case 6: // Elder
                if(code[2] == 0) // Time elder was on line family waterslide (seconds)
                {
                    stats_time.sum_time_elder_famwaterslide_seconds += code[3];
                } 
                if(code[2] == 1) // Time elder was on line family waterslide (microseconds)
                {
                    stats_time.sum_time_elder_famwaterslide_microseconds += code[3];
                }
                break; 

            case 7: // Adult
                if(code[2] == 0) // Time adult was on line family waterslide (seconds)
                {
                    stats_time.sum_time_adult_famwaterslide_seconds += code[3];
                } 
                if(code[2] == 1) // Time adult was on line family waterslide (microseconds)
                {
                    stats_time.sum_time_adult_famwaterslide_microseconds += code[3];
                }
                break; 

            default:
                break;
        }
}



/*
    Decode message will call this funtion to handle live statistics of the park and print what info it receives
    Code[0] == 2
*/
void decode_live_stats(int code[4])
{
    switch (code[1]) // Switch to differenciate what atraction the info relates
        {
        case 0:
            if(code[2] == 0) 
            {
                printf("A person (%d) is on line for the park                                         \n", code[3]); // Code 3 refers to the person ID
                live_stats.inline_park += 1;
            } else if (code[2] == 1) { 
                printf("A person (%d) entered the park                                                \n", code[3]);
                live_stats.inline_park -= 1;
                live_stats.inside_park += 1;
            } else if (code[2] == 2){ 
                printf("A person (%d) exited the park                                                 \n", code[3]);
                live_stats.inside_park -= 1;
            } else {
                printf("A person (%d) tried to enter, but the park was closed                         \n", code[3]);
                live_stats.inline_park -= 1;
            }
            break;
        
        case 1:
            if(code[2] == 0) 
            {
                printf("A person (%d) is going to the toboggan section                                \n", code[3]); // Code 3 refers to the person ID
                live_stats.toboggan += 1;
            } else { 
                printf("A person (%d) is exiting the toboggan section                                 \n", code[3]);
                live_stats.toboggan -= 1;
            }
            break;
        
        case 2:
            if(code[2] == 0)
            {
                printf("A person (%d) is going to the Family Waterslide section                       \n", code[3]); // Code 3 refers to the person ID
                live_stats.family_waterslide += 1;
            }
            else 
            {
                printf("A person (%d) is exiting the Family Waterslide section                        \n", code[3]); // Code 3 refers to the person ID
                live_stats.family_waterslide -= 1;

            }
            break;
        
        case 3:
            if(code[2] == 0)
            {
                printf("A person (%d) is going to the Snackbar section                                \n", code[3]); // Code 3 refers to the person ID
                live_stats.Snackbar += 1;
            } else { 
                printf("A person (%d) is exiting the Snackbar section                                 \n", code[3]);
                live_stats.Snackbar -= 1;
            }
            break;

        default:
            break;
        }
}


/*
    This function is responsible for decoding the messages that it receives, receives an argument of type int
    and checks the code in a switch to check what kind of action need to be taken.
*/
void decode_message(int code[4])
{
    if(code[0] == 0) // Related to number of people who used something
    {
        decode_number_people(code);
    }

    if(code[0] == 1) // Related to the time
    { 
        decode_time_people(code);
    }

    if(code[0] == 2) // Related to the live stats of the park
    {
        decode_live_stats(code);
    }

    if(code[0] == 100) // Receive the information on the timescale being used
    { 
        stats_time.timeScale = code[1];
    }

    if(code[0] == 999) // The simulator has ended, calculate and print the final statistics
    { 
        printf("\033[999;999H"); // Put the cursor at the end of the terminal
        printf("\n");

        // Calculate final statistics
        calculate_final_statistics();

        printf("Measured in number of people                                                \n");
        printf("----------------------------------------------------------------------------\n");
        printf("Park: (Entered | Didn't Enter):...............................(%d | %d)     \n", 
        stats_num.used_park_today, stats_num.park_closed_before_entry);
        printf("Used the Family Waterslide: (Children | Adults | Elders):.....(%d | %d | %d)\n", 
        stats_num.children_used_Familywaterslide, stats_num.adults_used_Familywaterslide, stats_num.elders_used_Familywaterslide);
        printf("Toboggan (Used | Quit on Line):...............................(%d | %d)     \n",
        stats_num.used_Toboggan, stats_num.quit_Toboggan);
        printf("Snackbar (Used | Quit on Line):...............................(%d | %d)     \n",
        stats_num.used_Snack_bar, stats_num.quit_Snack_bar);
        printf("\n");

        // Print final statistics to show the user
        printf("Information about this day at the park:                                     \n");
        printf("----------------------------------------------------------------------------\n");
        printf("Average time on line for the park: %d(h) %d(m) %d(s)                        \n",
        final_stats.avg_time_on_line_park.hours, final_stats.avg_time_on_line_park.minutes, 
        final_stats.avg_time_on_line_park.seconds);
        printf("Average time inside the park: %d(h) %d(m) %d(s)                             \n",
        final_stats.avg_time_inside_park.hours, final_stats.avg_time_inside_park.minutes, 
        final_stats.avg_time_inside_park.seconds);
        printf("Average time waiting for the toboggan: %d(h) %d(m) %d(s)                    \n",
        final_stats.avg_time_on_line_toboggan.hours, final_stats.avg_time_on_line_toboggan.minutes, 
        final_stats.avg_time_on_line_toboggan.seconds);
        printf("Average time waiting for the Snackbar: %d(h) %d(m) %d(s)                    \n",
        final_stats.avg_time_on_line_snack.hours, final_stats.avg_time_on_line_snack.minutes, 
        final_stats.avg_time_on_line_snack.seconds);
        printf("Average time children waited for the family waterslide: %d(h) %d(m) %d(s)   \n", 
        final_stats.avg_time_child_famWaterslide.hours, final_stats.avg_time_child_famWaterslide.minutes, 
        final_stats.avg_time_child_famWaterslide.seconds);
        printf("Average time elders waited for the family waterslide: %d(h) %d(m) %d(s)     \n", 
        final_stats.avg_time_elder_famWaterslide.hours, final_stats.avg_time_elder_famWaterslide.minutes, 
        final_stats.avg_time_elder_famWaterslide.seconds);
        printf("Average time adults waited for the family waterslide: %d(h) %d(m) %d(s)     \n", 
        final_stats.avg_time_adult_famWaterslide.hours, 
        final_stats.avg_time_adult_famWaterslide.minutes, final_stats.avg_time_adult_famWaterslide.seconds);

        // Flush the output to ensure it's displayed immediately
        fflush(stdout);
    } 
    else 
    {
        // Prints the live statistics of park
        printf("                                                                            \n");
        printf("Live numbers of the park:                                                   \n");
        printf("In line to enter the park:..........%d                                      \n", live_stats.inline_park);
        printf("Inside the park:....................%d                                      \n", live_stats.inside_park);
        printf("In the Family waterslide:...........%d                                      \n", live_stats.family_waterslide);
        printf("In Toboggan:........................%d                                      \n", live_stats.toboggan);
        printf("In the Snackbar:....................%d                                      \n", live_stats.Snackbar);
        printf("\033[7A\033[0G\033[?25l"); // This print puts the cursor 7 lines up and on the first caracter of the line

        // Flush the output to ensure it's displayed immediately
        fflush(stdout);
    }
}


/*
    Function responsible for receiving messages for the other process, and check if
    the other process disconnected
*/
int check_client_disconnect()
{
    int code[4] = {};
    int bytes_received = recv(client_socket, &code, sizeof(code), 0);

    if (bytes_received <= 0)
    {
        // The client has disconnected or an error occurred, so exit the loop
        printf("\033[999;999H");
        printf("\n");
        printf("Client disconnected or an error occurred. Exiting the loop.\n");
        return 0;
    }
    else
    {
        decode_message(code);
        return 1;
    }
}


/*
    This function is responsible for connecting to a client when receiving a request to connect
*/
int connect_client()
{
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1) // Problem with the connection
    {
        return -1;
    }
    else // The connection is sucessfull
    {
        return 1;
    }
}




