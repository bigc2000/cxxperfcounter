#ifndef LIBEVENT_HTTP_SERVER_H
#define LIBEVENT_HTTP_SERVER_H
#include <memory.h>
#include <malloc.h>
#if defined(WIN32)
#include <winsock2.h>
#ifndef THREAD_ID
#define THREAD_ID uintptr_t
#endif

#else
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/queue.h>

#ifndef THREAD_ID
#define THREAD_ID pthread_t
#endif

#endif

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/keyvalq_struct.h>


struct mc_http_worker;
struct evhttp_request;
typedef void (*mc_http_callback)(struct evhttp_request *, void *);

typedef struct mc_http_url_mapping {
    const char *path; /*start with '/' */
    mc_http_callback cb;
    //struct mc_http_url_mapping *next;
    SLIST_ENTRY(mc_http_url_mapping) next;

} mc_http_url_mapping_t;


typedef struct mc_http_event_config {
    int worker_count; /*thread count*/
    unsigned short timeout_secs;
    unsigned short port;
    char ip[12];
    //struct evhttp_cb* callbacks;
    SLIST_HEAD(httcb, mc_http_url_mapping) url_mapping;
} mc_http_event_config_t, *mc_http_event_config_ptr;

typedef struct mc_http_server {
    mc_http_event_config_t *http_event_config;
    // socket
    evutil_socket_t fd;
    // worker thread pool
    struct mc_http_worker **worker_pool;
} mc_http_server_t;


typedef struct mc_http_worker {
    THREAD_ID thread_id;
    struct event_base *evbase;
    struct evhttp *http;
    evutil_socket_t fd;//server socket fd
    mc_http_event_config_t *http_event_config;
} mc_http_worker_t;

mc_http_event_config_t *mc_http_event_config_new();

void mc_http_event_config_free(mc_http_event_config_t **pp_config);

mc_http_server_t *mc_http_server_new(mc_http_event_config_t *config);

void mc_http_server_free(mc_http_server_t **server);

void mc_start_http_server(mc_http_server_t *server);

mc_http_url_mapping_t *mc_http_url_mapping_new();

void mc_http_url_mapping_free(mc_http_url_mapping_t **);

#endif //HEADER