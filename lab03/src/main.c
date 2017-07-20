#include <matrix.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

enum filter_type {
    EROSION,
    DILATION
};
typedef enum filter_type filter_t;

struct thread_data {
    filter_t filter;
    size_t i;
    size_t j;
};
typedef struct thread_data thread_data_t;

#define TD_FILTER(td) ((td)->filter)
#define TD_I(td) ((td)->i)
#define TD_J(td) ((td)->j)

static mtx_t* in;
static mtx_t* out;
static mtx_t* erosion;
static mtx_t* dilation;

static void* filter_apply(void* data) {
    mtx_t* filter = NULL;
    thread_data_t* td = (thread_data_t*)data;

    switch(TD_FILTER(td)) {
        case EROSION:
            filter = erosion;
            break;
        case DILATION:
            filter = dilation;
            break;
    }

    size_t offset = MTX_N(filter) / 2;
    double value = 0;
    for (size_t k = 0; k < MTX_N(filter); k++) {
        for (size_t l = 0; l < MTX_M(filter); l++) {
            value += MTX_I_J(in, TD_I(td) - offset + k, TD_J(td) - offset + l) *
                     MTX_I_J(filter, k, l);
        }
    }

    MTX_I_J(out, TD_I(td) - offset, TD_J(td) - offset) = value;

    return NULL;
}

struct filter_wrapper {
    filter_t filter;
};
typedef struct filter_wrapper filter_wrapper_t;

#define FW_FILTER(fw) ((fw)->filter)

static void* filter_apply_generic(void* data) {
    pthread_attr_t attr;
    mtx_t* filter = NULL;
    filter_wrapper_t* fw = (filter_wrapper_t*)data;

    switch(FW_FILTER(fw)) {
        case EROSION:
            filter = erosion;
            break;
        case DILATION:
            filter = dilation;
            break;
    }

    size_t offset = MTX_N(filter) / 2;
    size_t threads_count = (MTX_N(in) - 2 * offset) * (MTX_M(in) - 2 * offset);
    pthread_t* tids = (pthread_t*)malloc(sizeof(pthread_t) * threads_count);
    thread_data_t* tds = (thread_data_t*)malloc(sizeof(thread_data_t) * threads_count);

    pthread_attr_init(&attr);

    for (size_t i = offset; i < MTX_N(in) - offset; i++) {
        for (size_t j = offset; j < MTX_M(in) - offset; j++) {
            size_t index = i * (MTX_M(in) - 2 * offset);
            TD_I(&tds[index]) = i;
            TD_J(&tds[index]) = j;
            TD_FILTER(&tds[index]) = FW_FILTER(fw);
            pthread_create(&tids[index], &attr, filter_apply, &tds[index]);
        }
    }

    for (size_t i = 0; i < threads_count; i++) {
        pthread_join(tids[i], NULL);
    }

    return NULL;
}

static void filter_apply_multithread(void) {
    const size_t FW_COUNT = 2;
    pthread_attr_t attr;
    filter_wrapper_t fws[FW_COUNT];
    pthread_t tids[FW_COUNT];

    FW_FILTER(&fws[0]) = EROSION;
    FW_FILTER(&fws[1]) = DILATION;

    for (size_t i = 0; i < FW_COUNT; i++) {
        pthread_create(&tids[i], &attr, filter_apply_generic, &fws[i]);
    }

    for (size_t i = 0; i < FW_COUNT; i++) {
        pthread_join(tids[i], NULL);
    }
}

int main(int argc, char* argv[]) {
    mtx_create(&erosion, 3, 3);
    mtx_create(&dilation, 3, 3);
    mtx_create(&in, 6, 7);
    mtx_create(&out, 4, 5);

    mtx_read(erosion);
    mtx_read(dilation);
    mtx_read_and_extend(in, 3);

    filter_apply_multithread();
}
