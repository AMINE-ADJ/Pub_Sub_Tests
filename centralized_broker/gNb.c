#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    // I- Defining needed sockets.
    void *context = zmq_ctx_new();

    // socket for downlink messages
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_connect(publisher, "tcp://localhost:5555");


    // socket for uplink messages
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://localhost:5556");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "uplink", strlen("uplink"));

    // II- Reading process : same as in Device: except here we are receiving from uplink topic
    zmq_pollitem_t items[] = {
        {subscriber, 0, ZMQ_POLLIN, 0}};

    int count = 0;
    while (1)
    {
        // for incoming uplink messages
        int rc = zmq_poll(items, 1, 1000); // Poll every second
        if (rc > 0 && items[0].revents & ZMQ_POLLIN)
        {
            // Read first the topic then the message.
            //  char topic[256];
            char topic[256] = {0};
            // char message[256];
            char message[256] = {0};
            zmq_recv(subscriber, topic, sizeof(topic), 0);
            topic[sizeof(topic) - 1] = '\0';
            zmq_recv(subscriber, message, sizeof(message), 0);
            message[sizeof(message) - 1] = '\0';

            printf("gNb received on %s: %s\n", topic, message);
        }

        // III- Sending IQ samples as in Device..But in Downlink this time.
        //  Send downlink message
        char topic[] = "downlink";
        char message[256];
        snprintf(message, sizeof(message), "Downlink message %d from gNb \n", count++);
        zmq_send(publisher, topic, strlen(topic), ZMQ_SNDMORE);
        zmq_send(publisher, message, strlen(message), 0);
        // printf("Base Station sent on %s: %s\n", topic, message);

        sleep(3); // Send every 3 seconds
    }

    zmq_close(publisher);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}
