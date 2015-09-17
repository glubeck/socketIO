#include <iostream>
#include "server.h"

using namespace std;

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
  string request = get_request(client);
  string cache = request;
  string cache2;
  int i = 8;
    while (i != 0) {
      i--;
      //cout << "another iteration" << endl;
        // get a request



      
      //  string request = get_request(client);
      // break if client is done or an error occurred
      if (request.empty())
	break;
      // send response
      bool success = false;
      bool needed = false;
      int byteNum;
      Message *message;

        
	//cout << cache.length() << endl;
	//cout << cache << endl;
	  
	message = parse_request(cache);
	  
	if(message->needed) {
	  cache = get_value(client, message->length, cache);
	  //cout << cache << endl;
	  message->value = cache;
	}

	
	  
	//cout << "trying to store" << endl;
	message->value.erase(message->length, message->value.length()-1);
	//cout << message->value << endl;

	if(message->length > 0)
	  cache.erase(0, message->value.length()+message->firstLine.length()+1);
	else
	  cache.erase(0, 1+message->firstLine.length());
	    //cout << cache << endl;

	storeMessage(message);

	stringstream ss;
	ss << message->value.length();
	cout << "Stored a file called " + message->fileName
	  + " with " + ss.str() + " bytes" << endl;
	
	// std::cout << message->value << "\n" << endl;
	  
	  
    }
    close(client);
}

string Server::get_value(int client, int messageLength, string cache) {

  // send_response(client, "\n");
  int left = messageLength - cache.length();  
  string cache2 = cache;

  while(left > 0) {
    //cout << left << endl;
    int nread = recv(client,buf_,1024,0);
    
    left -= nread;

    // stringstream ss;
    // ss << left;
    //  send_response(client, ss.str() + "\n");
  
    if (nread < 0) {
      if (errno == EINTR)
	// the socket call was interrupted -- try again
	continue;
      else
	// an error occurred, so break out
	break;
    } else if (nread == 0) {
      // the socket is closed
      break;
    }

    cache2.append(buf_,nread);
  }
  return cache2;
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



Message* Server::parse_request(string request) {

  bool needed;
  int byteNum;
  Message *dummy = new Message();
  string text = "";
  if(containsNewline(request)) {
    //cout << "contains new line" << endl;


    vector<string> halves = half(request);

    //cout << halves[0] << endl;
    
    vector<string> elements = split(halves[0], ' ');
    

    if(elements[0] == "store" && elements.size() == 3) {
      
      //cout << "tyring to store " << elements[1] << endl;
      if(isNumber(elements[2])) {
	
	byteNum = atoi(elements[2].c_str());
	//cout << "this has " << byteNum << " bytes" << endl;


	if(byteNum > request.length()) {
	  needed = true;
	}
	if(byteNum <= request.length()) {
	  needed = false;
	}
	
	Message *message = new Message(halves[0], elements[0], elements[1], byteNum,
	  			       halves[1], needed);
	
	return message;

      }
      return dummy;
    }
    return dummy;
  }
  return dummy;
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
    if(str[i] != '\n') {
      first += str[i];
      second.erase(0,1);
    }
    if(str[i] == '\n') {
      halves.push_back(first);
      second.erase(0,1);
      halves.push_back(second);
      break;
    }
  }
  return halves;
}



vector<string> Server::splitCache(string cache, int byteNum) {


  string first = "";
  string second = cache;
  vector<string> halves;
  
  for(size_t i = 0; i < byteNum; i++) {

    first+=cache[i];
    second.erase(0,1);
  }
  halves.push_back(first);
  halves.push_back(second);
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
    if (str[i] == '\n') {
      return true;
    }
  }
  return false;
}
