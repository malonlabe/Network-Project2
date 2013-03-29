// EE122 Project 2 - sender.c
// Xiaodian (Yinyin) Wang and Arnab Mukherji
//
// sender.c is the sender sending the packets to the router

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include "common.h"

//Input Arguments:
//argv[1] is Sender ID
//argv[2] is the inter-packet time W with mean value R (based on Poisson distr). 
//argv[3] is the receiver ID
//argv[4] is the router IP
//argv[5] is the time duration in seconds (dictates how long sender will send packets to target).

int main(int argc, char *argv[]) {
    //Variables used for input arguments
    unsigned int sender_id; 
    unsigned int r; //inter-packet time W w/ mean R
    unsigned int receiver_id;
    char *dest_ip; //destination/router IP
    unsigned int duration; 
    
    //Variables used for establishing the connection
    int sockfd;
    struct addrinfo hints, *receiver_info;
    int return_val;
    
    //Variabes used for outgoing packets
    int packet_success;
    unsigned int seq = 0;
    struct msg_payload *buffer;
    struct msg_payload payload;
    struct timeval start_time;
    struct timeval curr_time;
    unsigned int delta_time;
    
    //Parsing input arguments
    if (argc != 6) {
        perror("Sender: incorrect number of command-line arguments\n");
        return 1; 
    } else {
        sender_id = atoi(argv[1]);
        r = atoi(argv[2]);
        receiver_id = atoi(argv[3]);
        dest_ip = argv[4];
        duration = atoi(argv[5]);
        printf("Sender id %d, r value %d, receiver id %1d, router IP address %s, port number %s, time duration is %d\n", sender_id, r, receiver_id, dest_ip, ROUTER_PORT, duration);
    }
    
    //load struct addrinfo with host information
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    //Get target's address information
    if ((return_val = getaddrinfo(dest_ip, ROUTER_PORT, &hints,
                          &receiver_info)) != 0) {
        perror("Sender: unable to get target's address info\n");
        return 2;
    }
    
    //Take fields from first record in receiver_info, and create socket from it.
    if ((sockfd = socket(receiver_info->ai_family,receiver_info->ai_socktype,receiver_info->ai_protocol)) == -1) {
        perror("Sender: unable to create socket\n");
        return 3;
    }
    
    //Establishing the packet: filling packet information
    memset(&payload, 0, sizeof payload);
    buffer = &payload;
    buffer->seq = seq++;
    buffer->sender_id = sender_id;
    buffer->receiver_id = receiver_id;
    buffer->timestamp = 0;
    gettimeofday(&start_time, NULL);
    
    while ((delta_time / ONE_MILLION) < duration) {
        packet_success = sendto(sockfd, buffer, sizeof(struct msg_payload), 0, receiver_info->ai_addr, receiver_info->ai_addrlen);
        poisson_delay(r);
        gettimeofday(&curr_time, NULL);
        delta_time = (curr_time.tv_sec * ONE_MILLION + curr_time.tv_usec) - (start_time.tv_sec * ONE_MILLION + start_time.tv_usec);
        buffer->timestamp = delta_time;
        buffer->seq = seq++;
        printf("Sender: Total packets sent so far: %d\n", seq);
        printf("Sender: Delta time: %d usec\n", delta_time); 
    }
    close(sockfd);
    return 0; 
}