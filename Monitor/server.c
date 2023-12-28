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
struct calculated_statistics final_stats = {0};    // Struct that will hold the final stats to present to the user


/*
   Function: server_socket_create
   Purpose:  Creates a server socket and sets it up for listening on a specified port.
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
    Function: calculate_avg_time
    Purpose:  Calculates the average time, converts it to simulated time, and returns the result.
    Parameters:
      - time_seconds: Time in seconds.
      - time_microseconds: Time in microseconds.
      - total_number_people: Total number of people.
    Returns: Average time in simulated time (seconds).

    How it works: Checks if any people used the atraction if not returns 0. If someone used the
    atraction, converst the microseconds into seconds and then the total amount of time people kept
    waiting for the number of people that used the atraction. 
    Finally multiplies by the timescale of how much each second in real life is worth to get the 
    wait time on the simulated world
*/
double calculate_avg_time(int time_seconds, int time_microseconds, int total_number_people){
    if(total_number_people > 0){
        double total_time = time_seconds + time_microseconds / 1.0e6;
        return ((total_time/total_number_people) * stats_time.timeScale);
    }
    
    return 0;
}



/*
    Function: convert_personalized_time
    Purpose:  Converts a given time in seconds to a structure representing hours, minutes, and seconds.
    Parameters:
        - time: The time in seconds to be converted.
        - pers_time: Pointer to a struct personalized_time where the result will be stored.
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
    Function: calculate_final_statistics
    Purpose:  Calculates various average times based on accumulated statistics when the simulator ends.

    How it works: Create a temporary variable that will hold the return of the calculate_avg_time()
    function, and pass that value to the convert_personallized_time() where it will convert to that
    time format and put it on the final_stats structure
*/
void calculate_final_statistics()
{
    double temp_seconds = 0; // Temp variable

    // Calculate average time online to use the park (doesnt count the ones that couldnt use the park)
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_park_seconds, 
    stats_time.sum_time_on_line_park_microseconds, 
    stats_num.used_park_today);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_park);
    

    // Calculate average time inside the park
    temp_seconds = calculate_avg_time(stats_time.sum_time_inside_park_seconds, 
    stats_time.sum_time_inside_park_microseconds, 
    stats_num.used_park_today);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_inside_park);

    // Calculate average time waiting fot the toboggan
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_toboggan_seconds, 
    stats_time.sum_time_on_line_toboggan_microseconds, 
    stats_num.used_Toboggan);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_toboggan);

    // Calculate the average time waiting for the snackbar
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_snack_seconds, 
    stats_time.sum_time_on_line_snack_microseconds, 
    stats_num.used_Snack_bar);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_snack);
    

    // Calculate the average time children waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_child_famwaterslide_seconds, 
    stats_time.sum_time_child_famwaterslide_microseconds, 
    stats_num.children_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_child_famWaterslide);
    

    // Calculate the average time elders waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_elder_famwaterslide_seconds, 
    stats_time.sum_time_elder_famwaterslide_microseconds, 
    stats_num.elders_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_elder_famWaterslide);

    // Calculate the average time adults waited for the family waterslide
    temp_seconds = calculate_avg_time(stats_time.sum_time_adult_famwaterslide_seconds, 
    stats_time.sum_time_adult_famwaterslide_microseconds, 
    stats_num.adults_used_Familywaterslide);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_adult_famWaterslide);

    // Calculate the average time waiting for the waterpolo
    temp_seconds = calculate_avg_time(stats_time.sum_time_on_line_waterpolo_seconds, 
    stats_time.sum_time_on_line_waterpolo_microseconds, 
    stats_num.played_waterpolo);
    convert_personalized_time(temp_seconds, &final_stats.avg_time_on_line_waterpolo);

    temp_seconds = calculate_avg_time(stats_time.sum_time_to_signature_book_seconds, 
    stats_time.sum_time_to_signature_book_microseconds, 
    (stats_num.signed_book + stats_num.read_book)); //Sum of the people who read and wrote
    convert_personalized_time(temp_seconds, &final_stats.avg_time_for_signature_book);
    
    
}



