#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main()
{
    // I- Initialize ZeroMQ context
    void *context = zmq_ctx_new();

    // Publisher socket for uplink messages
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_connect(publisher, "tcp://localhost:5555");

    // Subscriber socket for downlink and current time
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://localhost:5556");

    // Subscribe to specific topics
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "Current_Time", strlen("Current_Time"));
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "Downlink", strlen("Downlink"));

    // Polling setup
    zmq_pollitem_t items[] = {
        {subscriber, 0, ZMQ_POLLIN, 0}};

    // State tracking
    int server_exists = 0;
    int count = 0;

    while (1)
    {
        // 1. Send heartbeat (availability message)
        char heartbeat_topic[] = "Heartbeat";
        char heartbeat_msg[256];
        snprintf(heartbeat_msg, sizeof(heartbeat_msg), "Client Available");

        zmq_send(publisher, heartbeat_topic, strlen(heartbeat_topic), ZMQ_SNDMORE);
        zmq_send(publisher, heartbeat_msg, strlen(heartbeat_msg), 0);

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

            printf("Client received on %s: %s\n", topic, message);

            // Check for Current Time to confirm server exists
            if (strcmp(topic, "Current_Time") == 0)
            {
                server_exists = 1;
                printf("Server detected. Current Time: %s\n", message);
            }
        }

        // 3. If server exists, communicate via uplink/downlink
        if (server_exists)
        {
            // Publish client data to uplink
            char uplink_topic[] = "Uplink";
            char uplink_msg[256];
            snprintf(uplink_msg, sizeof(uplink_msg), "Client Uplink Data %d", count++);

            zmq_send(publisher, uplink_topic, strlen(uplink_topic), ZMQ_SNDMORE);
            zmq_send(publisher, uplink_msg, strlen(uplink_msg), 0);
        }

        sleep(3); // Cycle every 3 seconds
    }

    // Cleanup
    zmq_close(publisher);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}