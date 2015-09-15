#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <map>
#include <string>

using namespace std;

     class Message {

 public:
   Message(string& command, string& fileName, int& length, string& value,
	   bool needed) {

     this->command = command;
     this->fileName = fileName;
     this->length = length;
     this->value = value;
     this->needed = needed;
   }
   Message() {

     this->command = "";
     this->fileName = "";
     this->length = 0;
     this->value = "";
     this->needed = false;
   }
   //~Message();
   
   string command; 
   string fileName; 
   int length;
   string value;
   bool needed;
   
   string getCommand() {
     return command;
   }
   void setCommand(string command) {
     this->command = command;
   }

   string getFileName() {
     return fileName;
   }
   void setFileName(string fileName) {
     this->fileName = fileName;
   }

   int getLength() {
     return length;
   }
   void setLength(int length) {
     this->length = length;
   }

   string getValue() {
     return value;
   }
   void setValue(string value) {
     this->value = value;
   }
   
   bool getNeeded() {
     return needed;
   }
   void setNeeded(bool needed) {
     this->needed = needed;
   }
  
 };

class Server {
public:
    Server();
    ~Server();

    void run();
    
protected:
    virtual void create();
    virtual void close_socket();
    void serve();
    void handle(int);
    string get_request(int);
    bool send_response(int, string);
    vector<string> split(string, char);
    bool isNumber(string);
    bool containsNewline(string);
    vector<string> half(string);
    vector<string> splitCache(string, int);
    Message* parse_request(string);
    void get_value(int, Message*);
    
    int server_;
    int buflen_;
    char* buf_;
    map<string, vector<Message*> > messageMap;

    void storeMessage(Message* message) {

      string fileName = message->fileName;
      
      if(messageMap.find(fileName) == messageMap.end()) {
	//not present in map
	vector<Message*> messages;
	messages.push_back(message);
	messageMap[fileName]=messages;
      }
      else {
	//name already present in map
	messageMap[fileName].push_back(message);
      }
    }


};


