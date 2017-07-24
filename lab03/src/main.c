#include <matrix.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include <getopt.h>
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

static pthread_mutex_t mutex;

static int threads_disabled = 0;
static int64_t available_threads = -1;

static mtx_t* in_erosion;
static mtx_t* in_dilation;
static mtx_t* out_erosion;
static mtx_t* out_dilation;
static mtx_t* erosion;
static mtx_t* dilation;

static void* filter_apply(void* data) {
    mtx_t* filter = NULL;
    mtx_t* out = NULL;
    mtx_t* in = NULL;
    thread_data_t* td = (thread_data_t*)data;

    switch(TD_FILTER(td)) {
        case EROSION:
            filter = erosion;
            in = in_erosion;
            out = out_erosion;
            break;
        case DILATION:
            filter = dilation;
            in = in_dilation;
            out = out_dilation;
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

    if (!threads_disabled) {
        pthread_mutex_lock(&mutex);
        available_threads++;
        pthread_mutex_unlock(&mutex);
    }

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
    mtx_t* in = NULL;
    filter_wrapper_t* fw = (filter_wrapper_t*)data;
    size_t threads_created = 0;

    switch(FW_FILTER(fw)) {
        case EROSION:
            filter = erosion;
            in = in_erosion;
            break;
        case DILATION:
            filter = dilation;
            in = in_dilation;
            break;
    }

    size_t offset = MTX_N(filter) / 2;
    size_t threads_count = (MTX_N(in) - 2 * offset) * (MTX_M(in) - 2 * offset);
    pthread_t* tids = (pthread_t*)malloc(sizeof(pthread_t) * threads_count);
    thread_data_t* tds = (thread_data_t*)malloc(sizeof(thread_data_t) * threads_count);
    pthread_attr_init(&attr);

    for (size_t i = offset; i < MTX_N(in) - offset; i++) {
        for (size_t j = offset; j < MTX_M(in) - offset; j++) {
            size_t index = (i - offset) * (MTX_M(in) - 2 * offset) + j - offset;
            TD_I(&tds[index]) = i;
            TD_J(&tds[index]) = j;
            TD_FILTER(&tds[index]) = FW_FILTER(fw);

            if (available_threads != 0) {
                if (available_threads > 0) {
                    pthread_mutex_lock(&mutex);
                    available_threads--;
                    pthread_mutex_unlock(&mutex);
                }
                if (pthread_create(&tids[threads_created],
                                   &attr,
                                   filter_apply,
                                   &tds[index])) {
                    perror("ERR CALC CREATE");
                }
                threads_created++;
            }
            else {
                filter_apply(&tds[index]);
            }
        }
    }

    pthread_attr_destroy(&attr);

    for (size_t i = 0; i < threads_created; i++) {
        pthread_join(tids[i], NULL);
    }

    free(tids);
    free(tds);

    return NULL;
}

static void filter_apply_multithread(void) {
    const size_t FW_COUNT = 2;
    pthread_attr_t attr;
    pthread_mutexattr_t mutex_attr;
    filter_wrapper_t fws[FW_COUNT];
    pthread_t tids[FW_COUNT];
    size_t threads_created = 0;

    FW_FILTER(&fws[0]) = EROSION;
    FW_FILTER(&fws[1]) = DILATION;

    pthread_attr_init(&attr);

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&mutex, &mutex_attr);

    for (size_t i = 0; i < FW_COUNT; i++) {
        if (available_threads != 0) {
            if (available_threads > 0) {
                pthread_mutex_lock(&mutex);
                available_threads--;
                pthread_mutex_unlock(&mutex);
            }
            if (pthread_create(&tids[threads_created],
                               &attr,
                               filter_apply_generic,
                               &fws[i])) {
                perror("ERR CREATE");
            }
            threads_created++;
        }
        else {
            filter_apply_generic(&fws[i]);
        }
    }

    for (size_t i = 0; i < threads_created; i++) {
        if (pthread_join(tids[i], NULL)) {
            perror("ERR JOIN");
        }
    }

    pthread_attr_destroy(&attr);

    pthread_mutexattr_destroy(&mutex_attr);
    pthread_mutex_destroy(&mutex);
}

