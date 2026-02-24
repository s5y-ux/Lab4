#pragma once
#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>


#define MONITOR_QUEUE "/MONITOR"
#define NODE_NAME_PREFIX "/NODE_"
#define oops(ermsg,erno) { perror(ermsg); exit(erno); }


typedef struct temperature {
    mqd_t msqid;
    float previousTemperature;
} TEMPERATURE;

typedef struct message {
    int stable;
    int nodeId;
    float temperature;
} MESSAGE;



/*
1. All includes
printf, exit, fork, mq_x, strings, snprintf

2. structs
MESSAGE
TEMPERATURE

3. Any Constants
MONITOR_QUEUE_NAME
NODE_PREFIX
 */

