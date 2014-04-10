//
//  Contacts.cpp
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 3/7/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#include "Contacts.h"
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

int Contacts::getSocketID(std::string nickName){
    pthread_mutex_lock(&_lock);
    for (int i = 0; i < _contacts.size(); i++) {
        Contact curr = _contacts[i];
        if (!std::strcmp(&(curr.nickName[0]), &(nickName[0]))) {
            pthread_mutex_unlock(&_lock);
            return curr.socketID;
        }
    }
    pthread_mutex_unlock(&_lock);
    return -2;
}

std::vector<std::string> Contacts::getAllUsers(){
    pthread_mutex_lock(&_lock);
    std::vector<std::string> result;
    for (int i = 0; i < _contacts.size(); i++) {
        result.push_back(_contacts[i].nickName);
    }
    pthread_mutex_unlock(&_lock);
    return result;
}

std::string Contacts::pushContact(int socketID, std::string nickName, std::string pass){
    if(getSocketID(nickName) == -2){
        pthread_mutex_lock(&_lock);
        Contact newContact (socketID, nickName, pass);
        _contacts.push_back(newContact);
        pthread_mutex_unlock(&_lock);
        return "success registation";
    }else{
        return "iam:failed";
    }
}

std::string Contacts::change(int socket, std::string nickName, std::string pass){
    pthread_mutex_lock(&_lock);
    for (int i = 0; i < _contacts.size(); i++) {
        Contact curr = _contacts[i];
        if (!std::strcmp(&(curr.nickName[0]), &(nickName[0]))) {
            if (!std::strcmp(&(curr.password[0]), &(pass[0]))) {
                if (curr.socketID != -1) {
                    pthread_mutex_unlock(&_lock);
                    return "user is in use, log out first";
                }
                curr.socketID = socket;
                pthread_mutex_unlock(&_lock);
                return "success login";
            }
            pthread_mutex_unlock(&_lock);
            return "inccorect password";
        }
    }
    pthread_mutex_unlock(&_lock);
    return "user doesn't exist";
}

void Contacts::logout(std::string nickName){
    pthread_mutex_lock(&_lock);
    for (int i = 0; i < _contacts.size(); i++) {
        Contact curr = _contacts[i];
        if (!std::strcmp(&(curr.nickName[0]), &(nickName[0]))) {
            _contacts[i].socketID = -1;
            break;
        }
    }
    pthread_mutex_unlock(&_lock);
}

void Contacts::logoutOfEveryUserWithSock(int sock){
    pthread_mutex_lock(&_lock);
    for (int i = 0; i < _contacts.size(); i++) {
        Contact curr = _contacts[i];
        if (curr.socketID == sock) {
            _contacts[i].socketID = -1;
        }
    }
    pthread_mutex_unlock(&_lock);
}

