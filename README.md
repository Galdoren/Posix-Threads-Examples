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
Gives an introduction to a different locking mechanism called semaphores. The most distinctive difference between a semaphore and a mutex is a mutex can only be unlocked by the thread that locked it, however a semaphore can be unlocked by a different thread. In this case, you also need to know that a semaphore has a counter mechanism that holds the number of times it can be locked consecutively. We specify this number while creating a semaphore. By locking a semaphore, we mean that we decrese the counter by 1, and by unlocking it, we increase it by 1.
