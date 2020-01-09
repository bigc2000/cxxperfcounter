#include "LibeventHttpServer.h"
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <stdlib.h>

static int start_http_worker(mc_http_worker_t *worker);

static mc_http_worker_t *mc_http_worker_new(mc_http_event_config_t *);

static void mc_http_worker_free(mc_http_worker_t **ppworker);

static void *worker_main(void *arg);

static evutil_socket_t init_server_socket(mc_http_server_t *server) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    const char *ip = server->http_event_config->ip;
    unsigned short port = server->http_event_config->port;
    addr.sin_port = htons(port);
    memset(&(addr.sin_addr), 0, sizeof(addr.sin_addr));
    addr.sin_addr.s_addr = inet_addr(ip);

    evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        fprintf(stderr, "Could not create socket.\n");
        return -1;
    }

    int ret = 0;
    int enable = 1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &enable, sizeof(enable));
    if (ret < 0) {
        fprintf(stderr, "Could not set up TCP_NODELAY to socket.\n");
        return -1;
    }

    ret = evutil_make_socket_nonblocking(fd);
    if (ret < 0) {
        perror("Could not make socket nonblocking.\n");
        return -1;
    }

    ret = evutil_make_listen_socket_reuseable(fd);

    if (ret < 0) {
        perror("Could not set listen to socket.\n");
        evutil_closesocket(fd);
        return -1;
    }

    bind(fd,(struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        perror("Could not bind to socket.\n");
        evutil_closesocket(fd);
        return -1;
    }
    listen(fd,SOMAXCONN);

    return fd;
}

mc_http_server_t *mc_http_server_new(mc_http_event_config_t *config) {
    int i;
    mc_http_server_t *server = (mc_http_server_t *) malloc(sizeof(mc_http_server_t));
    server->http_event_config = config;
    server->fd = init_server_socket(server);
    server->worker_pool = (mc_http_worker_t **) calloc(server->http_event_config->worker_count,
                                                       sizeof(mc_http_worker_t *));
    for (i = 0; i < server->http_event_config->worker_count; i++) {
        server->worker_pool[i] = mc_http_worker_new(config);
        server->worker_pool[i]->fd = server->fd;
    }
    return server;
}

void mc_http_server_free(mc_http_server_t **pp_server) {
    int i;
    mc_http_server_t *server;
    if (pp_server != NULL) {
        server = *pp_server;
        if (server != NULL && server->http_event_config->worker_count > 0) {
            for (i = 0; i < server->http_event_config->worker_count; i++) {
                mc_http_worker_free(&(server->worker_pool[i]));
                server->worker_pool[i] = NULL;
            }
            free(server->worker_pool);
            server->worker_pool = NULL;
        }
        free(server);
        *pp_server = NULL;
    }
}

void mc_start_http_server(mc_http_server_t *server) {
    int i;
    if (server->http_event_config->worker_count <= 0) {
        fprintf(stderr, "work thread number must be greater than 0 ");
        exit(-1);
    }
    for (i = 0; i < server->http_event_config->worker_count; i++) {
        start_http_worker(server->worker_pool[i]);
    }
}


static void http_request_default(struct evhttp_request *req, void *arg) {
    fprintf(stderr, "http request for test only\n");
}

