#ifndef LUT_SIMPLE_H

#define println(msg, ...)   printf(msg "\n", ##__VA_ARGS__)

#include <stddef.h>
#include <stdbool.h>

#define lut_free(lut)   _lut_free(&lut)
#define lut_add(lut, key, val)  (*(typeof(val) *)(_lut_add2(&lut, sizeof(*lut), (void *)(uintptr_t)key, sizeof(key))) = val)

//void *_lut_iter_all(lut, 

void lut_print(void *lut);
void lut_clear(void *lut);
void lut_set(void *lut, void *key);
//void *_lut_get(void *lut, size_val, void *key);
void lut_ref(void *lut, void *key);
void lut_copy(void *lut, void *key);
void lut_del(void *lut, void *key);

size_t lut_cap(void *lut);
void *_lut_add2(void *lut, size_t size_val, void *key, size_t size_key);
void _lut_free(void *lut);

typedef size_t (*LutHash)(void *);
typedef int (*LutCmp)(void *, void *);

void lut_set_hash(void *lut, LutHash hash);
void lut_set_cmp(void *lut, LutCmp cmp);

typedef struct LutMeta {
    size_t hash;
    void *key;
    void *val;
} LutMeta;

void *_lut_grow2(void *lut, size_t size_key, size_t size_val, size_t width);

// void lut_free(void *lut);
// void lut_clear(void *lut);
// //void lut_grow(void *lut);
// void *lut_copy(void *lut);
// void lut_once(void *lut);
// void lut_set(void *);
// void lut_get();
// void lut_get_kv();
// void lut_del();
// void lut_dump();
// void lut_iter_all();

#define LUT_SIMPLE_H
#endif

