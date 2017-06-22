//
// Created by 傅永健 on 2017/5/21.
// Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#include <unistd.h>
#include "Responsor.h"
#include "cgi.h"
#include <stdlib.h>

string week_day[] = {
        "Sun",
        "Mon",
        "Tues",
        "Wed",
        "Thur",
        "Fri",
        "Sat",
};
string month_day[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec",
};

Responsor::Responsor(string root){
    this->server_name = "yjfu/1.0";
    this->root = root;
    enroll(this);
}
void Responsor::set_cgi_list(map<string, string(*)(Request*)> cgis) {
    this->cgi_list = cgis;
}
string Responsor::response(Request* request) {
    string response;

    switch(request->get_type()){
        case static_req:{
            string filepath = this->root+request->get_url();
            struct stat fstat;
            if(!stat(filepath.c_str(), &fstat)) {
                response = this->compose_head(0, &fstat, 200);
            }
            else {
                filepath = this->root+"/404.html";
                stat(filepath.c_str(), &fstat);
                response = this->compose_head(0, &fstat, 404);
            }
            response += this->static_file_resonse(filepath);
            break;
        }
        case dynamic_get_req:
        case dynamic_post_req:{
            if(this->cgi_list.count(request->get_cgi())){
                string entity = this->execute_cgi(request);
                //string entity = this->cgi_list[request->get_cgi()](request);
                response = compose_head((int)entity.length(), nullptr, 200)+entity;
            }
            else{
                response = this->retr_404();
            }
            break;
        }
        case head_req:{
            string filepath = this->root+request->get_url();
            struct stat fstat;
            if(!stat(filepath.c_str(), &fstat)) {
                response = this->compose_head(0, &fstat, 200);
            }
            else {
                filepath = this->root+"/404.html";
                stat(filepath.c_str(), &fstat);
                response = this->compose_head(0, &fstat, 404);
            }
            break;
        }
        default:{
            response = this->compose_head(0, nullptr, 400);
            break;
        }
    }
    return response;
}

string Responsor::compose_head(int length, struct stat* file_state, int state) {
    char* head = new char[MAX_HEAD_LEN];

    time_t t = time(nullptr);
    string date = this->format_date_gmt(t);
    string modi_date;
    if(file_state){
        timespec mode_time = file_state->st_mtimespec;
        modi_date = this->format_date_gmt(mode_time.tv_sec);
        length = (int)file_state->st_size;
    }
    else
        modi_date = date;

    sprintf(head, "HTTP/1.0 %d %s\r\n"\
            "Date: %s\r\n"\
            "Server: %s\r\n"\
            "Last-Modified: %s\r\n"\
            "Content-Length: %d\r\n"\
            "Content-Type: text/html\r\n\r\n",
            state, this->state_info(state).c_str(),
            date.c_str(),
            this->server_name.c_str(),
            modi_date.c_str(),
            length
    );

    return string(head);
}

string Responsor::format_date_gmt(time_t time) {
    tm date = {0};
    gmtime_r(&time, &date);
    char format_date[30];

    sprintf(format_date,"%s, %02d %s %d %02d:%02d:%02d GMT",
            week_day[date.tm_wday].c_str(),
            date.tm_mday,
            month_day[date.tm_mon].c_str(),
            date.tm_year+1900,
            date.tm_hour,
            date.tm_min,
            date.tm_sec);

    string s(format_date);
    return s;
}

string Responsor::state_info(int state) {
    switch(state){
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 505:
            return "HTTP Version Not Supported";
        default:
            return "illegal code";
    }
}

string Responsor::static_file_resonse(string filename) {
    struct stat fstat;
    stat(filename.c_str(), &fstat);
    char* buffer = new char[fstat.st_size+1];
    memset(buffer, 0, sizeof(char)*(fstat.st_size+1));

    std::ifstream fin(filename, std::ios::in);
    fin.read(buffer, fstat.st_size);

    string fcontent(buffer);
    delete[] buffer;
    return fcontent;
}
string Responsor::retr_404() {
    string response;
    struct stat fstat;
    string filepath = this->root+"/404.html";
    stat(filepath.c_str(), &fstat);
    response = this->compose_head(0, &fstat, 404);
    response += this->static_file_resonse(filepath);
    return response;
}

string Responsor::execute_cgi(Request* request){
    char* buf = new char[4096];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;


    /* 建立管道*/
    pipe(cgi_output);
    /*建立管道*/
    pipe(cgi_input);

    pid = fork();
    if (pid == 0){
        //child
        /* 把 STDOUT 重定向到 cgi_output 的写入端 */
        dup2(cgi_output[1], 1);
        /* 把 STDIN 重定向到 cgi_input 的读取端 */
        dup2(cgi_input[0], 0);
        /* 关闭 cgi_input 的写入端 和 cgi_output 的读取端 */
        close(cgi_output[0]);
        close(cgi_input[1]);

        /*用 execl 运行 cgi 程序*/
        string cgi = this->root+"/cgi"+request->get_cgi()+".out";
        char length[10]={0};
        sprintf(length, "%ld", request->get_para_s().length());
        execl(cgi.c_str(), cgi.c_str(), length, NULL);
        exit(0);
    } else {
        /* parent */
        /* 关闭 cgi_input 的读取端 和 cgi_output 的写入端 */
        close(cgi_output[1]);
        close(cgi_input[0]);

        string para = request->get_para_s();
        printf("\n\n%s", para.c_str());
        write(cgi_input[1], para.c_str(), para.length());

        /*读取 cgi_output 的管道输出到客户端，该管道输入是 STDOUT */
        read(cgi_output[0], buf, 4096);
        /*关闭管道*/
        close(cgi_output[0]);
        close(cgi_input[1]);
        /*等待子进程*/
        waitpid(pid, &status, 0);
        printf("%s", buf);
        return string(buf);
    }
}