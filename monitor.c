

#include "message.h"

int main(int argc, char *argv[]) {
  mqd_t msqid;

  int initalTemp = atoi(argv[1]);
  int numOfNodes = atoi(argv[2]);
  int nodeTemps[100];

  struct mq_attr attr;
attr.mq_flags = 0;
attr.mq_maxmsg = 10;         
attr.mq_msgsize = sizeof(MESSAGE); 
attr.mq_curmsgs = 0;

  mq_unlink(MONITOR_QUEUE);
  if ((msqid = mq_open(MONITOR_QUEUE, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attr)) < 0) {
      oops("Error opening a queue.", errno);
   }


  for (int i = 3; i < argc; i++) {
    nodeTemps[i-3] = atoi(argv[i]);
  }

  //Fork nodes here
  for (int i = 0; i < numOfNodes; i++) {
    char nodeName[4];
    sprintf(nodeName, "%d", i);
    char temp[8];
    sprintf(temp, "%d", nodeTemps[i]);

    MESSAGE msg;
    msg.nodeId = i;  
    msg.stable = 0;  
    msg.temperature = initalTemp;

    char mqName[20] = NODE_NAME_PREFIX;
    strcat(mqName, nodeName);

    mq_unlink(mqName);
    mqd_t nodeQueue = mq_open(mqName, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attr);
        if (nodeQueue < 0) {
            oops("Error opening a queue.", errno);
        }

        
        if (mq_send(nodeQueue, (char *)&msg, sizeof(MESSAGE), 1) < 0) {
            oops("Error sending message to node", errno);
        }

    pid_t forkedNode = fork();
    if (forkedNode < 0) {
      oops("Error forking.", errno);
    } else if (forkedNode == 0) {
      if (execlp("./node", nodeName, temp, NULL) == -1) {
        perror("execlp");
        exit(EXIT_FAILURE);
      }
    }
  }

  sleep(1);
float previous_integrated_temp = initalTemp;

int stable = 0;

mqd_t msqidNode;
if ((msqidNode = mq_open(MONITOR_QUEUE, O_RDWR)) < 0) {
            oops("Error opening Node queue", errno);
        }

while (1) {
    float sum_of_client_temps = 0.0;

    // Receive messages from all nodes
    for (int i = 0; i < numOfNodes; i++) {
        MESSAGE msg;
        unsigned int msgType;
        /*mqd_t msqidNode;

        if ((msqidNode = mq_open(MONITOR_QUEUE, O_RDWR)) < 0) {
            oops("Error opening Node queue", errno);
        }*/
        //sleep(1);
        if (mq_receive(msqidNode, (char *)&msg, sizeof(MESSAGE), &msgType) < 0) {
            oops("Error receiving message", errno);
        }

        if(msg.stable == 1){
          stable += 1;
        }

        //printf("Monitor received node %d temperature: %.2f\n", msg.nodeId, msg.temperature);
        sum_of_client_temps += msg.temperature;
    }

    if(stable == numOfNodes){
      printf("STABLE TEMPERATURE DETECTED.\nMONITOR TERMINATING...\n");
      mq_unlink(MONITOR_QUEUE);
      mq_close(msqidNode);
      exit(EXIT_SUCCESS);
    }

    float new_integrated_temp = (2 * previous_integrated_temp + sum_of_client_temps) / 6;
    //printf("Monitor computed new integrated temperature: %.2f\n", new_integrated_temp);

    previous_integrated_temp = new_integrated_temp;

    for (int i = 0; i < numOfNodes; i++) {
        char nodeName[20];
        sprintf(nodeName, "%d", i);
        char mqName[20] = NODE_NAME_PREFIX;
        strcat(mqName, nodeName);

        MESSAGE msg;
        msg.nodeId = i;
        msg.temperature = new_integrated_temp;

        mqd_t msqidNode = mq_open(mqName, O_RDWR);
        if (msqidNode < 0) {
            oops("Error opening Node queue for sending", errno);
        }

        if (mq_send(msqidNode, (char *)&msg, sizeof(MESSAGE), 1) < 0) {
            oops("Error sending new integrated temperature to node", errno);
        }
    }
}

}

