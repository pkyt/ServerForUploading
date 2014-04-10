//
//  HTTPContentToSend.cpp
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 2/4/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#include "HTTPContentToSend.h"
#include <string>

void HTTPContent::constructContentOfHTTP(std::string content){
    contentOfHTTP = "<h1>" + content + "<h1>";
}

void HTTPContent::constructAttachment(std::string fileName){
    attachment = "\r\nContent-disposition: attachment; filename=" + fileName;
}
void HTTPContent::constructContentType(std::string type){
    contentType = "\r\nContent-type: " + type;
}
void HTTPContent::constructWhatToSendErrorNotFound(){
    whatToSend = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nERROR 404: Not Found";
}
void HTTPContent::constructWhatToSendHTTP(){
    whatToSend = "HTTP/1.1 200 Okay" + contentType + "\r\n\r\n" + contentOfHTTP;
}
void HTTPContent::constructWhatToSendAttachemnt(){
    whatToSend = "HTTP/1.1 200 Okay" + attachment + contentType + "\r\n\r\n";
}

std::string HTTPContent::getWhatToSend(){
    return whatToSend;
}

