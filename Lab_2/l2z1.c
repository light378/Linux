#include <stdio.h>
#include <time.h>
#include <limits.h>

int main() {
    printf("Size of time_t: %zu bytes\n", sizeof(time_t));

    // Перевірка signed/unsigned
    time_t t = (time_t)-1;

    if (t < 0) {
        printf("time_t is signed\n");

        // Обчислення максимуму
        int bits = sizeof(time_t) * 8;
        time_t max = (time_t)((1ULL << (bits - 1)) - 1);

        printf("Max time_t value: %lld\n", (long long)max);
        printf("Overflow will occur at: %s", ctime(&max));

    } else {
        printf("time_t is unsigned\n");

        time_t max = (time_t)(~(time_t)0);

        printf("Max time_t value: %llu\n", (unsigned long long)max);
        printf("Overflow will occur at: %s", ctime(&max));
    }

    return 0;
}

