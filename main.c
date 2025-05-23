#include "lut.h"
#include <stdio.h>
#include <stdint.h>

size_t hash(unsigned long value) {
    return (((value + 123) * 98752315538ULL) >> 4) ^ (((value + 987) * 87273391236ULL) << 6);
}

int cmp(unsigned long a, unsigned long b) {
    unsigned long result = a - b;
    println("compare %lu %lu = %lu", a, b, result);
    return result;
}

int main(void) {

    unsigned long *values = 0;
    //lut_set_cmp(&values, (LutCmp)cmp);
    //lut_set_hash(&values, (LutHash)hash);
    lut_config(values, int, (LutCmp)cmp, (LutHash)hash);

    int key = 0xBB;
    unsigned long value = 456;

#if 0
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 2);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 4);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 6);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 8);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 10);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 12);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 14);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 16);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 18);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 20);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 22);
    printf("lut_cap %zu\n", lut_cap(values));
    values = _lut_grow2(values, sizeof(*values)+sizeof(key)+sizeof(LutMeta), 24);
    printf("lut_cap %zu\n", lut_cap(values));
    lut_free(&values, sizeof(*values)+sizeof(key)+sizeof(LutMeta));
#endif

    lut_add(values, key, value);
    lut_print(values);
    lut_add(values, key, 0xBB);
    lut_print(values);

    key = 333;
    value = 999;
    lut_add(values, key, value);
    lut_print(values);

    value = 0xAA;
    lut_add(values, key, value);
    lut_print(values);
    lut_add(values, key, value);
    lut_print(values);
    lut_add(values, key, value);

    //*(typeof(value) *)_lut_add2(&values, sizeof(*values), (void *)(uintptr_t)key, sizeof(key)) = value;

    //*(typeof(value) *)_lut_add2(&values, sizeof(*values), (void *)(uintptr_t)key, sizeof(key)) = value;
    lut_print(values);

    /// *(typeof(value) *)_lut_add2(&values, sizeof(*values), (void *)(uintptr_t)key, sizeof(key)) = value;
    /// lut_print(values);
    /// *(typeof(value) *)_lut_add2(&values, sizeof(*values), (void *)(uintptr_t)key, sizeof(key)) = value;
    /// lut_print(values);
    /// //*(typeof(value) *)_lut_add2(&values, sizeof(*values), (void *)(uintptr_t)key, sizeof(key)) = value;
    /// //println("values ptr %p", values);

    lut_free(values);

#if 0
    for(int i = 0; i < 10; ++i) {

    }
#endif


    return 0;
}


