#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main()
{
    // I- Initialize ZeroMQ context
    void *context = zmq_ctx_new();

    // Publisher socket for downlink messages and current time
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_connect(publisher, "tcp://localhost:5555");

    // Subscriber socket for uplink messages and broker notifications
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://localhost:5556");

    // Subscribe to both uplink and any potential notification topics
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "uplink", strlen("uplink"));
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "Heartbeat", strlen("Heartbeat")); // Subscribe to all messages

    // Polling setup
    zmq_pollitem_t items[] = {
        {subscriber, 0, ZMQ_POLLIN, 0}};

    // State tracking
    int client_available = 0;
    int count = 0;

    while (1)
    {
        // 1. Publish current time
        time_t now;
        time(&now);
        char time_str[256];
        snprintf(time_str, sizeof(time_str), "%ld", now);

        char time_topic[] = "Current_Time";
        zmq_send(publisher, time_topic, strlen(time_topic), ZMQ_SNDMORE);
        zmq_send(publisher, time_str, strlen(time_str), 0);

        // 2. Check for incoming messages
        int rc = zmq_poll(items, 1, 1000); // Poll for 1 second

        if (rc > 0 && items[0].revents & ZMQ_POLLIN)
        {
            char topic[256] = {0};
            char message[256] = {0};

            zmq_recv(subscriber, topic, sizeof(topic), 0);
            topic[sizeof(topic) - 1] = '\0';
            zmq_recv(subscriber, message, sizeof(message), 0);
            message[sizeof(message) - 1] = '\0';

            printf("Server received on %s: %s\n", topic, message);

            // Check for client availability or uplink messages
            if (strcmp(topic, "Heartbeat") == 0)
            {
                client_available = 1;
            }
        }

        // 3. If client is available, send downlink data
        if (client_available)
        {
            char downlink_topic[] = "Downlink";
            char downlink_msg[256];
            snprintf(downlink_msg, sizeof(downlink_msg), "Server Downlink Data %d", count++);

            zmq_send(publisher, downlink_topic, strlen(downlink_topic), ZMQ_SNDMORE);
            zmq_send(publisher, downlink_msg, strlen(downlink_msg), 0);
        }

        sleep(3); // Cycle every 3 seconds
    }

    // Cleanup
    zmq_close(publisher);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}