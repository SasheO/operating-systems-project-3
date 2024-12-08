/*
Authors: Mezisashe Ojuba (sole group member)
Date: 8 Dec 2024.
*/

#include "server.h"

// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE
extern int numReaders;
extern pthread_mutex_t rw_lock; // to lock when writing
extern pthread_mutex_t mutex; // for read count

extern char *server_MOTD;
extern struct node * head;
int next_room_ID = 1; // do not modify except in get_next_room_ID

struct room * ROOMS[MAX_NUM_ROOMS];
struct connection * connections;

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
  pthread_mutex_lock(&rw_lock);
  head = createAndInsertU(head, client , username); 
  // add the GUEST to the DEFAULT ROOM (i.e. Lobby)
  current_user = createAndInsertU(NULL, client , username); 
  add_user_to_room(current_user, ROOMS[DEFAULT_ROOM_ID]);
  pthread_mutex_unlock(&rw_lock);

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
        printf("create room: %s\n", arguments[1]); 
        // perform the operations to create room arg[1]
        if(arguments[1]==NULL){
          sprintf(buffer, "Failed creating room: Enter room name\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
        // create room with name given or newroom+socketnumber if user did not name room
        pthread_mutex_lock(&rw_lock);
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
          pthread_mutex_unlock(&rw_lock);
        }

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }

      else if (strcmp(arguments[0], "join") == 0){
        // TODO: check if room exists and messag user appropriately. also prevent user from creating duplicate rooms and message appropriately
        printf("join room: %s\n", arguments[1]);  
        if (arguments[1]==NULL){
          sprintf(buffer, "Give the name of the room you want to join.\nCommand 'rooms' gives a list of available rooms.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
          pthread_mutex_lock(&rw_lock);
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
          pthread_mutex_unlock(&rw_lock);
        }
        

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }
      else if (strcmp(arguments[0], "leave") == 0){
        
        printf("leave room: %s\n", arguments[1]); 
        if (arguments[1]==NULL){
          sprintf(buffer, "Give the name of the room you want to leave.\nCommand 'rooms' gives a list of available rooms.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        else{
          pthread_mutex_lock(&rw_lock);
          int indx = 0;
          strcpy(roomname, arguments[1]);
          while (indx<next_room_ID){
            // TODO: check if room exists, and message the user appropriately
            if (strcmp(ROOMS[indx]->roomname, roomname)==0){
              // TODO: check if user was even in room to begin with, and message the user appropriately
              remove_user_from_room(ROOMS[indx], current_user->username);
              sprintf(buffer, "You have been removed from ");
              strcat(buffer, ROOMS[indx]->roomname);
              send(client , buffer , strlen(buffer) , 0 ); // send back to client
            }
            indx ++;
          }
          pthread_mutex_unlock(&rw_lock);
        }
        
        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      } 
      else if (strcmp(arguments[0], "connect") == 0){
        
        
        printf("connect to user: %s \n", arguments[1]);
        if (arguments[1]==NULL){
          sprintf(buffer, "Enter the username of user you want to connect to.\nCommand 'users' gives a list of available users.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        // perform the operations to connect user with socket = client from arg[1]
        else{
          pthread_mutex_lock(&rw_lock);
          strcpy(username, arguments[1]);
          other_user = findU(head,username);
          if(other_user == NULL){
            sprintf(buffer, "User does not exist. Note that usernames are case sensitive.\n");
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
          }
          else{
            connections = createAndInsertConnection(connections, current_user->username, other_user->username, buffer);
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
            printConnections(connections);
          }
          pthread_mutex_unlock(&rw_lock);
        } 
        

        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }
      else if (strcmp(arguments[0], "disconnect") == 0){         
        printf("disconnect from user: %s\n", arguments[1]);

        // perform the operations to disconnect user with socket = client from arg[1]
        if (arguments[1]==NULL){
          sprintf(buffer, "Enter the username of user you want to disconnect from.\nCommand 'users' gives a list of available users.\n");
          send(client , buffer , strlen(buffer) , 0 ); // send back to client
        }
        // perform the operations to connect user with socket = client from arg[1]
        else{
          pthread_mutex_lock(&rw_lock);
            strcpy(username, arguments[1]);
            connections = removeConnection(connections, current_user->username, username, buffer);
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
            printf("1\n");
            printConnections(connections);
            pthread_mutex_unlock(&rw_lock);
          
        } 
        
        sprintf(buffer, "\nchat>");
        send(client , buffer , strlen(buffer) , 0 ); // send back to client
      }                  
      else if (strcmp(arguments[0], "rooms") == 0) {
        
        printf("List all the rooms\n");

        pthread_mutex_lock(&mutex);
        numReaders++;
        if(numReaders==1){
          pthread_mutex_lock(&rw_lock);
        }
        printf("\n%d Reader is inside ",numReaders);
        pthread_mutex_unlock(&mutex);
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
        
        pthread_mutex_lock(&mutex);
        numReaders--;
        if(numReaders==0){
          pthread_mutex_unlock(&rw_lock);
        }
        printf("\n%d No reader inside",numReaders);
        pthread_mutex_unlock(&mutex);
      }
      else if (strcmp(arguments[0], "users") == 0) {
        printf("List all the users\n");
        pthread_mutex_lock(&mutex);
        numReaders++;
        if(numReaders==1){
          pthread_mutex_lock(&rw_lock);
        }
        printf("\n%d Reader is inside ",numReaders);
        pthread_mutex_unlock(&mutex);
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

        pthread_mutex_lock(&mutex);
        numReaders--;
        if(numReaders==0){
          pthread_mutex_unlock(&rw_lock);
        }
        printf("\n%d No reader inside",numReaders);
        pthread_mutex_unlock(&mutex);
      }                           
      else if (strcmp(arguments[0], "login") == 0) {
        
        if (arguments[1]!=NULL){
          pthread_mutex_lock(&rw_lock);
          strcpy(username, arguments[1]);
          if (findU(head, username)!=NULL){
            sprintf(buffer, "Not logged in: Username taken.\n");
            send(client , buffer , strlen(buffer) , 0 ); // send back to client
          }
          else{
            renameU(head, current_user->username, username);
            int indx = 0;
            while (indx<next_room_ID){
              renameUserInRoom(ROOMS[indx], current_user->username, username);
              indx ++;
            }
            renameUserInConnectionsList(connections, current_user->username, username);
            strcpy(current_user->username, username);

          }
          pthread_mutex_unlock(&rw_lock);
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
        // remove user from rooms and chats, free all memory, tell other users they have left the chat, then close.
        pthread_mutex_lock(&rw_lock);
        // remove user from all rooms
        int indx = 0;
        while (indx<next_room_ID){
          remove_user_from_room(ROOMS[indx], current_user->username);
          indx++;
        }
        printf("rooms removed\n");

        // delete user
        head = removeU(head, current_user->username);
        printf("user removed\n");

        // remove user's connections
        connections = removeAllConnectionsWithUserFromConnectionsList(connections, current_user->username);
        printConnections(connections);
        printf("connections removed\n");

        free(current_user);
        current_user = NULL;

        pthread_mutex_unlock(&rw_lock);
        close(client); // close socket descriptor
        printf("exited\n");
        break;
        
      }                         
      else { 
        /////////////////////////////////////////////////////////////
        // 3. sending a message

        // send a message in the following format followed by the promt chat> to the appropriate receipients based on rooms, DMs
        // ::[userfrom]> <message>


        // int indx = 0;
        // while(arguments[indx]!=NULL){
        //   strcat(buffer, arguments[indx]);
        //   strcat(buffer, " ");
        //   indx++;
        // }

        sprintf(tmpbuf,"\n::%s> %s\nchat>", current_user->username, sbuffer);
        strcpy(sbuffer, tmpbuf);

        

        struct connection * current_connection = connections;
         while(current_connection != NULL) {
            if (userInConnection(current_connection, current_user->username)==1){
              strcpy(username, getOtherUser(current_connection, current_user->username));              
              other_user = findU(head, username);
              send(other_user->socket , sbuffer , strlen(sbuffer) , 0 ); 
            }
            current_connection = current_connection->next;
         }

      }

      memset(buffer, 0, sizeof(1024));
    }
  }
  printf("about to return\n");
  return NULL;
}


int get_next_room_ID(){
  next_room_ID++;
  return next_room_ID-1;
}

void freeAllResources(){
  pthread_mutex_lock(&rw_lock);
  struct node * current;
  while(current!=NULL){
    close(current->socket);
    current=current->next;
  }
  printf("All sockets closed.\n");
  deleteAllUs(head);
  printf("All users deleted.\n");
  int indx = 0;
  while (indx<next_room_ID){
    deleteRoom(ROOMS[indx]);
    indx ++;
  }
  printf("All rooms deleted.\n");
  closeAllConnections(connections);
  printf("All connections/DMs closed\n");
  printf("All resources have been freed\n");
  pthread_mutex_unlock(&rw_lock);
}