## Uthread and Semaphore Library
### Authors: Erol Bostan and Nivedita Amanjee
### ECS150 Project 2

## Design and Structures

### Queue
The queue program in the queue.c file contains two structures, a queue structure and a node structure. 

The node structure holds a node pointer to the next node and a void data element that could contain either the uthread TCB structure of the uthread library program or a semaphore structure of the semaphore library, depending on which library is using the queue. 

The queue structure holds an integer value that is set to the queue size and two node pointers, one for the first node in the queue and one for the last. 

### Uthread
The uthread library in the uthread.c file initially defines four states for the threads: RUNNING to 0, READY to 1, BLOCKED to 2, and TERMINATED to 4. 

The global variables for this program include the integer TID (the thread ID count), a uthread_tcb variable that holds the currently active thread and a uthread_tcb variable that holds the main thread. Finally, there are two initialized queues, one for the ready threads and one for the terminated threads. 

The structure used in this program, called uthread_tcb, is the thread context block containing the uthread context of type uthread_ctx_t, an integer for the thread ID named TID, and the state of the thread of type integer.
### Semaphores
The structure named "semaphore" in the sem.c file of the semaphore library contains two elements: the internal count of the semaphore of type size_t, and the semaphore queue named sem_queue of type queue_t defined from the queue library. 

## Modified Files and Functions
**1. queue.c** 


- **queue_create()**
This function initializes a queue by first allocating memory for a queue using malloc(). If the memory allocation is successful, it proceeds to set the queue size equal to 0 and the front and last elements of the queue equal to NULL. 

- **queue_destroy()**
This function destroys a queue by first checking if the queue is nonexistent, or if there exists a first element, or if the queue size is not equal to zero. So it first checks if there is a queue to delete, and if there is, it checks if the queue is empty. It returns -1 if any of those checks are true. Otherwise, it frees the memory space for the queue using the free() function and sets the queue equal to NULL. 


- **queue_enqueue()**
This function takes in the queue and data to enqueue and checks if the queue and data passed in are NULL; if so, it returns -1. Otherwise, it creates a node for the queue and sets the node_value of the node to the data passed into the enqueue function. It then sets the next pointer to NULL indicating that the newly created node should be the last value of the queue.
 If the queue is not empty, it sets the queue's last node's next pointer to the newly created node. It also sets queue_last equal to the newly created node. Lastly, if the new node is the first node of the queue, it sets the queue structure's first and last pointers to this node. It then increments the size count of the queue. 
 
- **queue_dequeue()**
This function takes in two parameters, the queue to be dequeued and the data value that will hold the dequeued node. It checks if the queue and data value is NULL and if the queue is empty. If so, it returns -1. 
Otherwise, it set the first element of the queue equal to the data element passed into the function. Then if the element dequeued was not the only node in the queue, it sets the queue_first pointer to the next pointer of the previously first node. 
Otherwise, the queue is set to empty by setting the first and last pointers of the queue to NULL. Lastly, the function decrements the queue size. 

- **queue_delete()**
This function takes in a queue and a data element. It first checks if the queue is empty and if the data element is NULL, and returns -1 if so. Then, a node named current is created to iterate through the queue and check if the data passed in matches with the data of each of the queue's nodes. The initial while loop breaks if it reaches the end of the queue without finding the matched data element. 
If it finds the matched element, it sets the element equal to NULL and frees the memory while resetting the pointers to the correct order. Finally, it decrements the queue and frees the current node memory. 

- **queue_iterate()**
This function takes in a queue and a function as its two parameters. It first checks if the queue is nonexistent, empty, and if the function is NULL. If so, it returns -1. Then a node called current is created to iterate through the queue's nodes and performs the function on each of the node elements of the queue. The function that is performed takes in the current queue and the node of the iterated queue as its arguments. 

- **queue_length()**
This function simply returns the size of the queue if the queue exists. 

**2. uthread.c**
To use this library, the main function must call uthread_run() with three arguments, the boolean of the preempt, the function to pass into each thread when the thread is created, and the argument of the function. 

Once run is called, a main idle thread is created and the initial thread is created with the function and argument passed in. The main thread is set to the currently active thread and also saved in the main thread global variable. Then the uthread_yield() function is called in a loop that terminates once the ready queue is empty. 

- **uthread_current()**
This function simply returns the currently active running thread block, which is a global variable. 

- **uthread_exit()**
This function gets the currently active thread by calling uthread_current() and gets the first thread in the ready queue by calling queue_dequeue() on the ready queue. 
It sets the currently active thread's state to terminated and the dequeued thread's state to run. It enqueues the thread held in the current block to the terminated queue and sets the currently active thread global variable to the thread dequeued from the ready queue. 

- **uthread_yield()**
This function gets the currently active thread by calling uthread_current() and gets the first thread in the ready queue by calling queue_dequeue() on the ready queue. It sets the currently active thread's state to ready and the dequeued thread's state to run. 
Lastly it sets the currently active thread to the ready dequeued thread and performs a context switch on the two threads by calling uthread_ctx_switch(current->context, next->context). The context switch runs the switched thread's function and calls uthread_exit(). 
- **uthread_run()**
This function first initializes the two globally set queues, readyQueue and terminatedQueue, by calling queue_create(). Then it checks if the ready queue is empty; if so, it creates the main idle thread by allocating memory, allocating a context, setting the TID to 0 and setting its state to running. 
Then the initial thread is created by calling uthread_create() with the function and argument passed in. Then a while loop is entered by checking if the readyQueue is not empty. In the while loop, uthread_yield() is called. The while loop exits when the ready queue only has one thread in it and that thread is the main thread. 
Lastly, the terminated queue is dequeued one by one in a loop and the terminated queue's nodes are deleted using the following functions: queue_delete(), uthread_ctx_destroy_stack(), and free(). 

