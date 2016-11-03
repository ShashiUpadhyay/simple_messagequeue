extern "C" {
#include "MQ.h"
}
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {

    int rv;
    cs550_MQ mq;

    // rv = unlink("/tmp/mq");

    rv = cs550_MQOpen(&mq, "/import/linux/home/supadhy2/cs550/cs550_assignment3/testcases/simple_mq_examples/test20.file", 5); assert(rv == 0);

    for (int i = 0; i < 10; i++) {
        char buf[100];
        sprintf(buf, "Hello %d from process %d!", i,
         (int) getpid());
        rv = cs550_MQSend(&mq, buf, strlen(buf));
        sleep(1);
    }

    rv = cs550_MQClose(&mq); assert(rv == 0);
}
