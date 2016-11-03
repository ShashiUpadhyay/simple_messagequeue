extern "C" {
#include "MQ.h"
}
#include <assert.h>
#include <stdio.h>

int main() {

    int rv;
    cs550_MQ mq;

    // rv = unlink("/tmp/mq");

    rv = cs550_MQOpen(&mq, "/import/linux/home/supadhy2/cs550/cs550_assignment3/testcases/simple_mq_examples/test20.file", 5); assert(rv == 0);

    while (true) {
        char msg[1024];
        rv = cs550_MQRecv(&mq, msg, 1024); assert(rv >= 0);

        msg[rv] = '\0';
        printf("%s\n", msg);
    }

    rv = cs550_MQClose(&mq); assert(rv == 0);
}
