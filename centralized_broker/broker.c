#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    void *context = zmq_ctx_new();
    printf("Broker is running using zmq proxy\n");
    // XSUB socket for publishers
    void *xsub_socket = zmq_socket(context, ZMQ_XSUB);
    printf("Broker is running using zmq proxy1\n");
    zmq_bind(xsub_socket, "tcp://*:5555"); // Extended subscriber

    // XPUB socket for subscribers
    void *xpub_socket = zmq_socket(context, ZMQ_XPUB);
    zmq_bind(xpub_socket, "tcp://*:5556"); // Extended publisher
    printf("Broker is running using zmq proxy2\n");

    // Proxy between XSUB and XPUB sockets
    // 1st function :XSUB=>XPUB to simplify message forwarding between sockets (  XSUB ( from publishers ) + XPUB (to subscribers ) )
    // 2nd function of zmq proxy : XPUB-> XSUB :takes subscription updates received on the XPUB socket (from subscribers) and forwards them to the XSUB socket (to publishers).

    zmq_proxy(xsub_socket, xpub_socket, NULL);
    printf("Broker is running using zmq proxy\n");

    // zmq_proxy(xpub_socket, xsub_socket, NULL);

    // Clean up
    zmq_close(xsub_socket);
    zmq_close(xpub_socket);
    zmq_ctx_destroy(context);

    return 0;
}
