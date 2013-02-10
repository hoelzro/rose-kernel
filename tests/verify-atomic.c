#include <assert.h>
#include <rose/atomic.h>
#include <rose/stdint.h>

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

int
main(void)
{
    check_8bit_cas();
    check_16bit_cas();
    check_32bit_cas();
    check_64bit_cas();
    return 0;
}
