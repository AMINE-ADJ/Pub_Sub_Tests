#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    // I- Defining needed sockets.
    void *context = zmq_ctx_new();

    // Publisher socket for uplink messages
    void *publisher = zmq_socket(context, ZMQ_PUB);
    // connect it to X-SUB
    zmq_connect(publisher, "tcp://localhost:5555");

    // Subscriber socket for downlink messages
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    // Connect it to X-PUB + downlink topic
    zmq_connect(subscriber, "tcp://localhost:5556");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "downlink", strlen("downlink"));

    // II- Incoming Data reading ( from downlink topic ) :
    // we define polling items : the subscriber socket + specify event type : incoming messages.
    zmq_pollitem_t items[] = {
        {subscriber, 0, ZMQ_POLLIN, 0}};

    int count = 0;
    while (1)
    {
        // Check for incoming downlink messages
        // here we define first the polling logic, monitor sockets in items ( we ve just one )
        // And define polling period = 1 second = 1000 ms.
        //( in polling period ZMQ checks if there are any incoming events on that socket or not )
        int rc = zmq_poll(items, 1, 1000);

        // rc indicates if our monitored socket received data or not. rc > 0 => we received data.
        // we double check if the event is ZMQ POLLIN so it's an incoming message from that socket not other thing..

        if (rc > 0 && items[0].revents & ZMQ_POLLIN)
        { // we read the incoming message ( IQ SAMPLES pour l'instant...)

            char topic[256];
            char message[256];
            zmq_recv(subscriber, topic, sizeof(topic), 0);
            zmq_recv(subscriber, message, sizeof(message), 0);
            printf("5G Device received on %s: %s\n", topic, message);

            // OR

            // Receive all of them :
            // char messageR[256];
            //     rc = zmq_recv(subscriber, messageR, sizeof(messageR) - 1, ZMQ_DONTWAIT);
            // if (rc == -1) {
            //     if (zmq_errno() == EAGAIN) {
            //         // No message available yet
            //         printf("No message available yet.\n");
            //     } else {
            //         // Print the error
            //         fprintf(stderr, "zmq_recv error: %s\n", zmq_strerror(zmq_errno()));
            //     }
            // } else {
            //     // Null-terminate the string
            //     messageR[rc] = '\0';

            //     // Check the identifier and filter out own
            //        printf("%s\n", messageR);
            // }
            // printf("topic: %s\n", topic);
            // printf ("message: %s\n", message);
        }

        // III- Produced IQ samples Sending process : ( publish simply to uplink topic )

        // Send uplink message
        char topic[] = "uplink";
        char message[256];
        // Format the message string
        snprintf(message, sizeof(message), "Uplink message %d from 5G Device 1\n", count++);
        // send it in chunks here ?
        zmq_send(publisher, topic, strlen(topic), ZMQ_SNDMORE);
        zmq_send(publisher, message, strlen(message), 0);
        // printf("5G Device sent on %s: %s\n", topic, message);

        // OR we could do it this way to send a simple message :
        //  Format the message string with the current value of i
        // snprintf(message, sizeof(message), "Hello World from pub %d", i);
        // Send the formatted string
        // rc = zmq_send(publisher, message, strlen(message), 0);
        // assert(rc == strlen(message));

        sleep(3); // Send every 3 seconds
    }

    zmq_close(publisher);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}
