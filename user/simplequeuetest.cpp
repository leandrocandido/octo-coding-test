#include "simplequeuetest.h"

void execute_simple_queue_test()
{
    // const int size = 1000;
    thread pushthread[100];
    thread popthread[100];
    for (size_t i = 0; i < 100; i++)
    {
        fileInfo fi;
        fi.path = L"text.txt";
        fi.pid = i;
        pushthread[i] = thread([&]() { simple_queue.push(fi); });
    }

    for (size_t i = 0; i < 100; i++)
    {
        popthread[i] = thread([]() { simple_queue.wait_and_pop(); });
    }

    for (int i = 0; i < 100; i++)
    {
        pushthread[i].join();
    }

    for (int i = 0; i < 100; i++)
    {
        popthread[i].join();
    }
}