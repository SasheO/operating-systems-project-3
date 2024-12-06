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