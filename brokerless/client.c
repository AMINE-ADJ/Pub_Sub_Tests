#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

int main()
{
    void *context = zmq_ctx_new();

    // SUB socket to receive messages from the server
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://127.0.0.1:8000");
    assert(rc == 0);
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "server", strlen("server"));
    assert(rc == 0);

  

    char messageR[256];
    char messageS[256];
    int i = 0;

    while (1)
    {
    
 	
        
           
        // Receive a message
        rc = zmq_recv(subscriber, messageR, 256, ZMQ_DONTWAIT);
        assert(rc != -1);
        messageR[rc] = '\0'; // Null-terminate the received string
        printf("Received at client: %s\n", messageR);

       

        i++;
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}

