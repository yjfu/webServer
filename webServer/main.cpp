//
//  main.cpp
//  webServer
//
//  Created by 傅永健 on 2017/5/21.
//  Copyright (c) 2017 傅永健. All rights reserved.
//

#include "Server.h"

int main(int argc, const char * argv[]) {
    string src_root = "/Users/yjfu/github/webServer/webSrc";
    Server* server = new Server("127.0.0.1", 8080, src_root);
    server->run();
}