//
// Created by 傅永健 on 2017/5/21.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "Responsor.h"
#include "Request.h"

using std::string;

#define MAX_BUFFER 4096

class Server {
private:
    int server_socket;
    int port;
    int max_wait_queue;
    char* buffer;
    Responsor* responsor;
    string ip_addr;
public:
    Server(string ip_addr, int port, string root);
    int run();

};


#endif //WEBSERVER_SERVER_H
