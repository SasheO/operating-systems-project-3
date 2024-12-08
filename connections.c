#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "connections.h"

/////////////////// USERLIST //////////////////////////

//insert connection at the first location. setting head as NULL means this just creates and returns a new connection
struct connection* createAndInsertConnection(struct connection *head, char *username1, char *username2, char* message_output){
  if(findConnection(head,username1,username2) == NULL) {
        
        if ((username1[0]=='\0')||username2[0]=='\0'){
          sprintf(message_output, "Connection failed: Empty username\n");
          return head;
        }
        else if (strcmp(username1, username2)==0){
          sprintf(message_output, "Connection failed: Cannot DM self\n");
          return head;
        }
       //create a link
       struct connection *link = (struct connection*) malloc(sizeof(struct connection));
       strcpy(link->username1,username1);
       strcpy(link->username2,username2);
       
       //point it to old first connection
       link->next = head;

       //point first to new first connection
       head = link;
       sprintf(message_output, "Connnection created between %s::%s\n", username1, username2);
 
   }
   else
       sprintf(message_output, "Connnection already exists between %s::%s\n", username1, username2);
   return head;
}

// insert already existing link to first location
struct connection* insertConnection(struct connection *connectionlist, struct connection *link);

//find a connection with given username
struct connection* findConnection(struct connection *head, char *username1, char *username2){
  //start from the first link
   struct connection* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   int condition = isConnection(current, username1, username2);
    while(1){
      if (condition==1){
        break;
      }
	
      //if it is last connection
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
         
      }
      condition = isConnection(current, username1, username2);
   }      
	
   //if username found, return the current Link
   return current;
}

//remove link with given user. reassign head to the outptut of this function since it modifies the linked list
struct connection* removeConnection(struct connection *head, char *username1, char *username2, char* message_output){
  //start from the first link
   struct connection* current = head;
   struct connection* prev = NULL;

   //if list is empty
   if(head == NULL) {
      sprintf(message_output, "Connnection does not exist.\n");
      return NULL;
   }

   //navigate through list
    int condition = isConnection(current, username1, username2);
    while(1){
      if (condition==1){
        break;
      }
      //if it is last connection
      if(current->next == NULL) {
        sprintf(message_output, "Connnection does not exist.\n");
         return NULL;
      } else {
         //go to next link
         prev = current;
         current = current->next;
      }
      condition = isConnection(current, username1, username2);
   }      
	
   //if username found, remove and the current Link
   condition = isConnection(current, username1, username2);
   if (condition){
    sprintf(message_output, "Connection successfully closed.\n");
    if (prev!=NULL){ // not first link
      prev->next = current->next;
      current->next = NULL;
      free(current); // deallocate
      current = NULL;
      return head;
    }
    else{ // first like i.e. no prev or prev == NULL
      sprintf(message_output, "Connection successfully closed.\n");
      current = current->next;
      head->next = NULL;
      free(head); // deallocate
      head = NULL;
      return current;
    }
    
    
   }
   sprintf(message_output, "Connnection does not exist.\n");
   return NULL;
}

void printConnections(struct connection *head){
  struct connection* current = head;
  while (current!=NULL){
    printf("%s::%s\n", current->username1, current->username2);
    current = current->next;
  }
}

int userInConnection(struct connection *c, char* username){
  
  if ((strcmp(c->username1, username)==0)||(strcmp(c->username2, username)==0)){
    return 1;
  }
  return 0;
}

char * getOtherUser(struct connection *c, char* username){
  if ((strcmp(c->username1, username)==0)){
    return c->username2;
  }
  if ((strcmp(c->username2, username)==0)){
    return c->username1;
  }
  return NULL;
}

void renameUserInConnectionsList(struct connection * connectionlist, char * oldusername, char * newusername){
  struct connection* current = connectionlist;
  while (current!=NULL){
    if (userInConnection(current, oldusername)){
      if (strcmp(current->username1, oldusername)==0){
        strcpy(current->username1, newusername);
      }
      else if (strcmp(current->username2, oldusername)==0){
        strcpy(current->username2, newusername);
      }
    }
    current = current->next;
  }
}

int isConnection(struct connection * current, char *username1, char * username2){
  int response = ((strcmp(current->username1, username1) == 0)&&(strcmp(current->username2, username2) == 0))||((strcmp(current->username1, username2) == 0)&&(strcmp(current->username2, username1) == 0));
  return response;
}

struct connection * removeAllConnectionsWithUserFromConnectionsList(struct connection * connections, char *username){
  struct connection * head = connections;
  struct connection * next;
  struct connection * prev;
  char * other_username;

  char dummymessage[30];
  dummymessage[0] = '\0';
  while (userInConnection(head,username)){
    next = head->next;
    free(head);
    head = NULL;
    head = next;
  }

  prev = head;
  next = head->next;

  while(next!=NULL){
    if(userInConnection(next,username)){
      other_username = getOtherUser(next, username);
      prev->next = removeConnection(next, username, other_username, dummymessage);
      next = prev->next;
    }
    else{
      prev = prev->next;
      next = next->next;
    }
  }


  return head;
}