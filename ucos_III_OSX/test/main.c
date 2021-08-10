//
//  main.c
//  test
//
//  Created by Lei Yue on 2021/8/4.
//

#include <stdio.h>

//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
//}


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* Simple error handling functions */

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void *
sig_thread(void *arg)
{
    sigset_t *set = arg;
    int s, sig;

    for (;;) {
        /**
         The sigwait() function suspends execution of the calling thread
                until one of the signals specified in the signal set set becomes
                pending.  The function accepts the signal (removes it from the
                pending list of signals), and returns the signal number in sig.

         If the signal is blocked by the signal mask, then the state of the signal is set to pending, and the program continues execution. A signal may be blocked, which means that it will not be delivered until it is later unblocked.  Between the time when it is generated and
         when it is delivered a signal is said to be pending.
         */
        // 子线程也block了SIGQUIT和SIGUSR1，因而进入pending状态
        s = sigwait(set, &sig); // 等待信号量集合set中任意一个进入pending状态
        if (s != 0)
            handle_error_en(s, "sigwait");
        printf("Signal handling thread got signal %d\n", sig);
    }
}

int
main(int argc, char *argv[])
{
    pthread_t thread;
    sigset_t set;
    int s;

    /* Block SIGQUIT and SIGUSR1; other threads created by main()
     will inherit a copy of the signal mask. */

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1); // 主线程屏蔽SIGQUIT和SIGUSR1信号，子线程继承主线程的屏蔽集合
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_sigmask");

    s = pthread_create(&thread, NULL, &sig_thread, &set);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    /* Main thread carries on to create other threads and/or do
     other work. */

    pause();            /* Dummy pause so we can test program */
}
