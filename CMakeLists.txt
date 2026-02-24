project(messageQueues C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-m64 -g -O0 -D_DEBUG -Wall")
set(MONITOR monitor.c)
set(NODE node.c)

add_executable(monitor ${MONITOR})
add_executable(node ${NODE})
