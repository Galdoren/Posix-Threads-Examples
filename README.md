# Posix-Threads-Examples
POSIX Threads Examples repository. These examples shows you how to create a POSIX thread, how to modify them. 

> You can find the compilation command inside the source files.

## Example 1
Illustrates how you can create a simple POSIX thread.

## Example 2
Gives you an example for passing an argument for the thread and passing attribute struct for thread.

## Example 3
Gives you an example for increasing the available stack size for a thread. Modifying the stack size might be required in cases where you need to create an array or some sort of object that's size exceeds the default stack size. In this case, you need to modify the stacksize of the thread. We use the thread attribute struct that we've created to modify the stack size avaiable for the POSIX thread.

## Example 4
Gives a bried introduction to thread synchronization using mutexes. This is a really important topic in for avoiding a setting called race-condition.

## Example 5
Gives an introduction to a different locking mechanism called semaphores. The most distinctive difference between a semaphore and a mutex is a mutex can only be unlocked by the thread that locked it, however a semaphore can be unlocked by a different thread. In this case, you also need to know that a semaphore has a counter mechanism that holds the number of times it can be locked consecutively. We specify this number while creating a semaphore and the semaphore value is set to the given parameter. By locking a semaphore, we mean that we decrese this counter by 1, and by unlocking it, we increase it by 1.

## Example 6
Here I've written a simple producer consumer model. There are N consumers and M producers in this example, and each of them tries to access the same resource. I've added 2 safeguards which are indices of the current producer/consumer location, in order to make consumers wait for a new job to consume. In this example, you'll notice that there is no precedence, meaning that the producer might not even obtain the mutex because there are many consumers also waiting to acquire the mutex, and the OS have no idea to order the waiting threads, thus creating the starvation problem.

## Example 7
In this example, I'll given one of the ways to solve the starvation problem, by using the triple mutex method. You'll also noticed another type here, pthread_cond_t, which is basically a conditional variable. A conditional variable is a synchronization method like a mutex, however we do not lock or unlock it, instead, we satisfy a condition and the thread continues to work, simple as that. I've named them as barrier here since it suits the context in the given code because I've used them to make all producer and consumer threads wait to be notified by the main threadbefore starting doing work, and after each of threads are created, I've broadcasted that the condition is satisfied to the waiting threads thus they can continue to work.
