#include <matrix.h>

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

void filter_apply(mtx_t* mtx, mtx_t* filter) {
    if (mtx == NULL || filter == NULL) {
        return;
    }

    size_t filter_size = MTX_N(filter);
    size_t offset = filter_size / 2;

    for (size_t i = offset; i < MTX_N(mtx) - offset; i++) {
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            double value = 0;
            for (size_t k = 0; k < MTX_N(filter); k++) {
                for (size_t l = 0; l < MTX_M(filter); l++) {
                    value += MTX_I_J(mtx, i - offset + k, j - offset + l) *
                             MTX_I_J(filter, k, l);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    /*const size_t THREAD_COUNT = 512;
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
    }*/

    mtx_t* filter = NULL;
    mtx_t* mtx = NULL;

    mtx_create(&filter, 3, 3);
    mtx_create(&mtx, 6, 7);

    mtx_read(filter);
    mtx_read_and_extend(mtx, 3);

    filter_apply(mtx, filter);
}
