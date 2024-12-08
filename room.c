/*
Authors: Mezisashe Ojuba (sole group member)
Date: 8 Dec 2024.
Version: 1.0
Distribution: https://github.com/SasheO/operating-systems-project-3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "room.h"

struct room * create_room(int roomID, char roomname[30]) {
  struct room *myroom = (struct room*) malloc(sizeof(struct room));
  myroom->roomID = roomID;
  strcpy(myroom->roomname,roomname);
  myroom->users = NULL;
  return myroom;
}

void name_room(struct room * myroom, char roomname[30]){
  strcpy(myroom->roomname,roomname);
}

void add_user_to_room(struct node* user, struct room * myroom){
  myroom->users = createAndInsertU(myroom->users, user->socket, user->username);
} 

void remove_user_from_room(struct room * myroom, char username[30]){
  //  returns null if user not there or error
  myroom->users = removeU(myroom->users, username);
}

void renameUserInRoom(struct room* myroom, char * oldusername, char * newusername){
  renameU(myroom->users, oldusername, newusername);
}

void deleteRoom(struct room *myroom){
  deleteAllUs(myroom->users);
  free(myroom);
  myroom=NULL;
}