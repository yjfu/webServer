#ifndef CGI_H
#define CGI_H

#include <sys/stat.h>
#include <fstream>
#include "Request.h"
#include "Responsor.h"


string read_file(string filename){
    struct stat fstat;
    if(!stat(filename.c_str(), &fstat)){
        char* buff = new char[fstat.st_size+1];
        memset(buff, 0, sizeof(char)*(fstat.st_size+1));
        std::ifstream fin(filename.c_str(), std::ios::in);
        fin.read(buff, fstat.st_size);
        delete[] buff;
        return string(buff);
    }
    else
        return "";
}
string inline my_replace(string target, string sub, string to_replace){
    target.replace(target.find(sub), sub.length(), to_replace, 0);
    return target;
}
bool s2d(string s, double& d){
    if(!isnumber(s[0]))return 0;
    bool has_p = 0;
    for(int i = 1;i<s.length();i++)
        if(!isnumber(s[i])){
            if(has_p)return 0;
            else if(s[i]=='.')has_p = 1;
            else return 0;
        }

    if((d = atof(s.c_str())) == 0)
        if(s.length() == 0)return 0;

    return 1;
}
string add(Request* request){
    string path = "/Users/yjfu/github/webServer/webTemplate";
    map<string,string> para= request->get_parameters();
    string html;
    string as = para["a"], bs = para["b"];
    double add_res, a, b;
    if(s2d(as, a)&s2d(bs,b)){
        add_res = a+b;
        path += "/add.html";
        html = read_file(path);
        stringstream ss;
        string to_replace = para["a"]+"+"+para["b"]+"=";
        ss<<to_replace<<add_res;
        to_replace = ss.str();
        printf("%s\n",to_replace.c_str());
        html = my_replace(html, "{{content}}", to_replace);
    }
    else{
        path += "/cgi_err.html";
        html = read_file(path);
        html = my_replace(html, "{{cgi}}", request->get_cgi());
        html = my_replace(html, "{{err-name}}", "参数错误");
        html = my_replace(html, "{{err-info}}", "格式不正确或者填充不完整");
    }
    return html;



}
string to_lower(Request* request){
    string path = "/Users/yjfu/github/webServer/webTemplate";
    map<string, string> para = request->get_parameters();
    string s = para["text"];
    char* text = new char[s.length()+1];
    strcpy(text, s.c_str());
    int convert = 'a'-'A';
    for(int i = 0;i<s.length();i++){
        if(text[i]>='A'&&text[i]<='Z')
            text[i] += convert;
    }
    string news(text);
    path += "/tolower.html";
    string html = read_file(path);
    html = my_replace(html,"{{old}}", s);
    html = my_replace(html,"{{new}}", news);
    delete[]text;
    return html;
}
void enroll(Responsor* responsor){
    map<string, string(*)(Request*)> cgis;
    cgis["/add"] = add;
    cgis["/tolower"] = to_lower;
    responsor->set_cgi_list(cgis);
}

#endif