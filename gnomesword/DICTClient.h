/*
 * File:		DICTClient.h
 * Version:		0.9
 * History:             see DICTClient.cc
 *
 * Legalese
 * Copyright (C) 1999 Sudhakar Chandrasekharan (thaths@netscape.com)
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
#ifndef DICT_HH
#define DICT_HH

#include <string>
#include <vector>

class DICTClient {
private:
  DICTClient(const DICTClient & other);
  DICTClient & operator=(const DICTClient & other);
  
public:

  DICTClient(const char * h = "dict.org", int p = 2628);
  ~DICTClient() {hangup();}

  void dial();                     // Connect to dict server */
  void hangup();                   // diconnect from dict server
  void setHost(const char * h = "dict.org", int p = 2628); 
                                   // set dict server
  const char * getHost();	   // Get name of dict server
  int getPort();		   // Get port of dict server
  bool isConnected() {return connected;}

  struct Definition {
    string book;
    string definition;
    Definition(const string & b) : book(b) {}
  };
  typedef vector<Definition> Definitions;

  Definitions lookup(const char *, const char * book = "*");   
                                // Looks up a defination and 

  struct Exception {
    virtual string message() const = 0;
    virtual ~Exception() {}
  };
  struct BadPort : public Exception {
    int port;
    BadPort(int p) : port(p) {}
    string message() const;
  };
  struct InvalidHost : public Exception {
    string host_name;
    InvalidHost(const string & n) : host_name(n) {}
    string message() const;
  };
  struct CannotMapTcpToProtocolNumber : public Exception {
    string message() const {return "Cannot map \"tcp\" to protocol number";}
  };
  struct SocketCreationFailed : public Exception {
    string message() const {return "Socket creation failed";}
  };
  struct SetsockoptFailed : public Exception {
    string message() const {return "setsockopt() failed";}
  };
  struct ConnectFailed : public Exception {
    string message() const {return "connect() failed";}
  };
  struct NotConnected : public Exception {
    string message() const {return "Not connected";}
  };
  struct ServerDisconnected : public Exception {
    string message() const {return "The server disconnected";}
  };

private:
  int		sd;		/* socket descriptor */
  string	host;	/* dict server */
  int		port;	/* port of dict server */
  int		connected;	/* A flag to indicate whether connected */

  int		readline(int, char *, int);
  string	find_book(const char *);
};

struct WordNetDefinition {
  string part_of_speech;
  string number;
  string definition;
};

vector<WordNetDefinition> wordnet_split(const string &);

#endif
//#endif /* USE_ASPELL */