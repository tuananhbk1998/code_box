//
//  HttpSession.cpp
//  MonitorService
//
//  Created by Pham Quang Hoa on 06/05/2020.
//  Copyright © 2020 Pham Quang Hoa. All rights reserved.
//

#include <string.h>

#include "HttpSession.hpp"

HttpSession::HttpSession() {
    this->handle = curl_easy_init();
    this->list = NULL;
    this->method = "GET";
    this->data = NULL;
    this->size = 0;
    this->httpStatus = -1;
    
    curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, HttpSession::writecb);
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(this->handle, CURLOPT_PRIVATE, this);
    curl_easy_setopt(this->handle, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(this->handle, CURLOPT_LOW_SPEED_TIME, 4L);
    curl_easy_setopt(this->handle, CURLOPT_LOW_SPEED_LIMIT, 30L);
}

HttpSession::~HttpSession() {
    curl_easy_cleanup(this->handle);
    
    if (this->list != NULL) {
        curl_slist_free_all(list);
    }
    
    if (this->data != NULL) {
        free(this->data);
    }
}

void HttpSession::setUrl(const std::string &url) {
    curl_easy_setopt(this->handle, CURLOPT_URL, url.c_str());
}

void HttpSession::setMethod(const std::string& method) {
    this->method = method;
}

void HttpSession::addHeader(const std::string& header) {
    this->list = curl_slist_append(this->list, header.c_str());
}

void HttpSession::setPostString(const std::string &content) {
    this->postString = content;
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, this->postString.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDSIZE, this->postString.size());
}

void HttpSession::setPostData(const void* data, size_t length) {
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDSIZE, length);
}
void HttpSession::setUserPassword(std::string author, bool isBasic) 
{
    curl_easy_setopt(this->handle, CURLOPT_USERPWD, author.c_str());
    if (isBasic) {
        curl_easy_setopt(this->handle, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
    }
    else {
        curl_easy_setopt(this->handle, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
    }
}
bool HttpSession::start() {
    CURLcode res;
    
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->list);
    curl_easy_setopt(this->handle, CURLOPT_SSL_VERIFYPEER, 0);
    
    if (this->method != "") {
        curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, this->method.c_str());
    }
    
    res = curl_easy_perform(this->handle);
    
    curl_easy_getinfo(this->handle, CURLINFO_RESPONSE_CODE, &this->httpStatus);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return false;
    }
    return true;
}

void* HttpSession::getData() {
    return this->data;
}

size_t HttpSession::getSize() {
    return this->size;
}

int HttpSession::getHttpStatus() {
    return this->httpStatus;
}

size_t HttpSession::writecb(void* data, size_t size, size_t nmemb, void* ptr) {
    HttpSession* session = (HttpSession*)ptr;
    size_t length = size * nmemb;
    size_t new_size = session->size + length;
    uint8_t* new_ptr = (uint8_t*)realloc(session->data, new_size);
    if (new_ptr != NULL) {
        session->data = new_ptr;
        memcpy(session->data + session->size, data, length);
        session->size = new_size;
    }
    return length;
}

void HttpSession::setDoneFunc(std::function<void (HttpSession *)> f) {
    this->doneFunc = f;
}