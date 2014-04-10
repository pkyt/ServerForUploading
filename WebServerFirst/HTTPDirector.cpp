//
//  HTTPDirector.cpp
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 2/5/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#include "HTTPDirector.h"
#include "HTTPContentToSend.h"

HTTPDirector::HTTPDirector(){
    fileName = "standardName";
    content = "Standard Message";
    contentType = "text/html";
    fileSend = standard;
}

void HTTPDirector::construct(struct HTTPContent* httpc){
    if (fileSend == attachment){
        httpc->constructAttachment(fileName);
        httpc->constructContentType(contentType);
        httpc->constructWhatToSendAttachemnt();
    }else if (fileSend == http){
        httpc->constructContentType(contentType);
        httpc->constructContentOfHTTP(content);
        httpc->constructWhatToSendHTTP();
    }else if (fileSend == error404){
        httpc->constructWhatToSendErrorNotFound();
    }else{
        httpc->constructContentType(contentType);
        httpc->constructContentOfHTTP(content);
        httpc->constructWhatToSendHTTP();
    }
}