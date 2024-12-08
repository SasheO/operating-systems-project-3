/*
Authors: Mezisashe Ojuba (sole group member)
Date: 8 Dec 2024.
Version: 1.0
Distribution: https://github.com/SasheO/operating-systems-project-3
*/

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

void remove_user_from_room(struct room * myroom, char username[30]);
void add_user_to_room(struct node* user, struct room * myroom); 
struct room * create_room(int roomID, char roomname[30]);
void name_room(struct room * myroom, char roomname[30]);

void renameUserInRoom(struct room* myroom, char * oldusername, char * newusername);

// delete and deallocate room in the list
void deleteRoom(struct room *myroom);
#endif