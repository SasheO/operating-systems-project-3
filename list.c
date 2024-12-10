/*
Authors: Mezisashe Ojuba, Emily Jones
Date: 8 Dec 2024.
Version: 1.0
Distribution: https://github.com/SasheO/operating-systems-project-3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#ifndef LIST_H
#define LIST_H


struct node {
   char username[30];
   int socket;
   struct node *next;
};

/////////////////// USERLIST //////////////////////////

// Create and insert a node at the first location
struct node* createAndInsertU(struct node *head, int socket, char *username);

// Insert an already existing link to the first location
struct node* insertU(struct node *nodelist, struct node *link);

// Find a node with a given username
struct node* findU(struct node *head, char* username);

// Remove a link with a given username
struct node* removeU(struct node *head, char* username);

// Rename a user
void renameU(struct node *head, char* oldusername, char* newusername);

// Delete and deallocate every node in the list
void deleteAllUs(struct node *head);

#endif // LIST_H

