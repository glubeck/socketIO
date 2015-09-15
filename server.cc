#include "server.h"

Server::Server() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    // create and run the server
    create();
    serve();
}

void
Server::create() {
}

void
Server::close_socket() {
}

void
Server::serve() {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {

        handle(client);
    }
    close_socket();
}

void
Server::handle(int client) {
    // loop to handle all requests
    while (1) {
        // get a request
        string request = get_request(client);
        // break if client is done or an error occurred
        if (request.empty())
            break;
        // send response
	bool success = false;
	int byteNum;

	if(containsNewline(request)) {
	  
	  send_response(client, "contains new Line\n");
	  
	  vector<string> halves = half(request);

	  if(halves.size() == 2) {
	    send_response(client, halves.front() + '\n');
	    send_response(client, halves.back() + '\n');
	  }

	  

	  vector<string> elements = split(halves.front(), ' ');


	  if(elements.front() == "store" && elements.size() == 3) {
	    
	    send_response(client, elements.front() + '\n');
	    send_response(client, elements.back() + '\n');
	    
	    if(isNumber(elements.back())) {
	      send_response(client, "third argument is number\n");
	      byteNum = atoi(elements.back().c_str());
	      success = send_response(client, elements.back() + '\n');

	      
	    }
	  }
	}

        //client input invalid command
        if (not success)
	  send_response(client, "Invalid input\n");
    }
    close(client);
}

string
Server::get_request(int client) {
    string request = "";
    // read until we get a newline
    while (request.find("\n") == string::npos) {
        int nread = recv(client,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        request.append(buf_,nread);
    }
    // a better server would cut off anything after the newline and
    // save it in a cache
    return request;
}

bool
Server::send_response(int client, string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

vector<string> Server::split(string s, char delim) {
  stringstream ss(s);
  string item;
  vector<string> elems;
  while(getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

vector<string> Server::half(string str) {
  string first = "";
  string second = str;
  vector<string> halves;
  for (size_t i = 0; i < str.length(); i++) {
    if(str[i] != '\\') {
      first += str[i];
      second.erase(0,1);
    }
    if(str[i] == '\\') {
      i++;
      if(str[i] == 'n') {
	halves.push_back(first);
	second.erase(0,2);
	halves.push_back(second);
	break;
      }
      else {
	first += '\\';
	first += str[i];
	second.erase(0,2);
      }
    }
  }
  return halves;
}

bool Server::isNumber(string str) {

  for (size_t i = 0; i < str.length(); i++) 
    if (!isdigit(str[i])) 
	return false;
	
  return true;
}

bool Server::containsNewline(string str) {

  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '\\') {
      i++;
      if(str[i] == 'n') {
	return true;
      }
    }
  }
  return false;
}