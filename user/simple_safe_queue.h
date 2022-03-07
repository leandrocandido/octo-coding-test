#ifndef SIMPLESAFEQUEUE_H
#define SIMPLESAFEQUEUE_H
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

//I'm going to write and try be more specific possible once this is for a test purpose

struct fileInfo
{
    std::wstring path;
    uint32_t pid = 0;
};

template <typename T>
class simple_safe_queue
{

    // that is our node which one wrap any kind of data,
    // this structure holds both the data to be store and the pointer to the next item
    struct node
    {
        shared_ptr<T> data;
        unique_ptr<node> next;
    };

private:
    //in this aproach both tail and head have their own mutex
    std::mutex head_mtx;
    std::mutex tail_mtx;
    std::condition_variable cv; //used to notify when queue is read for reading
    std::unique_ptr<node> head;
    node* tail;

private:

    //check if notification received and then proceed with the pop from head
    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> ul = wait_for_data();
        return pop_head();
    }

    // this method is responsibe to set head pointer to the second element in the
    // chain and then return the element pointed by head previously
    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node> ptr = std::move(head);
        head = std::move(ptr->next);
        return ptr;
    }

    // will wait until gets the notification by condition variable in push method
    // and will proceed if head is not null
    std::unique_lock<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> ul(head_mtx);
        cv.wait(ul, [&] { return head != nullptr; });
        return std::move(ul);
    }

public:
    simple_safe_queue()
    {
        head = nullptr;
        tail = nullptr;
    }


    //add elements to the queue
    void push(T item)
    {
        shared_ptr<T> data(make_shared<T>(std::move(item)));
        auto n = make_unique<node>();
        n->data = std::move(data);
        {
            // start of critical section
            std::lock_guard<std::mutex> lg(tail_mtx);
            if (head == nullptr)
            {
                head = std::move(n);
                tail = head.get();
            }
            else
            {
                tail->next = std::move(n);
                tail = tail->next.get();
            }                     
        } //end of critical section

        cv.notify_one();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_ptr<node> ptr = wait_pop_head();

        return ptr->data;
    }
};

#endif // !SIMPLESAFEQUEUE_H