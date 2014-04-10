//
//  HTTPDirector.h
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 2/5/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#ifndef __WebServerFirst__HTTPDirector__
#define __WebServerFirst__HTTPDirector__

#include <iostream>

#include "HTTPContentToSend.h"

enum sendType {
    attachment = 0,
    http = 1,
    error404 = 2,
    standard = 3
};

class HTTPDirector {
public:
    std::string fileName = "standardName";
    std::string content = "Standard Message";
    std::string contentType = "text/html";
    sendType fileSend = standard;
    
    void construct(class HTTPContent* http);
    HTTPDirector();
};



#endif /* defined(__WebServerFirst__HTTPDirector__) */
