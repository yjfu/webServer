//
// Created by 傅永健 on 2017/5/22.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#include "Request.h"

Request::Request(string request) {
    //this->request = URLDecode(request);
    this->request = request;
    this->parse();

}

void Request::parse() {
    stringstream ss(this->request);
    //method:POST/HEAD/GET
    //src:url/cgi
    string method, src;
    ss>>method;
    ss>>src;
    if(method[0] == 'G'){
        if(src.find('?') == -1){
            this->type = static_req;
            if(src[src.length()-1]=='/')
                src += "index.html";
            this->url = src;
        }
        else{
            this->type = dynamic_get_req;
            this->dynamic_get_parse(src);
        }
    }
    else if(method[0] == 'P'){
        this->type = dynamic_post_req;
        this->cgi = src;
        this->dynamic_post_parse();
    }
    else if(method[0] == 'H'){
        this->type = head_req;
        this->url = src;
    }
}

void Request::dynamic_get_parse(string cgi) {
    vector<string> vs = this->split(cgi, "?");
    this->cgi = vs[0];
    this->para_s = vs[1];
    vs = this->split(para_s, "&");
    for(int i = 1;i<vs.size();i++){
        string name,value;
        unsigned long sep = vs[i].find('=');
        name = vs[i].substr(0,sep);
        value = vs[i].substr(sep+1);
        name = URLDecode(name);
        value = URLDecode(value);
        this->parameters[name] = value;
    }
}

vector<string> Request::split(string s, string sep) {
    vector<string> vs;
    char* split = new char[s.length()+1];
    char* left = nullptr;
    char* piece = nullptr;
    strcpy(split,s.c_str());
    piece = strtok_r(split, sep.c_str(), &left);

    while(piece){
        vs.push_back(piece);
        piece = strtok_r(nullptr, sep.c_str(), &left);
    }
    delete [] split;
    return vs;
}

void Request::test_print() {
    if(this->type == static_req) {
        printf("method:get\n");
        printf("url:%s\n", this->url.c_str());
    }
    else if(this->type == dynamic_get_req){
        printf("method:get\n");
        printf("cgi:%s\n", this->cgi.c_str());
        for(map<string,string>::iterator im = this->parameters.begin();im != this->parameters.end();im++){
            printf("name:%s\n",im->first.c_str());
            printf("value:%s\n",im->second.c_str());
        }
    }
    else if(this->type == dynamic_post_req){
        printf("method:post\n");
        printf("cgi:%s\n", this->cgi.c_str());
        for(map<string,string>::iterator im = this->parameters.begin();im != this->parameters.end();im++){
            printf("name:%s\n",im->first.c_str());
            printf("value:%s\n",im->second.c_str());
        }
    }
}

void Request::dynamic_post_parse() {
    unsigned long entity_start = this->request.find("\r\n\r\n");
    string entity = this->request.substr(entity_start+4);
    this->para_s = entity;
    vector<string> vs = this->split(entity, "&");
    for(vector<string>::iterator iv = vs.begin();iv != vs.end();iv++){
        string name,value;
        unsigned long sep = iv->find('=');
        name = iv->substr(0,sep);
        value = iv->substr(sep+1);
        name = URLDecode(name);
        value = URLDecode(value);
        this->parameters[name] = value;
    }
}

req_type Request::get_type() {
    return this->type;
}

string Request::get_para_s() {
    return this->para_s;
}
string Request::get_cgi() {
    return this->cgi;
}

string Request::get_url() {
    return this->url;
}

map<string, string> Request::get_parameters() {
    return this->parameters;
}