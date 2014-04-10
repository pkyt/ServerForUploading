//
//  Contacts.h
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 3/7/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#ifndef __WebServerFirst__Contacts__
#define __WebServerFirst__Contacts__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <list>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

struct Contact {
    int socketID;
    std::string nickName;
    std::string password;
    Contact(int sock, std::string nickname, std::string pass){
        socketID = sock;
        Contact::nickName = nickname;
        password = pass;
    }
};

class Contacts {
    std::vector<Contact> _contacts;
    pthread_mutex_t _lock;
    
public:
    Contacts(){
        pthread_mutex_init(&_lock, NULL);
    }
    ~Contacts(){
        pthread_mutex_destroy(&_lock);
    }
    std::vector<std::string> getAllUsers();
    int getSocketID(std::string nickName); // -1 if nickname doesn't exist
    std::string pushContact(int socketID, std::string nickName, std::string pass);
    std::string change(int socket, std::string nickName, std::string pass);
    void logout(std::string nickName);
    void logoutOfEveryUserWithSock(int sock);
};

#endif /* defined(__WebServerFirst__Contacts__) */
