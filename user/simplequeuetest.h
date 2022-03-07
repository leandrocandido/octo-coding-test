#ifndef SIMPLEQUEUETEST_H
#define SIMPLEQUEUETEST_H

#include <thread>
#include "simple_safe_queue.h"

using namespace std;

extern simple_safe_queue<fileInfo> simple_queue;

void execute_simple_queue_test();

#endif // !SIMPLEQUEUETEST_H