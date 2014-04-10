//
//  HTTPContentToSend.h
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 2/4/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#ifndef __WebServerFirst__HTTPContentToSend__
#define __WebServerFirst__HTTPContentToSend__

#include <iostream>
#include <string>

#include "HTTPDirector.h"



class HTTPContent {
    std::string contentOfHTTP;
    std::string attachment;
    std::string contentType;
    std::string whatToSend;
    
public:
    void constructContentOfHTTP(std::string content);
    void constructAttachment(std::string fileName);
    void constructContentType(std::string type);
    void constructWhatToSendErrorNotFound();
    void constructWhatToSendHTTP();
    void constructWhatToSendAttachemnt();
    std::string getWhatToSend();
};



#endif /* defined(__WebServerFirst__HTTPContentToSend__) */
