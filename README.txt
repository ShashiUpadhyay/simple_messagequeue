CS 550: Message Queue (Concurrency)

Due May 15th, 11:59 PM.

(This document was last modified on Friday, May 20, 2016 at 11:11:42 AM.)

For this assignment, you will implement an interprocess message queue. A short example follows:

#include "MQ.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// This is a callback that will be called everytime a message is sent.
void accept_msg(char *msg, size_t sz) {
  printf("Got message of size %zu\n", sz);
}

int main() {

  int rv;
  struct cs550_MQ mq;

  // Open port to the queue.
  rv = cs550_MQOpen(&mq, "/var/work/kchiu/mq_test", 5); assert(rv >= 0);

  char buf[1024];
  ... Fill message here.
  // Message sent to all open ports (excluding the sender of course).
  rv = cs550_MQSend(&mq, buf, 569); // Assuming the message is 569 bytes long.

  // Now recv a message.  This message would have to come from a different sender.
  ssize_t rv_ss = cs550_MQRecv(&mq, buf, 1024); // Max message length.
  printf("%zd\n", rv_ss); // Print actual size of message.

  // Now register to be notified.
  rv = cs550_MQNotify(&mq, accept_msg); assert(rv >= 0);

  // We will wait 1 hour for messages to arrive.  Each arrival will cause accept_msg()
  // to be called once.
  sleep(60*60);

  // Close the port.  Remove the queue file if last to close.
  rv = cs550_MQClose(&mq); assert(rv >= 0);
}
API

This is the API that your code will provide. Because we will compile our own test code against your code, it is crucial that you conform to this API exactly. Failure to do so will result in a low score.
int cs550_MQOpen(struct cs550_MQ *, const char *path, int queue_length);
This function opens the message queue, initalizing the cs550_MQ port struct. The path is a valid file system path. If the file does not exist, it creates it. If the file exists, you may assume that it has been initialized properly. The queue_length argument gives the number of messages that may be queued. In other words, it's the number of messages that may be backed up. You may assume that each message is no longer that 128 KiB (128*1024).
An open message queue (port) belongs to the whole process, not just the thread that opened it. Thus, if a process opens the message queue more than once, it's an error. You may assume that no more than 10 processes will open a message queue.

All messages that are pending in the queue when the queue is opened then become eligible for reception by this port. If there is a message pending in the queue when the queue is opened, then the next receive by that process will receive that message.

int cs550_MQClose(struct cs550_MQ *);
This function closes the port. The corresponding message queue file should be removed if no more ports are open.
int cs550_MQSend(struct cs550_MQ *, const char *msg, size_t size);
This function sends the message with the specified topic on the specified queue. The message must be sent to all open ports, except itself. In other words, if the queue has been opened four times, then it must be received three times, once for each cs550_MQ port struct, except the sender's port.
Note the sending is “conceptual”. You should keep only one copy in the shared memory region.

If the queue is full, this function wil block.

ssize_t cs550_MQRecv(struct cs550_MQ *, char *msg, size_t size);
This function receives the next message in the queue into the message buffer. Messages should be received in the order that they are sent. If the message is too big, then CS550_TOOBIG is returned and the message is left in the queue, otherwise the number of bytes in the message is returned. If cs550_MQNotify() has already been called on this port, then this return CS550_CBREGISTERED.
If the queue is empty, this function will block.

ssize_t cs550_MQPeek(struct cs550_MQ *);
This function checks to see whether or not there is a pending message. If so, it returns the size of the message, else it return CS550_NOMSG.
int cs550_MQNotify(struct cs550_MQ *, void (*cb)(const char *msg, size_t sz));
This function registers the given function pointer cb as a callback to be called whenever a message is available. In other words, if a message arrives, your code must automatically call cb with a pointer to the message and the size of the message. If cs550_MQNotify() has been called, then cs550_MQRecv() must return CS550_CBREGISTERED.
Implementation

To implement the message queue, you will need to use shared memory using mmap(). The path given to cs550_MQOpen() is the name of the file to open for shared memory.
Here is a simple example to get you started.

Submission and Evaluation

Submit a compressed (gzip) tar file named cs550p3_lastname_firstname.tar.gz containing the files through BlackBoard. The tar file should expand to the single directory cs550p3_lastname_firstname with the files in it. Of course, in all cases lastname and firstname should be replaced with your actual last name and first name. The archive must not contain any executable or .o files.

The tar file should contain at a minimum MQ.c, MQ.h, and your test code. Also include a README.txt explaining your submission, what works, what doesn't work, etc. Your grade will be based on your test code. In other words, the more that you can show works, the higher your score. Note that test code is also under the academic honesty policy.