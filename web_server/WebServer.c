/* http_server.c - http 1.0 server  */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "config.h"
#include "helpers.h"

/*------------------------------------------------------------------------
 * Program:   http server
 *
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) read http request, reply to http request
 *              (3) close the connection
 *              (4) go back to step (1)
 *
 * Syntax:    http_server [ port ]
 *
 *               port  - protocol port number to use
 *
 * Note:      The port argument is optional.  If no port is specified,
 *            the server uses the port specified in config.h
 *
 *------------------------------------------------------------------------
 */

int main(int argc, char *argv[])
{
  struct  sockaddr_in serv_addr, cli_addr; /* structure to hold server's address  */
  int     listen_socket, connection_socket;
  int     port;            
  pid_t   pid;  /* id of child process to handle request */
  char    response_buffer[MAX_HTTP_RESP_SIZE];  
  int     status_code;
  char *  status_phrase;

  /* 1) Create a socket */

    listen_socket = socket(AF_INET , SOCK_STREAM , 0);
     
    if (listen_socket == -1)
    {
        printf("Could not create socket");
    }

  /* Check command-line argument for port and extract    */
  /* port number if one is specified.  Otherwise, use default  */

  if (argc > 1) {                 /* if argument specified        */
    port = atoi(argv[1]);   /* convert from string to integer   */
  } else {
    port = DEFAULT_PORT;
  }
  if (port <= 0) {             /* test for legal value       */
    fprintf(stderr, "bad port number %d", port);
    exit(EXIT_FAILURE);
  }

  /* 2) Set the values for the server  address structure:  serv_addr */
  memset(&serv_addr,0,sizeof(serv_addr)); /* clear sockaddr structure */

  //Prepare the sockaddr_in structure
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons( port );

  /* 3) Bind the socket to the address information set in serv_addr */

  //Bind
    if( bind(listen_socket,(struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0)
    {
        puts("bind failed");
    }
    puts("bind done");

  /* 4) Start listening for connections */

  listen(listen_socket, 3);

  /* Main server loop - accept and handle requests */

  while (true) {

    /* 5) Accept a connection */

    //Accept an incoming connection
	int c = sizeof(struct sockaddr_in);
    connection_socket = accept(listen_socket, (struct sockaddr *)&cli_addr, (socklen_t*)&c);
    if (connection_socket<0)
    {
        perror("accept failed");
    }
     
    puts("Connection accepted");

    /* Fork a child process to handle this request */

    if ((pid = fork()) == 0) {

      /*----------START OF CHILD CODE----------------*/
      /* we are now in the child process */
      
      /* child does not need access to listen_socket */
      if ( close(listen_socket) < 0) {
        fprintf(stderr, "child couldn't close listen socket");
        exit(EXIT_FAILURE);
      }

      struct http_request new_request; // defined in httpreq.h
      /* 6) call helper function to read the request         *
       * this will fill in the struct new_request for you *
       * see helper.h and httpreq.h                       */

      bool success = Parse_HTTP_Request(connection_socket, &new_request);

      /*** PART A:  PRINT OUT
       *   URI, METHOD and return value of  Parse_HTTP_Request()
       */

	//   printf("PRINTING URI:");
    //   printf("%s\n",new_request.URI);
	//   printf("PRINTING METHOD:");
    //   printf("%s\n",new_request.method);
	//   printf("PRINTING STATUS:");
    //   printf("%i\n",success);


      /****  PART B ONLY *******/

      /* 7) Decide which status_code and reason phrase to return to client */

	// status_code
	// status_phrase


	if(strcmp(new_request.method, "GET") == 0 || strcmp(new_request.method, "HEAD") == 0){
		if(Is_Valid_Resource(new_request.URI)){
			status_code = 200;
			status_phrase = "OK";
		} else {
			status_code = 404;
			status_phrase = "Not Found";
		}
	} else if(new_request.URI[0] != '/'){
		status_code = 400;
		status_phrase = "Bad Request";
	} else {
		status_code = 501;
		status_phrase = "Not Implemented";
	}

	// Date:
	// Pragma:
	// Location:
	// Server:

      // set the reply to send
      sprintf(response_buffer, "HTTP/1.0 %d %s\r\n", status_code, status_phrase); 
      printf("Sending response line: %s\n", response_buffer);
      send(connection_socket ,response_buffer,strlen(response_buffer),0);
      


      // send resource if requested, under what condition will the server send an 
      // entity body?

      if (strcmp(new_request.method, "GET") == 0)
	      Send_Resource(connection_socket, new_request.URI);
      else if (strcmp(new_request.method, "HEAD") == 0){
		  Send_Header(connection_socket, new_request.URI);
	  }
	  else {
        // don't need to send resource.  end HTTP headers
        send(connection_socket, "\r\n\r\n", strlen("\r\n\r\n"), 0);
      }
      
      /***** END PART B  ****/

      /* child's work is done, close remaining descriptors and exit */

      if ( close(connection_socket) < 0) {
        fprintf(stderr, "closing connected socket failed");
        exit(EXIT_FAILURE);
      }

      /* all done return to parent */
      exit(EXIT_SUCCESS);

    }  
    /*----------END OF CHILD CODE----------------*/

    /* back in parent process  */
    /* close parent's reference to connection socket, */
    /* then back to top of loop waiting for next request */
    if ( close(connection_socket) < 0) {
      fprintf(stderr, "closing connected socket failed");
      exit(EXIT_FAILURE);
    }

    /* if child exited, wait for resources to be released */
    waitpid(-1, NULL, WNOHANG);

  } // end while(true)
}