#define MTX_CHECK_ERR(err) \
    do { \
        if ((err) != MTX_NO_ERR) { \
            fprintf(stderr, \
                    "Errro in %s (%s:%d): %s\n", \
                    __func__, \
                    __FILE__, \
                    __LINE__, \
                    mtx_strerror(err)); \
            exit(err); \
        } \
    } while (0)

int main(int argc, char* argv[]) {
    mtx_t* in = NULL;
    size_t erosion_size = 0;
    size_t dilation_size = 0;
    size_t n = 0;
    size_t m = 0;
    size_t max_threads = 0;
    size_t apply_k_times = 1;
    int opt = 0;

    while ((opt = getopt(argc, argv, "hc:k:")) != -1) {
        switch(opt) {
            case 'c':
                max_threads = strtoul(optarg, NULL, 10);
                if (max_threads == ULONG_MAX && errno == ERANGE) {
                    perror("Threads count to big!");
                    exit(EXIT_FAILURE);
                }
                available_threads = max_threads;
                threads_disabled = (available_threads == 0) ? 1 : 0;
                break;
            case 'k':
                apply_k_times = strtoul(optarg, NULL, 10);
                if (apply_k_times == ULONG_MAX && errno == ERANGE) {
                    perror("Cannot apply filter k times. K is to big!");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                puts("-c\t\tset maximum threads count (unlimited if not set)\n"
                     "-k\t\tset k for applying filter k times (one if not set)\n"
                     "-h\t\tprint this message and exit");
                exit(EXIT_SUCCESS);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (scanf("%zu", &erosion_size) != 1) {
        perror("Cannot read erosion filter size!");
        exit(EXIT_FAILURE);
    }

    mtx_err_t err = mtx_create(&erosion, erosion_size, erosion_size);
    MTX_CHECK_ERR(err);
    err = mtx_read(erosion);
    MTX_CHECK_ERR(err);

    if (scanf("%zu", &dilation_size) != 1) {
        perror("Cannot read dilation filter size!");
        exit(EXIT_FAILURE);
    }

    err = mtx_create(&dilation, dilation_size, dilation_size);
    MTX_CHECK_ERR(err);
    err = mtx_read(dilation);
    MTX_CHECK_ERR(err);

    if (scanf("%zu %zu", &n, &m) != 2) {
        perror("Cannot read size of input matrix!");
        exit(EXIT_FAILURE);
    }

    err = mtx_create(&in, n, m);
    MTX_CHECK_ERR(err);
    err = mtx_read(in);
    MTX_CHECK_ERR(err);

    err = mtx_extend(&in_erosion, in, erosion_size);
    MTX_CHECK_ERR(err);

    err = mtx_extend(&in_dilation, in, dilation_size);
    MTX_CHECK_ERR(err);

    mtx_destroy(in);

    err = mtx_create(&out_erosion, n, m);
    MTX_CHECK_ERR(err);

    err = mtx_create(&out_dilation, n, m);
    MTX_CHECK_ERR(err);

    for (size_t k = 0; k < apply_k_times; k++) {
       filter_apply_multithread();
       err = mtx_extend_exist(in_erosion, out_erosion, erosion_size);
       MTX_CHECK_ERR(err);
       err = mtx_extend_exist(in_dilation, out_dilation, dilation_size);
    }

    mtx_print(out_erosion);
    putchar('\n');
    mtx_print(out_dilation);

    mtx_destroy(in_erosion);
    mtx_destroy(in_dilation);
    mtx_destroy(erosion);
    mtx_destroy(dilation);
    mtx_destroy(out_erosion);
    mtx_destroy(out_dilation);
}
