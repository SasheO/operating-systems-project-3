#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct node {
   char username[30];
   int socket;
   struct node *next;
   int current_room_ID;
};

 

/////////////////// USERLIST //////////////////////////

//insert node at the first location. setting head as NULL means this just creates and returns a new node
struct node* createAndInsertU(struct node *head, int socket, char *username, int room);

// insert already existing link to first location
struct node* insertU(struct node *nodelist, struct node *link);

//find a node with given username
struct node* findU(struct node *head, char* username);

//remove link with given user. this only deals with the linked list itself and not with the list header so if the node is at the front of the list, it will have to be dealt with accordingly
struct node* removeU(struct node *head, char* username);

#endif