/*
    Function: decode_number_people
    Purpose:  Handles information related to the number of people who did something based on the received code
    Parameters:
      - code: An integer array containing the code information.

    How it works: This function is called when code[0] is 0, then uses code[1] for a switch
    where each number representas a different atraction, or the park itself.
    Code[2] is used to represent something the person did while on that atraciton;
    Code[3] is used for the person ID
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

        case 4:
            if(code[2] == 0)
            {
                printf("A person (%d) played waterpolo                                                \n", code[3]); // Code 3 refers to the person ID
                stats_num.played_waterpolo += 1;
            }
            else
            {
                printf("A person (%d) couldnt find a teammate for waterpolo                           \n", code[3]);
                stats_num.not_played_waterpolo += 1;
            }
            break;

        case 5:
            if(code[2] == 0)
            {
                printf("A person (%d) signed the signature book                                       \n", code[3]); // Code 3 refers to the person ID
                stats_num.signed_book += 1;
            }
            else
            {
                printf("A person (%d) read the signature book                                         \n", code[3]);
                stats_num.read_book += 1;
            }  
            break;          

        default:
            break;
        }
}



/*
    Function: decode_time_people
    Purpose:  Handles information related to the time people spent doing something based on the receive code
    Parameters:
      - code: An integer array containing the code information.

    How it works: This function is called when code[0] is 1, then uses code[1] for a switch
    where each number representas a different lines, atractions or priority lines;
    Code[2] is used to represent if the information is in seconds or microseconds;
    Code[3] is used for the value of the time that is being passed;
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
            
            case 8:
                if(code[2] == 0) // Time a person was on line waterpolo
                {
                    stats_time.sum_time_on_line_waterpolo_seconds+= code[3];
                } 
                if(code[2] == 1) // Time a person was on line waterpolo
                {
                    stats_time.sum_time_on_line_waterpolo_microseconds += code[3];
                }
                break;

            case 9:
                if(code[2] == 0) // Time person waited for the signature book
                {
                    stats_time.sum_time_to_signature_book_seconds+= code[3];
                } 
                if(code[2] == 1) // Time person waited for the signature book
                {
                    stats_time.sum_time_to_signature_book_microseconds += code[3];
                }
                break;

            default:
                break;
        }
}



/*
    Function: decode_live_stats
    Purpose:  Handles information that is to be presented live to the user about the simulator
    Parameters:
      - code: An integer array containing the code information.

    How it works: This function is called when code[0] is 2, then uses code[1] for a switch
    where each number representas a different atraction, or the park itself.
    Code[2] is used to represent if the person entered an atracion, exited, entered the line, etc;
    Code[3] is used for the person ID
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

        case 4:
            if(code[2] == 0)
            {
                printf("A person (%d) is going to the Waterpolo section                               \n", code[3]); // Code 3 refers to the person ID
                live_stats.waterpolo += 1;
            } else { 
                printf("A person (%d) is exiting the Waterpolo section                                \n", code[3]);
                live_stats.waterpolo -= 1;
            }
            break;

        case 5:
            if(code[2] == 0)
            {
                printf("A person (%d) is going to the signature book section                          \n", code[3]); // Code 3 refers to the person ID
                live_stats.signature_book += 1;
            } else { 
                printf("A person (%d) is exiting the signature book section                           \n", code[3]);
                live_stats.signature_book -= 1;
            }
            break;

        default:
            break;
        }
}



/*
    Function: print_live_stats
    Purpose:  Prints live statistics of the park.
*/
void print_live_stats()
{
    // Prints the live statistics of park
    printf("                                                                            \n");
    printf("Live numbers of the park:                                                   \n");
    printf("In line to enter the park:..........%d                                      \n", live_stats.inline_park);
    printf("Inside the park:....................%d                                      \n", live_stats.inside_park);
    printf("In the Family waterslide:...........%d                                      \n", live_stats.family_waterslide);
    printf("In Toboggan:........................%d                                      \n", live_stats.toboggan);
    printf("In the Snackbar:....................%d                                      \n", live_stats.Snackbar);
    printf("In the Waterpolo:...................%d                                      \n", live_stats.waterpolo);
    printf("In the Signature book room:.........%d                                      \n", live_stats.signature_book);
    printf("\033[9A\033[0G\033[?25l"); // This print puts the cursor 8 lines up and on the first caracter of the line

    // Flush the output to ensure it's displayed immediately
    fflush(stdout);
}



