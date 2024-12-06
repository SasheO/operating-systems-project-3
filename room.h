#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"



struct room {
   char roomname[30]; // room will be titled something
   struct node *users;
   int roomID; 
};

struct node * remove_user_from_room(struct node* user);
struct node * add_user_to_room(struct node* user, char roomname[30]); // add user 
struct room * create_room(int roomID, char roomname[30]);
void name_room(int roomID);
#endif