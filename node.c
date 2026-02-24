

#include "message.h"

int main(int argc, char *argv[]) {
    MESSAGE msg;
    unsigned int msgType;
    float previousTemp = -1;

    char NODE_QUEUE[20] = NODE_NAME_PREFIX;
    strcat(NODE_QUEUE, argv[0]);

    mqd_t msqidNode;
    if ((msqidNode = mq_open(NODE_QUEUE, O_RDWR)) < 0)
        oops("Error opening Node queue", errno);

    mqd_t msqidMonitor;
    if ((msqidMonitor = mq_open(MONITOR_QUEUE, O_WRONLY)) < 0)
        oops("Error opening Monitor queue for sending", errno);

    while(1){

    // Receive message from Node's queue, not Monitor queue
    if (mq_receive(msqidNode, (char *)&msg, sizeof(MESSAGE), &msgType) < 0) {
        oops("Error receiving message", errno);
    }

    //printf("Node %d received message: Temp %.2f, Stable %d\n", msg.nodeId, msg.temperature, msg.stable);

    float previous_node_temp = atof(argv[1]); // Convert initial temp to float
    float new_integrated_temp = msg.temperature;

    float new_node_temp = (previous_node_temp * 3 + 2 * new_integrated_temp) / 5;
    printf("NODE: %d TEMPERATURE: %.2f\n", msg.nodeId, new_node_temp);

    msg.temperature = new_node_temp;
    if(previousTemp == new_node_temp){
        printf("NODE: %d TEMPERATURE: %.2f\n", msg.nodeId, new_node_temp);
        printf("NODE: %d TERMINATING...\n", msg.nodeId);
        msg.stable = 1;
        if (mq_send(msqidMonitor, (char *)&msg, sizeof(MESSAGE), 1) < 0) {
        oops("Error sending updated temperature to monitor", errno);
    }
        mq_unlink(NODE_QUEUE);
        mq_close(msqidNode);
        exit(EXIT_SUCCESS);
    }
    previousTemp = new_node_temp;

    if (mq_send(msqidMonitor, (char *)&msg, sizeof(MESSAGE), 1) < 0) {
        oops("Error sending updated temperature to monitor", errno);
    }
    }

    return 0;
}
