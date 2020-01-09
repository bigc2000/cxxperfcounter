
#include "CurlHttp.h"

#define  WARNING_LOG printf

CURLSH *share_handle = NULL;

// the call function for curl receive message from peer
// Input:
// data: the message received by curl, The size of the data pointed to by ptr is size multiplied with nmemb
// n_size: the size of the data pointed to by data is size multiplied with nmemb
// n_memb: the size of the data pointed to by data is size multiplied with nmemb
// user_buf: the buffer set by user through curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, userData)
// Output: return the number of bytes actually taken care of
// 在URL响应接收的过程中，只要收到一个数据包，这个函数就会被调用
// 在测试中，曾经出现http_batch_res_call_back被curl库连续回调两次，来接受同一个应答的两个数据包，所以这里做了对同一个应答包的拼装
static size_t http_batch_res_call_back(void *data, size_t n_size, size_t n_memb, void *user_buf);

/** Function: http_res_call_back()
* Description: the call function for curl receive message from peer
* Input:
*   @param data: the message received by curl, The size of the data pointed to by ptr is size multiplied with nmemb
*   @param n_size: the size of the data pointed to by data is size multiplied with nmemb
*   @param n_memb: the size of the data pointed to by data is size multiplied with nmemb
*   @param user_buf: the buffer set by user through curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, userData),TYPE = std::sting*
* Output:
*   @return the number of bytes actually taken care of
*/
size_t http_batch_res_call_back(void *data, size_t n_size, size_t n_memb, void *user_buf) {
  if (user_buf == NULL) {
    return 0;
  }
  std::string *page = (std::string *) user_buf;
  size_t real_size = n_size * n_memb;
  page->append((char *) data, real_size);
  return n_size * n_memb;
}

int curl_easy_http_get(CURL *curl_handle, int to_ms, const char *http_req_url, std::string *http_response) {
  if (curl_handle == NULL || http_req_url == NULL || http_response == NULL) {
    WARNING_LOG("The input paramter is illegal");
    return -1;
  }

  http_response->clear();
  CURLcode code;
  code = curl_easy_setopt(curl_handle, CURLOPT_URL, http_req_url);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));

    return code;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));

    return code;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, to_ms);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    return code;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_batch_res_call_back);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    return code;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) http_response);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    return code;
  }

  code = curl_easy_perform(curl_handle);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    return code;
  }

  return 0;
}


int curl_easy_http_post(CURL *curl_handle, int to_ms, const char *http_req_url, const std::string &http_req_post,
                        std::string *http_res) {
  if (curl_handle == NULL || http_req_url == NULL || http_res == NULL) {
    WARNING_LOG("The input paramter is illegal");
    return -1;
  }
  http_res->clear();

  struct curl_slist *header_list = NULL;
  CURLcode code;
  code = curl_easy_setopt(curl_handle, CURLOPT_URL, http_req_url);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }
  code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, http_req_post.c_str());
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }
  code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, http_req_post.size());
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, to_ms);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_batch_res_call_back);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }
  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) http_res);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }
  //#forbidden http1.1
  //header_list = curl_slist_append(header_list, "Expect:");
  //if(header_list == NULL) {
  //	fprintf(stderr,"curl perform error [%d]:[%s]",code,curl_easy_strerror(code));
  //	//WARNING_LOG("url[%s] could not set curl_slist_append Expect.", http_req_url->c_str());
  //	goto err_exit;
  //}
  header_list = curl_slist_append(header_list, "Content-Type:application/json; charset=UTF-8");


  code = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header_list);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
    goto err_exit;
  }

  code = curl_easy_perform(curl_handle);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));

    goto err_exit;
  }

  if (header_list != NULL) {
    curl_slist_free_all(header_list);
    header_list = NULL;
  }
  return 0;

  err_exit:
  if (header_list != NULL) {
    curl_slist_free_all(header_list);
    header_list = NULL;
  }
  return code;
}

void setDefaultCurlOpt(CURL *handle) {
  if (share_handle) {
    curl_easy_setopt(handle, CURLOPT_SHARE, share_handle);
  }
  CURLcode code = curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, -1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
  }
  code = curl_easy_setopt(handle, CURLOPT_TCP_NODELAY, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
  }
  code = curl_easy_setopt(handle, CURLOPT_POST, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
  }
  code = curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
  }
  code = curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
  if (code != CURLE_OK) {
    fprintf(stderr, "curl perform error [%d]:[%s]", code, curl_easy_strerror(code));
  }
}

