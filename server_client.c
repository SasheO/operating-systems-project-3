#include "server.h"

// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE
extern int numReaders;
extern pthread_mutex_t rw_lock;
extern pthread_mutex_t mutex;

extern char *server_MOTD;
int next_room_ID = 1; // do not modify except in get_next_room_ID

struct room * ROOMS[MAX_NUM_ROOMS];
extern struct node * head;

int get_next_room_ID();
/*
 *Main thread for each client.  Receives all messages
 *and passes the data off to the correct function.  Receives
 *a pointer to the file descriptor for the socket the thread
 *should listen on
 */

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void create_default_room(){
  ROOMS[0] = create_room(DEFAULT_ROOM_ID, DEFAULT_ROOM);
}

void *client_receive(void *ptr) {
  int client = *(int *) ptr;  // socket

  int received, i;
  char buffer[MAXBUFF], sbuffer[MAXBUFF];  //data buffer of 2K  
  char tmpbuf[MAXBUFF];  //data temp buffer of 1K  
  char cmd[MAXBUFF], username[30];
  char *arguments[80];
  char roomname[30];
  int roomID;


  struct node *current_user;
  struct node *other_user; // for whenever we need to handle a user that is not self

  send(client  , server_MOTD , strlen(server_MOTD) , 0 ); // Send Welcome Message of the Day.

  // Creating the guest user name

  sprintf(username,"guest%d", client);

  // Create guest and add to the users list "head"
  head = createAndInsertU(head, client , username); 
  // add the GUEST to the DEFAULT ROOM (i.e. Lobby)
  current_user = createAndInsertU(NULL, client , username); 
  add_user_to_room(current_user, ROOMS[DEFAULT_ROOM_ID]);

  while (1) {
      
    if ((received = read(client , buffer, MAXBUFF)) != 0) {

      buffer[received] = '\0'; 
      strcpy(cmd, buffer);
      strcpy(sbuffer, buffer);

      /////////////////////////////////////////////////////
      // we got some data from a client

      // 1. Tokenize the input in buf (split it on whitespace)

      // get the first token 

      arguments[0] = strtok(cmd, delimiters);

      // walk through other tokens 

      i = 0;
      while( arguments[i] != NULL ) {
        arguments[++i] = strtok(NULL, delimiters); 
        strcpy(arguments[i-1], trimwhitespace(arguments[i-1]));
      } 


      // Arg[0] = command
      // Arg[1] = user or room

      /////////////////////////////////////////////////////
      


      if(strcmp(arguments[0], "create") == 0){
        // TODO: acquire locks for the ROOMS to create it
        printf("create room: %s\n", arguments[1]); 
        // perform the operations to create room arg[1]
        if(arguments[1]==NULL){
          sprintf(buffer, "Failed creating room: Enter room name\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
        // create room with name given or newroom+socketnumber if user did not name room
          roomID = get_next_room_ID();
          if(roomID>=MAX_NUM_ROOMS){
          // too many rooms already
            sprintf(buffer, "You cannot create a room at this time. Max room number reached!\n");
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
          }
          else{
            strcpy(roomname, arguments[1]);
            ROOMS[roomID] = create_room(roomID, roomname);
            sprintf(buffer, "Success creating room\n");
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
          }
        }

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }

      else if (strcmp(arguments[0], "join") == 0){
        // TODO: acquire locks to join room
        printf("join room: %s\n", arguments[1]);  
        if (arguments[1]==NULL){
          sprintf(buffer, "Give the name of the room you want to join.\nCommand 'rooms' gives a list of available rooms.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
          // perform the operations to join room arg[1]
          strcpy(roomname, arguments[1]);
          int indx = 0;
          while (indx<next_room_ID){ // find the room with the given name. if room does not exist, while loop will run until indx==next_room_ID
            if (strcmp(ROOMS[indx]->roomname, roomname)==0){
              add_user_to_room(current_user, ROOMS[indx]);
              sprintf(buffer, "You have been added to room # %d: %s\n", ROOMS[indx]->roomID, ROOMS[indx]->roomname);
              send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }
            indx ++;
          }
        }
        

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }
      else if (strcmp(arguments[0], "leave") == 0){
        // TODO: acquire locks to leave room
        printf("leave room: %s\n", arguments[1]); 
        if (arguments[1]==NULL){
          sprintf(buffer, "Give the name of the room you want to leave.\nCommand 'rooms' gives a list of available rooms.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
          int indx = 0;
          strcpy(roomname, arguments[1]);
          while (indx<next_room_ID){
            if (strcmp(ROOMS[indx]->roomname, roomname)==0){
              remove_user_from_room(ROOMS[indx], current_user->username);
            }
            indx ++;
          }
        }
        
        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      } 
      else if (strcmp(arguments[0], "connect") == 0){
        // TODO: acquire locks to connect to user
        printf("connect to user: %s \n", arguments[1]);

        // perform the operations to connect user with socket = client from arg[1]

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }
      else if (strcmp(arguments[0], "disconnect") == 0){
        // TODO: acquire locks to disconnect to user             
        printf("disconnect from user: %s\n", arguments[1]);

        // perform the operations to disconnect user with socket = client from arg[1]

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }                  
      else if (strcmp(arguments[0], "rooms") == 0) {
        // TODO: acquire locks to read room (ensure no one is writing)
        printf("List all the rooms\n");

        // must add put list of rooms into buffer to send to client
        int indx = 0;
        while(indx<next_room_ID){
          strcat(buffer, ROOMS[indx]->roomname);
          strcat(buffer, ": ");
          other_user = ROOMS[indx]->users;
          while (other_user!=NULL){
            strcat(buffer, other_user->username);
            strcat(buffer, ", ");
            other_user = other_user->next;
          }
          strcat(buffer, "\n");
          indx ++;
        }

        strcat(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client                            
      }
      else if (strcmp(arguments[0], "users") == 0) {
        // TODO: acquire locks to print users (ensure no one is writing)
        printf("List all the users\n");

        // must add put list of users into buffer to send to client
        other_user = head;
        printf("1\n");
        while(other_user!=NULL){
          strcat(buffer, other_user->username);
          strcat(buffer, "\n");
          other_user = other_user->next;
        }
        
        strcat(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }                           
      else if (strcmp(arguments[0], "login") == 0) {

        // TODO: change usernames in DMS
        // TODO: acquire locks to modify rooms
        if (arguments[1]!=NULL){
          strcpy(username, arguments[1]);
          renameU(head, current_user->username, username);
          int indx = 0;
          while (indx<next_room_ID){
            renameU(ROOMS[indx]->users, current_user->username, username);
            indx ++;
          }
          strcpy(current_user->username, username);
        }
        else{
          sprintf(buffer, "Not logged in: Enter your login name\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        
        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      } 
      else if (strcmp(arguments[0], "help") == 0 ) {
        sprintf(buffer, "login <username> - \"login with username\" \ncreate <room> - \"create a room\" \njoin <room> - \"join a room\" \nleave <room> - \"leave a room\" \nusers - \"list all users\" \nrooms -  \"list all rooms\" \nconnect <user> - \"connect to user\" \nexit - \"exit chat\" \n\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client 
      }
      else if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "logout") == 0) {

        //TODO: Remove the initiating user from all rooms and direct connections, then close the socket descriptor.
        close(client); // close socket descriptor
      }                         
      else { 
        /////////////////////////////////////////////////////////////
        // 3. sending a message

        // send a message in the following format followed by the promt chat> to the appropriate receipients based on rooms, DMs
        // ::[userfrom]> <message>

        sprintf(tmpbuf,"\n::%s> %s\nchat>", "PUTUSERFROM", sbuffer);
        strcpy(sbuffer, tmpbuf);

        //  current_user = head;
        //  while(current_user != NULL) {

        //      if(client != current_user->socket){  // dont send to yourcurrent_user 

        //          send(current_user->socket , sbuffer , strlen(sbuffer) , 0 ); 
        //      }
        //      current_user = current_user->next;
        //  }

      }

      memset(buffer, 0, sizeof(1024));
    }
  }
  return NULL;
}


int get_next_room_ID(){
  // TODO: acquaire lock for next_room_ID
  next_room_ID++;
  return next_room_ID-1;
}