/*
 * File:		DICTClient.cpp
 * Version:		0.9
 * History:
 * 01/01/99		First revision.  Created skeleton.
 * 01/04/99		Added methods setHost(), getNumOfMatches()
 * 01/05/99		Added methods getHost(), getPort(), getBook(), getDefinition()
 *				Also rewrote lookup() to use string [] for books and definitions
 * 09/26/99 (kevin)     Name changed to DICTClinet.cc.
 *                      Added utilty for spliting up wordnet definitions
 *                      Added ability to only query for a specific book
 *                      Fixed memory problems, made more C++ish.  
 *                      Will now throw exceptions. And a lot more.
 *
 * Description:	A library to provide dict interface to various programs
 * 
 * Legalese
 * Copyright (C) 1999  Sudhakar Chandrasekharan (thaths@netscape.com)
 * Copyright (C) 1999  Kevin Atkinson (kevinatk@home.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
//#ifdef USE_ASPELL
#include <iostream.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string>

#include "DICTClient.h"

// Constructor
DICTClient::DICTClient(const char * h, int p) {
  connected = 0;
  host = h;
  port = p;
}

// Method to reset the server to something else
void DICTClient::setHost(const char * h, int p) {
  if (connected == 1) {
    hangup();	/* Disconnect if already connected */
  }
  host = h;
  port = p;
}

// Method to return the server name
const char * DICTClient::getHost() {
  return host.c_str();
}

// Method to return the server port
int DICTClient::getPort() {
  if (port> 0) {
    return port;
  } else {
    return 0;
  }
}

// Method to connect to the dict server
void DICTClient::dial() {
  struct hostent *ptrh;  /* pointer to a host table entry     */
  struct protoent *ptrp; /* pointer to a protocol table entry */
  struct sockaddr_in sad;/* structure to hold server's address*/
  int    option_value;   /* GPI: needed for setsockopt        */
  
  memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
  sad.sin_family = AF_INET;  /* set family to Internet */
  
  /* Test for legal value */
  if (port > 0) {
    sad.sin_port = htons((u_short)port);
  } else {
    /* Print error message and exit */
    throw BadPort(port);
  }
  
  /* Convert host name to equivalent IP address and copy sad */
  ptrh = gethostbyname(host.c_str());
  if (((char *)ptrh) == NULL) {
    throw InvalidHost(getHost());
  }
  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
  
  /* Map TCP tranbsport protocol name to protocol number */
  if (((int)(ptrp = getprotobyname("tcp"))) == 0) {
    throw CannotMapTcpToProtocolNumber();
  }
  
  /* Create a socket */
  sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0) {
    throw SocketCreationFailed();
  }
  
  /* GPI: Make sure that port used will be immediately reusable */
  option_value = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, 
		 (char *)&option_value, sizeof(option_value)) < 0) {
    throw SetsockoptFailed();
  }
  
  /* Connect the socket to the specified server */
  if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    throw ConnectFailed();
  }
  connected = 1;
}

// Method to disconnect from the dict server
void DICTClient::hangup() {
  if(connected == 1) {
    close(sd);
    connected = 0;
  }
}

// Method to lookup a word.
DICTClient::Definitions 
DICTClient::lookup(const char * word, const char * book) {
  Definitions definitions;
  char	buf[1024];
  int n=0;
  int count=0;
  
  if(connected != 1) {
    throw NotConnected();
  }
  
  /* Construct the query to send to the server */
  string query = string("DEFINE ") + book + " " + word + "\n\r";
  
  send(sd,query.c_str(),query.size(),0); 
  definitions.clear();
  
  while ((n=readline(sd,buf,sizeof(buf))) > 0) {
    if (strncmp(buf,"42",2) == 0) {
      throw ServerDisconnected();
    }
    
    if (strncmp(buf,"220",3) == 0)
      continue;	/* Ignore successful connection message */
    
    if (strncmp(buf,"250",3) == 0)
      break;		/* break out if completed successfully */
    
    if (strncmp(buf,"150",3) == 0) {
      /* How many matches */
      /* numOfMatches = atoi(buf+3); */
      continue;
    }

    /* Ignore EO each entry from server */
    if (strncmp(buf,".",1) == 0) {
      count++;
      continue;
    }
    
    /* Definition found */
    if (strncmp(buf,"151",3) == 0) {
      definitions.push_back(find_book(buf));
      continue;
    }
    
    /* break out if no matches found */
    if(strncmp(buf,"552",3) == 0) {
      break;
    }
    
    definitions[count].definition.append(buf);
  }
  return definitions;
}

string DICTClient::BadPort::message() const {
  return "Bad port number";
}

string DICTClient::InvalidHost::message() const {
  return "The host " + host_name + " is invalid";
}

/* Function to read one line at a time from socket */
int DICTClient::readline(int sdesc, char *cptr, int maxlen) {
  int i, nc;
  char c;

  for (i=0; i<maxlen; i++) {
    if ((nc = read(sdesc,&c,1)) == 1) {
      *cptr++ = c;
      if (c == '\n') {
	break;
      }
    } else if (nc == 0) {
      if (i == 1)
	return 0;
      else
	break;
    }
  }
  *cptr = '\0';
  return i;
}

string DICTClient::find_book(const char * str) {
  int i=0;
  int j=0;
  string bk;
  bk.resize(strlen(str));
  for (i=0, j=0; i<strlen(str); i++) {
    if (str[i] == '"') {
      if (str[i-1] == ' ') {
	j=0;
      }
      continue;
    } else {
      bk[j] = str[i];
      j++;
    }
  }
  return bk;
}

vector<WordNetDefinition> wordnet_split(const string & in) {
  vector<WordNetDefinition> defs;
  string def_str;
  int n1 = in.find('\n');
  int n2 = in.find('\n', n1+1);
  int n = 0;
  def_str = in.substr(n1+6,n2-1-6-n1);
  for (;;) {
    if (n2 == string::npos) break;
    n1 = n2;
    n2 = in.find('\n', n2 + 1);
    if (in[n1 + 6] != ' ') {
      // split def_str into its parts and add to the list
      WordNetDefinition def;
      n = def_str.find(':');
      def.definition = def_str.substr(n+2);
      int m = def_str.find(' ');
      if (m > n) {
	def.number = def_str.substr(0,n);
      } else {
	def.part_of_speech = def_str.substr(0,m);
	def.number = def_str.substr(m+1, n - m - 1);
      }
      defs.push_back(def);
      def_str = "";
    }
    n = in.find_first_not_of(' ', n1+1);
    if (n == string::npos) break;
    if (def_str.size()) def_str += ' ';
    def_str += in.substr(n,n2-1-n);
  }
  return defs;
}

//#endif /* USE_ASPELL */
