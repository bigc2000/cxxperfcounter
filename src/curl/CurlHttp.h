//
// Created by wx on 18-9-10.
//


#ifndef CURL_HTTP_H
#define CURL_HTTP_H

#include <string>
#include <curl/curl.h>

/**
*@param curl a CURL handle,已经初始化过的CURL句柄
*@param to_ms timeout in millseconds,超时时间，单位毫秒
*@param http_url 发送请求的URL如http://www.xr.com
*@param http_request 请求内容如 {"msg":"hello world"}
*@param http_response 服务返回的内容 {"msg":"server said Hello"}
*/
int
curl_easy_http_post(CURL *curl, int to_ms, const char *http_url, const std::string &http_request, std::string *http_response);

extern CURLSH *share_handle;

void setDefaultCurlOpt(CURL *curl);

#endif // CURL_HTTP_H
