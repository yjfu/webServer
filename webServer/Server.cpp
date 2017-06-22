//
// Created by 傅永健 on 2017/5/21.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#include "Server.h"

Server::Server(string ip_addr, int port, string root) {
    this->ip_addr = ip_addr;
    this->port = port;
    this->max_wait_queue = 20;
    this->buffer = new char[MAX_BUFFER];
    this->responsor = new Responsor(root);
}

int Server::run() {
    //创建一个套接字,相当于一个文件描述符
    this->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //sockaddr_in:用于存储地址相关信息的结构体
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(this->ip_addr.c_str());
    server_addr.sin_port = htons(this->port);
    //绑定套接字与地址信息
    int res = bind(this->server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    if(res){
        printf("端口号被占用!\n");
        return -1;
    }
    //开始监听有没有向socket传来的信息,并设定等待队列的最大值
    listen(this->server_socket, this->max_wait_queue);
    while(1){
        //客户端的地址信息
        sockaddr_in client_addr;
        socklen_t client_addr_length = (socklen_t)sizeof(client_addr);
        //得到客户端发来信息的socket
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_length);
        memset(this->buffer, 0, MAX_BUFFER);
        read(client_socket, this->buffer, MAX_BUFFER);
        printf("%s\n",this->buffer);
        Request request(this->buffer);
        string response = this->responsor->response(&request);
        request.test_print();
        write(client_socket, response.c_str(), response.length());
        fflush(stdout);
    }
}