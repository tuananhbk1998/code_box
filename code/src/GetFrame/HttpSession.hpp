//
//  HttpSession.hpp
//  MonitorService
//
//  Created by Pham Quang Hoa on 06/05/2020.
//  Copyright © 2020 Pham Quang Hoa. All rights reserved.
//

#ifndef HttpSession_hpp
#define HttpSession_hpp

#include <string>
#include <curl/curl.h>
#include <functional>

class HttpSession {
public:
    HttpSession();
    virtual ~HttpSession();
    void setUrl(const std::string& url);
    void setMethod(const std::string& method);
    void addHeader(const std::string& header);
    void setPostString(const std::string& content);
    void setPostData(const void* data, size_t length);
    void setDoneFunc(std::function<void(HttpSession*)> f);
    void setUserPassword(std::string author, bool isBasic);
    bool start();
    
    void* getData();
    size_t getSize();
    int getHttpStatus();

protected:
    CURL* handle;
    struct curl_slist *list;
    std::string method;
    std::string postString;
    std::function<void(HttpSession*)> doneFunc;
    uint8_t* data;
    size_t size;
    int httpStatus;
    
private:
    static size_t writecb(void* data, size_t size, size_t nmemb, void* ptr);
};

#endif /* HttpSession_hpp */
