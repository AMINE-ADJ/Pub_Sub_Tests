#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

int main()
{
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:5556");
    assert(rc == 0);
    
    
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    rc = zmq_connect(subscriber, "tcp://127.0.0.1:5557");
    assert(rc == 0);
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    assert(rc == 0);
    
    

    int i = 0;
    char message[256];
    char messageR[256];
    
    while (1)
    {
        // Format the message string with the current value of i
        snprintf(message, sizeof(message), "Hello World from pub %d", i);
        
        // Send the formatted string
        rc = zmq_send(publisher, message, strlen(message), 0);
        assert(rc == strlen(message));
        
        
        rc = zmq_recv(subscriber, messageR, sizeof(messageR) - 1, ZMQ_DONTWAIT);
        if (rc == -1) {
            if (zmq_errno() == EAGAIN) {
                // No message available yet
                printf("No message available yet.\n");
            } else {
                // Print the error
                fprintf(stderr, "zmq_recv error: %s\n", zmq_strerror(zmq_errno()));
            }
        } else {
            // Null-terminate the string
            messageR[rc] = '\0';

            // Check the identifier and filter out own 
               printf("%s\n", messageR);
        }
     
        
        
        i++;
    }
    
    zmq_close(publisher);
    zmq_ctx_destroy(context);
    
    return 0;
}
