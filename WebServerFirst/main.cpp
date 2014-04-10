//
//  main.cpp
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 1/17/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

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
#include <map>

#include "Queue.h"
#include "HTTPContentToSend.h"
#include "HTTPDirector.h"
#include "Contacts.h"

using namespace std;

#define BACKLOG 10

struct Task {
    int sock; // socket where to send
    std::vector<char> recvMsg; // received message
    Task(int s, std::vector<char> msg);
};

Task::Task(int s, std::vector<char> msg){
    sock = s;
    recvMsg = msg;
}

struct Queue<int> taskList; // Queue that keeps all tasks

void sendFileContent(int sock,string fileName){
    
    ifstream is;
    is.open (fileName.c_str(), ios::binary );
    is.seekg (0, ios::end);
    long long fileSize = is.tellg();
    
    FILE * fp = fopen(&(fileName[0]), "r");
    long sizeCheck = 0;
    char mfcc[1025];
    while (sizeCheck + 1024 <= fileSize){ // sending data by 1024 bytes
        size_t read = ::fread(mfcc, sizeof(char), 1024, fp);
        long sent = send(sock, mfcc, read, 0);
        if(sent == -1){
            perror("sent");
            break;
        }
        sizeCheck += sent;
    }
    if(fileSize - sizeCheck > 0){ // send rest of data
        char mfccPart [fileSize - sizeCheck + 1];
        ::fread(mfccPart, sizeof(char), fileSize - sizeCheck, fp);
        long sent = send(sock, mfccPart, fileSize - sizeCheck, 0);
        if(sent == -1){
            perror("sent");
        }
    }
    fclose(fp);
}

string getFileName(string fullMsg){
    size_t found = fullMsg.find("filename");
    string fileName;
    if (found!=string::npos) {
        int i = 0;
        int countChar = 0;
        while (true) {
            if (i > 250) {
                // Show error cause filename is is too long or doesn't exist
                break;
            }
            if (fullMsg[found+i] == '"') {
                countChar++;
                if (countChar == 2) {
                    break;
                }
            }else{
                if (countChar == 1) {
                    fileName+=fullMsg[found+i];
                }
            }
            i++;
        }
        cout << "FILENAME: " << fileName << endl;
    }else{
        return "";
    }
    return fileName;
}

int contentStarts(string msg){
    int count = 0;
    for (int i = 0 ; i < msg.length()-2; i++) {
        if (msg[i] == '\n') {
            if (count == 3) {
                return i+1;
            }
            count++;
        }
    }
    return -1;
}

string parseBoundary(string fullMsg){
    size_t boundaryBegin = fullMsg.find("boundary=");
    string boundary;
    if (boundaryBegin != string::npos) {
        int i = 9;
        while (fullMsg[boundaryBegin+i]!= '\r') {
            boundary+=fullMsg[boundaryBegin+i];
            i++;
        }
    }
    return boundary;
}

string getContent(string fullMsg, string boundary){
    size_t boundaryBegin = fullMsg.find(boundary);
    string rest = fullMsg.substr(boundaryBegin+boundary.length());
    size_t boundaryFirst = rest.find(boundary);
    string boundaryStart = rest.substr(boundaryFirst);
    int cs = contentStarts(boundaryStart);
        
    string contentWithWasteEnd = boundaryStart.substr(cs);
        
    size_t endOfContent = contentWithWasteEnd.find(boundary);
    if (endOfContent == string::npos) {
        return contentWithWasteEnd;
    }
    string content = contentWithWasteEnd.substr(0, endOfContent-4);
    return content;
}
/*
void handleGOODMsg(string filename, string boundary, vector<char> recvMsg){
    
}*/

#define MAX_PATH 1024