- **uthread_block()**
This function is called in the sem_down() function of the semaphore library. In this function, the current thread's state is set to blocked and the next thread is taken by dequeuing the ready queue. The next thread's state is set to running and a context switch is performed on the two threads. 

- **uthread_unblock()**
This function is called in the sem_up() function of the semaphore library. This function takes in the uthread_tcb block to be unblocked, set's the state of that thread to ready, and enqueues that thread into the ready queue. 

**3. sem.c**

- **sem_create()**
 This function takes in a semaphore count variable as an argument.  It initializes a semaphore structure by setting the count variable equal to the semaphore's internal count and creates a queue by calling queue_create(). Lastly, it returns the newly created semaphore structure. 
 
- **sem_destroy()**
This function first checks if the semaphore structure is nonexistent and if the queue of the structure is nonempty. If so, it returns -1. Otherwise, it destroys the semaphore queue by calling queue_destroy() and frees the semaphore by calling free(). 

- **sem_down()**
This function takes in the semaphore to be counted. It checks if that semaphore is NULL and returns -1 if it is. Then it checks if the semaphore's internal count is 0; if it is, it gets the current thread by calling uthread_current(), enqueues the current thread into the semaphore queue, and blocks the thread by calling uthread_block(). Lastly, it decrements the semaphore count. 

- **sem_up()**
This function takes in the semaphore to be counted as well. It returns -1 if that semaphore is NULL. Then it increments the semaphore count. An if condition checks if the count is greater than 0. If it is, it dequeues a thread from the semaphore queue and unblocks that thread calling uthread_unblock(). 

 **4. queue_tester_example.c**
 
This file has a defined TEST_ASSERT() function which prints "pass" if the function response sent in test_assert is true and prints "fail" if the response is false. 

- **test_create()**
This function calls the TEST_ASSERT(queue_create() != NULL) to check whether the queue has been created successfully. 

- **test_destroy()**
This function creates a queue and initializes an array of 5 integers. It then loops through those integers while first enqueueing and then dequeueing the queue. It then sets the integer "error_value" equal to the function call to queue_destroy() and checks to see if the destroy was successful using the test_assert. 

- **test_notemptyqueue_destroy()**
This function has a similar behavior to the queue_destroy function in that it creates an array of integers and enqueues it to a created queue. However, this time it tries to destroy the queue using queue_destroy() with elements still contained in the queue. 
The integer error_value is used to check if the queue_destroy() is unsuccessful, which is the expected behavior. 

- **test_queue_simple()**
This function tests the queue_enqueue() and queue_dequeue() functions by enqueueing a data pointer of integer value 3. It then dequeues this value and checks if the data dequeued is the same as the enqueued data using the test_assert() definition. 

- **test_length()**
This function tests the queue_length() function by creating a queue, initializing an array of 4 integers, and enqueueing the integers into the queue using queue_enqueue(). Then it tests if the value of queue_length() is equal to 4. 

- **test_queue_one()**
This function has the same performance as the test_length() function except this time the array has one element which is enqueued into the created queue. The test_assert() then checks if the queue_length() function returns 1. 

- **test_queue_null()**
This function tests if the queue_length() function returns -1 when a queue doesn't exist (or it's value is equal to NULL). 

- **test_delete()**
This function tests the queue_delete() function by first initializing a queue, initializing an array of 9 elements, and enqueueing those elements into the queue. Then it deletes 3 elements of the queue one by one and checks if the queue length decreases using the queue_length() function in test_assert. 

- **test_delete_nonexisting()**
This function checks if the queue_delete() function returns -1 when an element that is not in the queue is trying to be deleted. It does this by creating a queue, initializing an array of 9 elements, and enqueueing those elements into the queue. Then queue_delete() is called with a nonexistent integer in the queue and sent into the test_assert function. 

- **test_dequeue_null()**
This test function checks if the queue_dequeue() tries to set its dequeued element to a NULL pointer. It does this by initializing a pointer set to NULL, creating a queue, and sending the queue_dequeue() function with the NULL pointer into the test assert function. 

- **iterator_inc()**
This function was taken from the project prompt. It is used as the function to pass in as the argument of the queue_iterator() test function. It increments the value of the integer in the queue. 

- **test_iterator()**
This function tests our queue_iterator() function by creating a queue, initializing an array of integers, and enqueuing those integers into the queue. Then it iterates through the queue using queue_iterate() and passes in the iterator_inc() function, incrementing each of the integers in the queue by 1. 
Lastly, it uses the test assert definition to check if the data value of the first element of the queue has been incremented. It also checks if the queue length has remained the same at 9 elements. 

## Program Testing 

The programs used to test our uthread and semaphore library were the ones provided in the project guideline. They are as follows: 

1. **uthread_hello.c** 
This program creates one thread that prints "Hello world!". When we tested this program with our library, we got the correct output behavior. 

2. **uthread_yield.c**
This program creates three threads in cascade and prints "thread1", "thread2", and "thread3". Our library was able to produce this behavior. 

3. **sem_simple.c**
This program creates one thread which creates two more threads. Then, using the sem_up() and sem_down() functions, it is supposed to print "thread3", "thread2", "thread1" in order. Our program outputs this response. 

4. **sem_count.c**
This program is supposed to output alternating "thread2 x = 0", "thread1 x = 1", with an incrementing number for x until the number specified by the user in the command line. We were able to get our program to output the first thread print statement, but nothing after that. 

5. **sem_buffer.c**
This program does not work with our library. We are unsure where the mistake is in our semaphore library. 

6. **sem_prime.c**
This program does not work with our library. We are unsure where the mistake is in our semaphore library. 



