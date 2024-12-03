#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

int main()
{
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:8000");
    if (rc != 0) {
        printf("zmq_bind failed with error: %s\n", zmq_strerror(zmq_errno()));
        return 1; // Exit the program if zmq_bind fails
    }

 
    int i = 0;
    char messageS[256];
    char messageR[256];

    while (1)
    {
        // Format the message string with the current value of i
        snprintf(messageS, sizeof(messageS), "server %d", i);

        // Send the formatted string with "server" topic
        rc = zmq_send(publisher, "server ", 7, ZMQ_SNDMORE);
        assert(rc == 7);
        rc = zmq_send(publisher, messageS, strlen(messageS), 0);
        assert(rc == strlen(messageS));

     

        i++;
    }

    zmq_close(publisher);

    zmq_ctx_destroy(context);

    return 0;
}