void* doTask(void* q){
    while (true) {
        int sock = taskList.pop();
        cout << "message recieved from socket " << sock << endl;
        vector<char>recvMsg;
        int sizeRecvMessage = 1024;
        long len = sizeRecvMessage;
        while(len == 1024){
            char recvMessage[sizeRecvMessage];
            len = recv(sock, recvMessage, sizeRecvMessage, 0);
            recvMsg.insert(recvMsg.end(), recvMessage, recvMessage+len);
            if(len == -1){
                cerr << "ERROR: failed on receiving" << endl;
                exit(1);
            }
        }
        for (int i = 0; i < recvMsg.size(); i++) {
            cout << recvMsg[i];
        }
        cout << endl;
        string fullMsg = &(recvMsg[0]);
        vector<char> recvMsgCopy = recvMsg;
        if(len != 0){
            
            char * pch;
            pch = strtok(&(recvMsg[0]), "/");
            pch = strtok(NULL, " \n"); // Now pch correspond to the specidic data (path to data) a client needs
            if (pch == NULL){ // if no data needed send standard message
            }else{
                std::string testCompWhat (pch, pch+7);
                std::string testCompTo = "HTTP/1.";
                if (!strcmp(&(testCompWhat[0]), &(testCompTo[0]))){ // no file requested
                    sendFileContent(sock, "mainPage.html");
                }else{
                    // save file
                    string fileName = getFileName(fullMsg);
                    string boundary = parseBoundary(fullMsg);
                    while ((fileName.length() == 0) || (boundary.length() == 0)) { // not fully recv
                        recvMsg = recvMsgCopy;
                        int sizeRecvMessage = 1024;
                        long len = sizeRecvMessage;
                        while(len == 1024){
                            char recvMessage[sizeRecvMessage];
                            len = recv(sock, recvMessage, sizeRecvMessage, 0);
                            recvMsg.insert(recvMsg.end(), recvMessage, recvMessage+len);
                            if(len == -1){
                                cerr << "ERROR: failed on receiving" << endl;
                                exit(1);
                            }
                        }
                        fileName = getFileName(fullMsg);
                        boundary = parseBoundary(fullMsg);
                    }
                    ofstream outputFile(fileName);
                    outputFile << getContent(fullMsg, boundary);
                    
                    if (fullMsg.length() < recvMsgCopy.size()) {
                        for (unsigned long i = fullMsg.length();  i < recvMsgCopy.size()-4; i++) {
                            if (recvMsgCopy[i+3] == boundary[0]) {
                                bool eql = true;
                                for (int j = 0; j < boundary.length(); j++) {
                                    if (recvMsgCopy[i+j] != boundary[j]) {
                                        eql = false;
                                        break;
                                    }
                                }
                                if (eql) {
                                    break;
                                }
                            }
                            outputFile << recvMsgCopy[i];
                        }
                    }
                }
            }
        }else{

        }
        close(sock);
        cout << "message sent" << endl;
    }
    return NULL;
}

void* consoleSend(void*q){
    while (true) {
        string response;
        int sock;
        cout << "\nenter sock #:";
        cin >> sock;
        cout << "\nenter who are you:";
        string Iam;
        cin >> Iam;
        cout << "\nenter message:";
        string msg;
        cin >> msg;
        cout << endl;
        response = "snd:" + Iam + "\n" + msg;
        if (send(sock, &(response[0]), response.length(), 0))
            perror("send");
        
    }
}

int main(int argc, const char * argv[])
{
    std::cout << "Web Server Started.\n";
    
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *res;
    int status, sockfd;
    
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    status = getaddrinfo(NULL, "3490", &hints, &res);
    if (status != 0){
        std::cerr << "ERROR: unsuccessful getaddrinfo return" << std::endl;
        exit(1);
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1){
        cerr << "ERROR: socket() failed\n";
        exit(1);
    }
    if(::bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
        close(sockfd);
        perror("server: bind");
        exit(1);
    }
    
    freeaddrinfo(res);
    
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    int numConsumers = 20;
    pthread_t consumers[numConsumers];
    
    for(int i = 0; i < numConsumers; i++){
        // creating consumer's thread that will respond to requests
        pthread_create(&consumers[i], NULL, &doTask, NULL);
    }
    
    while (true) {
        addr_size = sizeof their_addr;
        int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd == -1){
            cerr << "ERROR: by accept()\n";
            exit(1);
        }
        taskList.push(new_fd);
    }
    
    return 0;
}

