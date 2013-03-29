// EE122 Project 2 - util.c
// Xiaodian (Yinyin) Wang and Arnab Mukherji
//
// util.c contains all of the common functions that are used by the senders, router and receivers.

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

//Get the socket address, IPv6 or IPv6 (taken from Beej's guide)
/*If the sa_family field is AF_INET (IPv4), return the IPv4 address. Otherwise return the IPv6 address.*/
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Enqueue packets to linked list
int enqueue (struct q_elem *elem, struct router_q *q, unsigned int max_q_size) {
    if (q->q_size >= max_q_size) {//If queue size is at max, drop incoming packets
        q->drop_cnt++;
        return 1; 
    }
    
    if (q->head == NULL) {
        q->head = elem;
    } else {
        q->tail->next = elem;
    }
    q->tail = elem;
    q->q_size++;
    elem->next = NULL;
    return 0; 
}

//Dequeue packets from linked list
struct q_elem *dequeue (struct router_q *q) {
    struct q_elem *elem = NULL;
    
    if (q->head == NULL) {
        return NULL;
    } else {
        elem = q->head;
        q->head = q->head->next;
        q->q_size--;
    }
    if (q->head == NULL) {
        q->tail = NULL;
        q->q_size = 0;
    }
    return elem; 
}

//Packet delay time, generates a time delay according to a poisson distribution
void poisson_delay(unsigned int mean) {
    
}

//Function to retreive the port for a given receiver ID
char port_str[32];
char *get_receiver_port(unsigned int receiver_id) {
    unsigned int port;
    port = (RECEIVER_PORT_BASE + (receiver_id - 1));
    sprintf(port_str, "%d", port);
    return port_str; 
}