//
// Created by 傅永健 on 2017/5/21.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#ifndef WEBSERVER_RESPONSOR_H
#define WEBSERVER_RESPONSOR_H
#include <string>
#include <sys/stat.h>
#include <fstream>
#include "Request.h"

#define MAX_HEAD_LEN 4096
using std::string;


class Responsor {
private:
    string server_name;
    string root;
    map<string, string(*)(Request*)> cgi_list;
public:
    Responsor(string root);
    string response(Request* request);
    void set_cgi_list(map<string, string(*)(Request*)> cgis);
private:
    string static_file_resonse(string filename);
    string dynamic_get();
    string dynamic_post();
    string compose_head(int length, struct stat* file_state, int state);
    string format_date_gmt(time_t time);
    string state_info(int state);
    string retr_404();
    string execute_cgi(Request* request);
};


#endif //WEBSERVER_RESPONSOR_H
