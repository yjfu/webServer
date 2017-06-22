//
// Created by 傅永健 on 2017/5/22.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "mycharset.h"
using std::stringstream;
using std::string;
using std::map;
using std::vector;
typedef enum {
    static_req = 0,
    dynamic_get_req,
    dynamic_post_req,
    head_req,
} req_type;

class Request {
private:
    string para_s;
    string request;
    req_type type;
    map<string, string> parameters;
    string cgi;
    string url;
public:
    string get_para_s();
    Request(string request);
    req_type get_type();
    map<string, string> get_parameters();
    string get_cgi();
    string get_url();
    void test_print();
private:
    void parse();
    void dynamic_get_parse(string uri);
    void dynamic_post_parse();
    vector<string> split(string s, string sep);
};


#endif //WEBSERVER_REQUEST_H