/*
    Function: print_final_stats()
    Purpose:  Prints lthe final statistics of the park to show the user
*/
void print_final_stats()
{
    printf("Measured in number of people                                                 \n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Park: (Entered | Didn't Enter):...............................(%d | %d)      \n", 
    stats_num.used_park_today, stats_num.park_closed_before_entry);
    printf("Used the Family Waterslide: (Children | Adults | Elders):.....(%d | %d | %d) \n", 
    stats_num.children_used_Familywaterslide, stats_num.adults_used_Familywaterslide, stats_num.elders_used_Familywaterslide);
    printf("Toboggan (Used | Quit on Line):...............................(%d | %d)      \n",
    stats_num.used_Toboggan, stats_num.quit_Toboggan);
    printf("Snackbar (Used | Quit on Line):...............................(%d | %d)      \n",
    stats_num.used_Snack_bar, stats_num.quit_Snack_bar);
    printf("Waterpolo (Played | Didnt Play):..............................(%d | %d)      \n",
    stats_num.played_waterpolo, stats_num.not_played_waterpolo);
    printf("Signature book (Write | Read):................................(%d | %d)      \n",
    stats_num.signed_book, stats_num.read_book);
    printf("\n");

    // Print Statistics related to time
    printf("Wait times on the park:                                                        \n");
    printf("-------------------------------------------------------------------------------\n");
    printf("Average time on line for the park:............................%d(h) %d(m) %d(s)\n",
    final_stats.avg_time_on_line_park.hours, final_stats.avg_time_on_line_park.minutes, 
    final_stats.avg_time_on_line_park.seconds);
    printf("Average time inside the park:.................................%d(h) %d(m) %d(s)\n",
    final_stats.avg_time_inside_park.hours, final_stats.avg_time_inside_park.minutes, 
    final_stats.avg_time_inside_park.seconds);
    printf("Average time waiting for the toboggan:........................%d(h) %d(m) %d(s)\n",
    final_stats.avg_time_on_line_toboggan.hours, final_stats.avg_time_on_line_toboggan.minutes, 
    final_stats.avg_time_on_line_toboggan.seconds);
    printf("Average time waiting for the Snackbar:........................%d(h) %d(m) %d(s)\n",
    final_stats.avg_time_on_line_snack.hours, final_stats.avg_time_on_line_snack.minutes, 
    final_stats.avg_time_on_line_snack.seconds);
    printf("Average time children waited for the family waterslide:.......%d(h) %d(m) %d(s)\n", 
    final_stats.avg_time_child_famWaterslide.hours, final_stats.avg_time_child_famWaterslide.minutes, 
    final_stats.avg_time_child_famWaterslide.seconds);
    printf("Average time elders waited for the family waterslide:.........%d(h) %d(m) %d(s)\n", 
    final_stats.avg_time_elder_famWaterslide.hours, final_stats.avg_time_elder_famWaterslide.minutes, 
    final_stats.avg_time_elder_famWaterslide.seconds);
    printf("Average time adults waited for the family waterslide:.........%d(h) %d(m) %d(s)\n", 
    final_stats.avg_time_adult_famWaterslide.hours, 
    final_stats.avg_time_adult_famWaterslide.minutes, final_stats.avg_time_adult_famWaterslide.seconds);
    printf("Average time waiting to play waterpolo:.......................%d(h) %d(m) %d(s)\n", 
    final_stats.avg_time_on_line_waterpolo.hours, 
    final_stats.avg_time_on_line_waterpolo.minutes, final_stats.avg_time_on_line_waterpolo.seconds);
    printf("Average time waiting for the signature book:..................%d(h) %d(m) %d(s)\n", 
    final_stats.avg_time_for_signature_book.hours, 
    final_stats.avg_time_for_signature_book.minutes, final_stats.avg_time_for_signature_book.seconds);

    // Flush the output to ensure it's displayed immediately
    fflush(stdout);   
}



/*
    Function: decode_message
    Purpose:  Decodes received messages based on the provided code and performs corresponding actions.
    Parameters:
        - code: An array of integers representing the message code and additional information.
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
        print_final_stats();
    } 
    else 
    {
        print_live_stats();
    }
}



/*
    Function: check_client_disconnect
    Purpose:  Checks if the client has disconnected or if an error occurred by attempting to receive a message.
              If the client has disconnected or an error occurred, it prints a message and exits the loop.
    Returns:
        - 0 if the client has disconnected or an error occurred.
        - 1 if the message was successfully received and decoded.
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
    Function: connect_client
    Purpose:  Accepts a connection from a client.
    Returns:
        - 1 if the connection is successful.
        - (-1) if there is a problem with the connection.
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




