#define VECTOR_IMPLEMENTATION
#define LOOKUP_IMPLEMENTATION
#include "vector.h"
#include "../lookup.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

size_t hash(unsigned long value) {
#if 0
    size_t hash = value;
#else
    size_t hash = (value + 12345ULL);
    hash = ((hash >> 32) ^ hash) * 0x85322754398623ULL;
    hash = ((hash >> 32) ^ hash) * 0x85322754398623ULL;
    hash = ((hash >> 32) ^ hash);
#endif
    return hash;
}

int cmp(unsigned long a, unsigned long b) {
    unsigned long result = a - b;
    //println("compare %lu %lu = %lu", a, b, result);
    return result;
}

char *str_create(char *str) {
    char *s = 0;
    size_t len = strlen(str);
    vec_resize(s, len + 1);
    memcpy(s, str, len);
    s[len] = 0;
    return s;
}

size_t str_hash(char *str)
{
    size_t hash = 5381;
    int c = 0;
    while((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int str_cmp(char *a, char *b) {
    return strcmp(a, b);
}

void str_free(char *s) {
    vec_free(s);
}

int main(void) {

    unsigned long *ages = 0;
    lut_config(ages, char *, (LutCmp)str_cmp, (LutHash)str_hash);
    lut_config_free(ages, (LutFree)str_free, 0);

    lut_add(ages, str_create("John"), 19);
    lut_add(ages, str_create("Lmao"), 25);
    lut_add(ages, str_create("Nice"), 69);
    lut_add(ages, str_create("Why"), 420);
    lut_del(ages, "Why");

    lut_it_all(ages, char *, name, age) {
        println("%s is %lu years old", name, age);
    }

    println("Length of table %zu", lut_len(ages));
    
    lut_free(ages);
    return 0;
}