/*for each worker thread main*/
void *worker_main(void *arg) {
    int ret = 0;
    mc_http_worker_t *worker = (mc_http_worker_t *) arg;
    if (worker == NULL) {
        fprintf(stderr, "invalid arg.\n");
        return NULL;
    }

    if (worker->fd < 0) {
        fprintf(stderr, "invalid socket fd.\n");
        return NULL;
    }

    worker->evbase = event_base_new();
    if (worker->evbase == NULL) {
        fprintf(stderr, "Could not create an event_base.\n");
        return NULL;
    }
    worker->http = evhttp_new(worker->evbase);
    if (worker->http == NULL) {
        fprintf(stderr, "Could not create an evhttp.\n");
        return NULL;
    }
    //ret = evhttp_bind_socket(worker->http,worker->http_event_config->ip,worker->http_event_config->port);
    ret = evhttp_accept_socket(worker->http, worker->fd);
    if (ret != 0) {
        fprintf(stderr, "Could not accept socket.%d\n", worker->fd);
        return NULL;
    }
    if (worker->http_event_config == NULL) {
        fprintf(stderr, "config must not be NULL,shared instance.\n");
        return NULL;
    }

    evhttp_set_timeout(worker->http, worker->http_event_config->timeout_secs);

    if (SLIST_FIRST(&(worker->http_event_config->url_mapping)) == NULL) {
        fprintf(stderr, "[WARNING] http request call back set none. add default handler");
        evhttp_set_cb(worker->http, "/", http_request_default, (void *) NULL);
    } else {
        mc_http_url_mapping_t *http_url_mapping = NULL;
        SLIST_FOREACH(http_url_mapping, &(worker->http_event_config->url_mapping), next) {
            evhttp_set_cb(worker->http, http_url_mapping->path, http_url_mapping->cb, (void *) worker);
        }
    }
    ret = event_base_dispatch(worker->evbase);
    if (ret != 0) {
        fprintf(stderr, "Could not dispatch base of worker.\n");
        return NULL;
    }
    return NULL;
}

int start_http_worker(mc_http_worker_t *worker) {
    int ret = 0;
    pthread_attr_t attr;
    ret = pthread_attr_init(&attr);
    if (ret != 0) {
        fprintf(stderr, "Could not initialize attr of worker.\n");
        return ret;
    }

    ret = pthread_attr_setstacksize(&attr, 256 * 1024);
    if (ret != 0) {
        fprintf(stderr, "Could not set stack size of worker.\n");
        return ret;
    }


    // set up thread schedule properties
    //struct sched_param param;
    //param.sched_priority = 90;
    //pthread_attr_setschedpolicy(&attr, SCHED_RR);
    //pthread_attr_setschedparam(&attr, &param);
    //pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    ret = pthread_create(&worker->thread_id, &attr, worker_main, worker);
    //ret = pthread_create(&worker->td, NULL, worker_main, worker);
    if (ret != 0) {
        fprintf(stderr, "Could not create worker.\n");
        return ret;
    }

    ret = pthread_attr_destroy(&attr);
    if (ret != 0) {
        fprintf(stderr, "Could not destroy attr of worker.\n");
        return ret;
    }
    return ret;
}

mc_http_worker_t *mc_http_worker_new(mc_http_event_config_t *config) {
    mc_http_worker_t *worker = (mc_http_worker_t *) malloc(sizeof(mc_http_worker_t));
    worker->http_event_config = config;
    return worker;
}

void mc_http_worker_free(mc_http_worker_t **ppworker) {
    int ret = 0;
    void *res;
    mc_http_worker_t *worker;
    if (ppworker == NULL || *ppworker == NULL) {
        return;
    }
    worker = *ppworker;
    ret = pthread_cancel(worker->thread_id);
    if (ret != 0) {
        fprintf(stderr, "Could not cancel worker.\n");
    }

    ret = pthread_join(worker->thread_id, &res);
    if (ret != 0) {
        fprintf(stderr, "Could not join worker.\n");
    }
    evhttp_free(worker->http);
    event_base_free(worker->evbase);
    free(worker);
}

mc_http_url_mapping_t *mc_http_url_mapping_new() {
    mc_http_url_mapping_t *url_mapping = (mc_http_url_mapping_t *) malloc(sizeof(mc_http_url_mapping_t));
    memset(url_mapping, 0, sizeof(mc_http_url_mapping_t));
    return url_mapping;
}

void mc_http_url_mapping_free(mc_http_url_mapping_t **pphttp_url_mapping) {
    if (pphttp_url_mapping == NULL || *pphttp_url_mapping == NULL) {
        return;
    }
    free(*pphttp_url_mapping);
}

mc_http_event_config_t *mc_http_event_config_new() {
    mc_http_event_config_t *config = (mc_http_event_config_t *) malloc(sizeof(mc_http_event_config_t));
    memset(config, 0, sizeof(mc_http_event_config_t));
    return config;
}

void mc_http_event_config_free(mc_http_event_config_t **pp_config) {
    if (pp_config != NULL) {
        free(*pp_config);
        memset(*pp_config, 0, sizeof(mc_http_event_config_t));
        *pp_config = NULL;
    }
}

