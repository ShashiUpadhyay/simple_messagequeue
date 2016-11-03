#ifndef CS550_MQ_H
#define CS550_MQ_H



#include <unistd.h>
#include <stddef.h>
#include <pthread.h>



struct Msg {
    size_t size;
    char buf[100];
};
struct Info {
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
    size_t n, in, out;
    struct Msg msgs[5];
};
struct cs550_MQ {
    struct Info *infop;
};

int cs550_MQOpen(struct cs550_MQ *, const char *path, int queue_length);
int cs550_MQClose(struct cs550_MQ *);
int cs550_MQSend(struct cs550_MQ *, const char *msg, size_t size);
ssize_t cs550_MQRecv(struct cs550_MQ *, char *msg, size_t size);
ssize_t cs550_MQPeek(struct cs550_MQ *);
int cs550_MQNotify(struct cs550_MQ *, void (*cb)(const char *msg, size_t sz));



#endif
