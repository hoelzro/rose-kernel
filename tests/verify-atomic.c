#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <rose/atomic.h>
#include <rose/stdint.h>

#define NUM_THREADS 4

static void
check_8bit_cas(void)
{
    uint8_t value = 17;

    assert(atomic_compare_and_set8(&value, 17, 18));
    assert(value == 18);
    assert(! atomic_compare_and_set8(&value, 17, 19));
    assert(value == 18);
}

static void
check_16bit_cas(void)
{
    uint16_t value = 17;

    assert(atomic_compare_and_set16(&value, 17, 18));
    assert(value == 18);
    assert(! atomic_compare_and_set16(&value, 17, 19));
    assert(value == 18);
}

static void
check_32bit_cas(void)
{
    uint32_t value = 17;

    assert(atomic_compare_and_set32(&value, 17, 18));
    assert(value == 18);
    assert(! atomic_compare_and_set32(&value, 17, 19));
    assert(value == 18);
}

static void
check_64bit_cas(void)
{
    uint64_t value = 17;

    assert(atomic_compare_and_set64(&value, 17, 18));
    assert(value == 18);
    assert(! atomic_compare_and_set64(&value, 17, 19));
    assert(value == 18);
}

static void *
many_thread_function_cas(void *udata)
{
    uint32_t *valuep = udata;
    uint32_t value;
    int i;

    for(i = 0; i < 1000000; i++) {
        value = *valuep;
        while(!atomic_compare_and_set32(valuep, value, value + 1)) {
            value = *valuep;
        }
    }
    return NULL;
}

static void
check_many_threads_cas(void)
{
    pthread_t threads[NUM_THREADS];
    int i;
    uint32_t value = 0;

    for(i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, many_thread_function_cas, &value);
    }

    for(i = 0; i < NUM_THREADS; i++) {
        void *result;
        pthread_join(threads[i], &result);
    }

    assert(value == NUM_THREADS * 1000000);
}

static void
check_8bit_incr(void)
{
    uint8_t value = 17;

    assert(atomic_incr8(&value) == 18);
    assert(atomic_incr8(&value) == 19);
    /* XXX check at roll over point(s) */
}

static void
check_16bit_incr(void)
{
    uint16_t value = 17;

    assert(atomic_incr16(&value) == 18);
    assert(atomic_incr16(&value) == 19);
    /* XXX check at roll over point(s) */
}

static void
check_32bit_incr(void)
{
    uint32_t value = 17;

    assert(atomic_incr32(&value) == 18);
    assert(atomic_incr32(&value) == 19);
    /* XXX check at roll over point(s) */
}

static void *
many_thread_function_incr(void *udata)
{
    uint32_t *valuep = udata;
    int i;

    for(i = 0; i < 1000000; i++) {
        atomic_incr32(valuep);
    }
    return NULL;
}

static void
check_many_threads_incr(void)
{
    pthread_t threads[NUM_THREADS];
    int i;
    uint32_t value = 0;

    for(i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, many_thread_function_incr, &value);
    }

    for(i = 0; i < NUM_THREADS; i++) {
        void *result;
        pthread_join(threads[i], &result);
    }

    assert(value == NUM_THREADS * 1000000);
}

int
main(void)
{
    check_8bit_cas();
    check_16bit_cas();
    check_32bit_cas();
    check_64bit_cas();
    check_many_threads_cas();

    check_8bit_incr();
    check_16bit_incr();
    check_32bit_incr();
    check_many_threads_incr();
    return 0;
}
