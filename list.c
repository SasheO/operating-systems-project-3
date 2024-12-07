#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

//insert node at the first location. setting head as NULL means this just creates and returns a new node
struct node* createAndInsertU(struct node *head, int socket, char *username, int room) {
    
   if(findU(head,username) == NULL) {
           
       //create a link
       struct node *link = (struct node*) malloc(sizeof(struct node));

       link->socket = socket;
       strcpy(link->username,username);
       link->current_room_ID = room;
       
       //point it to old first node
       link->next = head;

       //point first to new first node
       head = link;
 
   }
   else
       printf("Duplicate: %s\n", username);
   return head;
}

// insert already existing link to first location
struct node* insertU(struct node *head, struct node *link) {        
  //point it to old first node
  link->next = head;

  //point first to new first node
  head = link;
  return head;
}

//find a link with given user
struct node* findU(struct node *head, char* username) {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
    while(strcmp(current->username, username) != 0) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if username found, return the current Link
   return current;
}

//remove link with given user. this only deals with the linked list itself and not with the list header so if the node is at the front of the list, it will have to be dealt with accordingly. returns null if user not there or error
struct node* removeU(struct node *head, char* username) {

   //start from the first link
   struct node* current = head;
   struct node* prev = NULL;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
    while(strcmp(current->username, username) != 0) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         prev = current;
         current = current->next;
      }
   }      
	
   //if username found, remove and the current Link
   if (strcmp(current->username, username)==0){
    if (prev!=NULL){ // first link
      prev->next = current->next;
      current->next = NULL;
    }
    return current;
   }
   return NULL;
     
}