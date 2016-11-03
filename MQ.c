#include "MQ.h"
 #include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>

int
cs550_MQOpen(struct cs550_MQ *mqp, const char *path, int qlen) {

    int rv;

    if (access(path, F_OK) != 0) {
        int en = errno;
        assert(en == ENOENT);

        int fd = open(path, O_RDWR|O_CREAT|O_EXCL, 0600);
        if (fd < 0) {
            return -1;
        }

        rv = ftruncate(fd, 1024); assert(rv == 0);

        void *vp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        assert(vp != MAP_FAILED);

        rv = close(fd); assert(rv == 0);

        mqp->infop = vp;

        {
            pthread_mutexattr_t attr;
            rv = pthread_mutexattr_init(&attr); assert(rv == 0);
            rv = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); assert(rv == 0);
            rv = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); assert(rv == 0);
            rv = pthread_mutex_init(&mqp->infop->mutex, &attr); assert(rv == 0);
            rv = pthread_mutexattr_destroy(&attr); assert(rv == 0);
        }

        {
            pthread_condattr_t attr;
            rv = pthread_condattr_init(&attr); assert(rv == 0);
            rv = pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); assert(rv == 0);
            rv = pthread_cond_init(&mqp->infop->condvar, &attr); assert(rv == 0);
            rv = pthread_condattr_destroy(&attr); assert(rv == 0);
        }

        mqp->infop->n = 0;
        mqp->infop->in = 0;
        mqp->infop->out = 0;

    } else {

        int fd = open(path, O_RDWR);
        if (fd < 0) {
            return -1;
        }

        void *vp = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        assert(vp != MAP_FAILED);

        rv = close(fd); assert(rv == 0);

        mqp->infop = vp;
    }

    return 0;
}

int
cs550_MQSend(struct cs550_MQ *mq, const char *msg, size_t msg_size) {

    int rv;

    struct Info *ip = mq->infop;

    rv = pthread_mutex_lock(&ip->mutex); assert(rv == 0);

    fprintf(stderr, "Send: On queue: %zu\n", ip->n);

    while (ip->n == 5) {
        rv = pthread_cond_wait(&ip->condvar, &ip->mutex); assert(rv == 0);
    }

    memcpy(ip->msgs[ip->in].buf, msg, msg_size);
    ip->msgs[ip->in].size = msg_size;
    ip->in = (ip->in + 1)%10;
    ip->n++;

    fprintf(stderr, "Send: On queue: %zu\n", ip->n);

    rv = pthread_mutex_unlock(&ip->mutex); assert(rv == 0);

    rv = pthread_cond_broadcast(&ip->condvar); assert(rv == 0);

    return 0;
}

ssize_t
cs550_MQRecv(struct cs550_MQ *mq, char *msg, size_t buf_size) {

    int rv;

    struct Info *ip = mq->infop;

    rv = pthread_mutex_lock(&ip->mutex); assert(rv == 0);

    fprintf(stderr, "Recv: On queue: %zu\n", ip->n);

    while (ip->n == 0) {
        rv = pthread_cond_wait(&ip->condvar, &ip->mutex); assert(rv == 0);
    }

    memcpy(msg, ip->msgs[ip->out].buf, ip->msgs[ip->out].size);
    size_t sz = ip->msgs[ip->out].size;
    ip->out = (ip->out + 1)%5;
    ip->n--;

    fprintf(stderr, "Recv: On queue: %zu\n", ip->n);

    rv = pthread_mutex_unlock(&ip->mutex); assert(rv == 0);

    rv = pthread_cond_broadcast(&ip->condvar); assert(rv == 0);

    return sz;
}

ssize_t
cs550_MQPeek(struct cs550_MQ *mq) {

    /*
    int rv;

    struct Info *ip = mq->infop;

    rv = pthread_mutex_lock(&ip->mutex); assert(rv == 0);

    fprintf(stderr, "%zu\n", ip->n);
    while (count > 0) {
        pthread_cond_wait(&ip->condvar, &ip->mutex);
    }

    rv = pthread_mutex_unlock(&ip->mutex); assert(rv == 0);
    */

    return -1;
}

int
cs550_MQClose(struct cs550_MQ *mq) {
    return -1;
}
