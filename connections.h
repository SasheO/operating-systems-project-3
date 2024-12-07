#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct connection {
   char username1[30];
   char username2[30];
   struct connection *next;
};

 

/////////////////// USERLIST //////////////////////////

//insert connection at the first location. setting head as NULL means this just creates and returns a new connection
struct connection* createAndInsertConnection(struct connection *head, char *username1, char *username2, char* message_output);

// insert already existing link to first location
struct connection* insertConnection(struct connection *connectionlist, struct connection *link);

//find a connection with given username
struct connection* findConnection(struct connection *head, char *username1, char *username2);

//remove link with given user. this only deals with the linked list itself and not with the list header so if the connection is at the front of the list, it will have to be dealt with accordingly
struct connection* removeConnection(struct connection *head, char *username1, char *username2);

void printConnections(struct connection *head);
#endif