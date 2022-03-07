#include "notifications.h"
#include "wil/resource.h"
#include <Windows.h>
#include <memory>
#include <iostream>
#include "simple_safe_queue.h"
#include "simplequeuetest.h"

//linked list queue declared in globa scope
simple_safe_queue<fileInfo> simple_queue;

bool notify_callback(std::wstring const& path, uint32_t pid)
{
    //
    // McAfee Code Test Candidate. This is the function you must edit. You do not need to run or
    // test this code but it must compile before submission. Please see README.md for details.
    //
    
  //***********************************************************************************************
  /*in the exercice description we are suppose to store information for deferred processing
    in this case a queue data strucuture fits very well in this requirement.    
    it was suppose to make this callback method thread safe, but I made the option 
    to make the queue itself thread safe, same as we have in .net framewors (ConcurrentQueue)
    for simplicity I created two methods in the queue:
     - push : to inser items in the queue
     - wait_and_pop: to get and remove items from the queue, this methods has the same behavior
       as SQS(aws) long polling, it will hold until message is received to be processed */  
  //***********************************************************************************************


    //for simplicity sake not doing validation or exception threatment 
    fileInfo fi;
    fi.path = path;
    fi.pid = pid;
    simple_queue.push(fi);
    return true;
}


int main(int argc, char** argv)
{
    printf("starting\n");

    //execute_simple_queue_test();

    wil::unique_handle stop_event{CreateEvent(NULL, FALSE, FALSE, NULL)};
    if (!stop_event.get())
    {
        printf("failed to create stop event\n");
        return 1;
    }

    if (!register_for_fs_notifications(notify_callback, stop_event.get()))
    {
        printf("register_for_fs_notifications failed\n");
        return 2;
    }
    
    auto wait_result = WaitForSingleObject(stop_event.get(), INFINITE);
    if (wait_result == WAIT_FAILED)
    {
        printf("WaitForSingleObject failed\n");
        return 3;
    }

    printf("succeeded\n");
    return 0;
}
