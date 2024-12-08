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

struct node * remove_user_from_room(struct room * myroom, char username[30]){
  //  returns null if user not there or error
  struct node * firstuser=myroom->users;
  struct node * user;
  if (strcmp(firstuser->username, username) == 0){
    myroom->users = NULL;
    return firstuser;
  }
  else{ 
    user = removeU(firstuser, username);
    return user;
  }
}
