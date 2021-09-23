#include "test.h"
#include "libtest.h"
#include <stdint.h>
#include "yutil/keywords.h"
#include "yutil/time.h"

void test_time(void)
{
	timeval_t *tv = NULL;
	int64_t time1 = get_time_ms();
	it_b("get_time_ms() should work", time1 != 0, TRUE);

	it_b("get_time_us() should work", get_time_us() != 0, TRUE);

	sleep_s(1);

	it_b("get_time_delta() should work", get_time_delta(time1) != 0, TRUE);
	it_b("sleep_ms() should work", get_time_delta(time1) >= 90, TRUE);
	int64_t time2 = get_time_ms();

	sleep_ms(100);

	it_b("sleep_ms() should work", get_time_delta(time2) >= 90, TRUE);

	it_b("get_time_of_day() should work", get_time_of_day(tv), TRUE);
}