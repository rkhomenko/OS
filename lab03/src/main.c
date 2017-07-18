#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex;
static pthread_mutexattr_t mattr;
static size_t proc = 1;

static void* thread_main(void* tid) {
    pthread_mutex_lock(&mutex);
    printf("Hello from thread %lu!\n", proc++);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char* argv[]) {
    const size_t THREAD_COUNT = 512;
    pthread_t tids[THREAD_COUNT];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_mutexattr_init(&mattr);
    pthread_mutex_init(&mutex, &mattr);

    for (size_t i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&tids[i], &attr, thread_main, &tids[i]);
    }
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        pthread_join(tids[i], NULL);
    }
}
