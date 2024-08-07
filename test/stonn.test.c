#include "stonn.h"
#include <time.h>
#include "unity.h"
void init_genrand64(unsigned long long seed);
unsigned long long genrand64_int64(void);
long long genrand64_int63(void);
double genrand64_real1(void);

void setUp() { init_genrand64(time(NULL)); }

void tearDown() { }

void test_stonn(void)
{
    long long expected, actual, elen;
    char pbuf[40];
    for (int tc = 0; tc < 1000; tc++) {
        expected = genrand64_int63();
        elen = snprintf(pbuf, 40, "%lld", expected);
        stonn(pbuf, elen, 10, &actual);
        TEST_ASSERT_EQUAL(expected, actual);

        expected = -genrand64_int63();
        elen = snprintf(pbuf, 40, "%lld", expected);
        stonn(pbuf, elen, 10, &actual);
        TEST_ASSERT_EQUAL(expected, actual);

        expected = genrand64_int63();
        elen = snprintf(pbuf, 40, "-%llx", expected);
        stonn(pbuf, elen, 16, &actual);
        TEST_ASSERT_EQUAL(-expected, actual);
    }

    elen = snprintf(pbuf, 40, "-%llu", ULLONG_MAX - 1000);
    TEST_ASSERT_NOT_EQUAL(0, stonn(pbuf, elen, 10, &actual));
    elen = snprintf(pbuf, 40, "%llu", ULLONG_MAX - 1000);
    TEST_ASSERT_NOT_EQUAL(0, stonn(pbuf, elen, 10, &actual));
    elen = snprintf(pbuf, 40, "%llu", ULLONG_MAX - 1000);
    TEST_ASSERT_NOT_EQUAL(0, stonn(pbuf, elen, 16, &actual));
}

void test_stounn(void)
{
    unsigned long long expected, actual, elen;
    char pbuf[100];
    for (int tc = 0; tc < 1000; tc++) {
        expected = genrand64_int64();
        elen = snprintf(pbuf, 100, "%llo", expected);
        stounn(pbuf, elen, 8, &actual);
        TEST_ASSERT_EQUAL(expected, actual);

        expected = genrand64_int64();
        elen = snprintf(pbuf, 100, "%llu", expected);
        stounn(pbuf, elen, 10, &actual);
        TEST_ASSERT_EQUAL(expected, actual);

        expected = genrand64_int64();
        elen = snprintf(pbuf, 100, "%llx", expected);
        stounn(pbuf, elen, 16, &actual);
        TEST_ASSERT_EQUAL(expected, actual);
    }

    elen = snprintf(pbuf, 256, "111111110111111111111111");
    stounn(pbuf, elen, 2, &actual);
    TEST_ASSERT_EQUAL(0b111111110111111111111111, actual);
}

void test_stonlf(void)
{
    double expected, actual;
    int elen, eexp;
    char pbuf[256];

    for (int tc = 0; tc < 1000; tc++) {
        expected = genrand64_real1();
        elen = snprintf(pbuf, 256, "%.14lf", expected);
        stonlf(pbuf, elen, &actual);
        TEST_ASSERT_EQUAL_FLOAT(expected, actual);

        expected = genrand64_real1() * ((genrand64_int63() % 5000) - 2000);
        elen = snprintf(pbuf, 256, "%.14lf", expected);
        stonlf(pbuf, elen, &actual);
        TEST_ASSERT_EQUAL_FLOAT(expected, actual);

        expected = genrand64_real1();
        eexp = (genrand64_int63() % 700) - 350;
        elen = snprintf(pbuf, 256, "%.14lfe%d", expected, eexp);
        stonlf(pbuf, elen, &actual);
        TEST_ASSERT_EQUAL_FLOAT(expected * pow(10.0, eexp), actual);
    }

    elen = snprintf(pbuf, 256, "7.2057594037927933e+16", expected);
    stonlf(pbuf, elen, &actual);
    TEST_ASSERT_EQUAL_FLOAT(7.2057594037927933e+16, actual);

    elen = snprintf(pbuf, 256, "199999999999999999999999999999999999.9");
    stonlf(pbuf, elen, &actual);
    TEST_ASSERT_EQUAL_FLOAT(199999999999999999999999999999999999.9, actual);

    elen = snprintf(pbuf, 256, "1e999999999999999999999999999");
    stonlf(pbuf, elen, &actual);
    TEST_ASSERT_FLOAT_IS_INF(actual);

    elen = snprintf(pbuf, 256, "1e-999999999999999999999999999");
    stonlf(pbuf, elen, &actual);
    TEST_ASSERT_EQUAL_FLOAT(0, actual);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_stonn);
    RUN_TEST(test_stounn);
    RUN_TEST(test_stonlf);
    return UNITY_END();
}
