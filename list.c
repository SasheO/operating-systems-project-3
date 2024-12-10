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

// Create and insert a node at the first location
struct node* createAndInsertU(struct node *head, int socket, char *username) {
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    strcpy(new_node->username, username);
    new_node->socket = socket;
    new_node->next = head;
    return new_node;
}

// Insert an already existing link to the first location
struct node* insertU(struct node *nodelist, struct node *link) {
    link->next = nodelist;
    return link;
}

// Find a node with a given username
struct node* findU(struct node *head, char* username) {
    struct node *current = head;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Remove a link with a given username
struct node* removeU(struct node *head, char* username) {
    struct node *current = head;
    struct node *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            if (previous == NULL) {
                head = current->next; // Node is at the head
            } else {
                previous->next = current->next;
            }
            free(current);
            return head;
        }
        previous = current;
        current = current->next;
    }
    return head;
}

// Rename a user
void renameU(struct node *head, char* oldusername, char* newusername) {
    struct node *user = findU(head, oldusername);
    if (user != NULL) {
        strcpy(user->username, newusername);
    }
}

// Delete and deallocate every node in the list
void deleteAllUs(struct node *head) {
    struct node *current = head;
    while (current != NULL) {
        struct node *temp = current;
        current = current->next;
        free(temp);
    }
}

