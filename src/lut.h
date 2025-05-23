#ifndef LUT_SIMPLE_H

#define println(msg, ...)   printf(msg "\n", ##__VA_ARGS__)

#include <stddef.h>
#include <stdbool.h>

#define lut_free(lut)   _lut_free(&lut)
#define lut_add(lut, key, val)  (*(typeof(lut))(_lut_add2(&lut, sizeof(*lut), (void *)(uintptr_t)key)) = val)
#define lut_get(lut, key)       (typeof(lut))(_lut_get(lut, (void *)(uintptr_t)key))
#define lut_del(lut, key)       (_lut_del(lut, (void *)(uintptr_t)key))
#define lut_config(lut, key_type, key_cmp, key_hash)    _lut_config(&lut, sizeof(key_type), key_cmp, key_hash)
#define lut_config_free(lut, free_key, free_val)        _lut_config_free(&lut, free_key, free_val)

#define lut_it_all(lut, out_type, out_key, out_val)   \
        out_type out_key; \
        typeof(* lut) out_val; \
        for(LutMeta *it = lut_it_next(lut, 0); \
            it, \
                it ? ((out_key = *(typeof(out_key) *)it->key)) || true : 0, \
                it ? ((out_val = *(typeof(out_val) *)it->val)) || true : 0; \
            it = lut_it_next(lut, it) \
        ) \
    
typedef size_t (*LutHash)(void *);
typedef int (*LutCmp)(void *, void *);
typedef void (*LutFree)(void *);

void _lut_config_free(void *lut, LutFree free_key, LutFree free_val);
void lut_print(void *lut);
void lut_clear(void *lut);
void lut_set(void *lut, void *key);
void *_lut_get(void *lut, void *key);
void lut_ref(void *lut, void *key);
void lut_copy(void *lut, void *key);
void _lut_del(void *lut, void *key);
size_t lut_len(void *lut);

size_t lut_cap(void *lut);
void *_lut_add2(void *lut, size_t size_val, void *key);
void _lut_free(void *lut);
void _lut_config(void *lut, size_t key_size, LutCmp key_cmp, LutHash key_hash);

void lut_set_hash(void *lut, LutHash hash);
void lut_set_cmp(void *lut, LutCmp cmp);

typedef struct LutMeta {
    size_t hash;
    void *val;
    void *key;
} LutMeta;

LutMeta *lut_it_next(void *lut, LutMeta *prev);
void *_lut_grow2(void *lut, size_t size_key, size_t size_val, size_t width);

#define LUT_SIMPLE_H
#endif

