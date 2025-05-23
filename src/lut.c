#include "lut.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define LUT_EMPTY       SIZE_MAX

#define lut_get_at(l, index)  &l->data[index]

#define lut_width_cap(width)  (!!width * (size_t)1ULL << width)
#define lut_base(lut)   ((lut) - offsetof(Lut, data))

#define lut_must_exist(lut)   do { \
        if(!lut) { \
            lut = lut_init(); \
            /* println("lut didn't exist, mallocd on %p", lut); */ \
        } \
    } while(0)

#define lut_error(msg, ...)     do { \
        printf("\n" "lookup error: " msg "\n" , ##__VA_ARGS__); \
        /*printf("\n" VEC_DEBUG_FMT "lookup error: " msg "\n" VEC_DEBUG_ARGS, ##__VA_ARGS__);*/ \
        exit(1); \
    } while(0)

typedef struct Lut {
    struct {
        size_t size;
        LutHash hash;
        LutCmp cmp;
        LutFree f;
    } key;
    struct {
        LutFree f;
    } val;
    size_t used;
    size_t width;
    LutMeta *data;
} Lut;

static inline void *lut_init(void) {
    Lut *l = malloc(sizeof(Lut));
    if(!l) {
        lut_error("could not create lookup table");
    }
    memset(l, 0, sizeof(*l));
    return &l->data;
}

void *lutmeta_key(LutMeta *meta) {
    return meta->key;
}
void *lutmeta_val(LutMeta *meta) {
    return meta->val;
}

void lutmeta_activate(Lut *lut, LutMeta *item, size_t size_val) {
    size_t cap = lut_width_cap(lut->width);
    size_t i = item - lut->data;
    item->key = (void *)((unsigned char *)lut->data + (sizeof(LutMeta) * cap) + (i * (lut->key.size + size_val)));
    item->val = (void *)((unsigned char *)lut->data + (sizeof(LutMeta) * cap) + (i * (lut->key.size + size_val)) + lut->key.size);
    ++lut->used;
}

void lutmeta_deactivate(Lut *lut, LutMeta *item) {
    item->hash = LUT_EMPTY;
    /* wow, this is cursed XD */
    if(item->key && lut->key.f) lut->key.f((uintptr_t *) * (uintptr_t *)item->key);
    if(item->val && lut->val.f) lut->val.f((uintptr_t *) * (uintptr_t *)item->val);
    item->key = 0;
    item->val = 0;
    --lut->used;
}

void _lut_config(void *lut, size_t key_size, LutCmp key_cmp, LutHash key_hash) {
    void **p = lut;
    lut_must_exist(*p);
    Lut *l = lut_base(*p);
    l->key.hash = key_hash;
    l->key.cmp = key_cmp;
    l->key.size = key_size;
}

void _lut_config_free(void *lut, LutFree free_key, LutFree free_val) {
    void **p = lut;
    lut_must_exist(*p);
    Lut *l = lut_base(*p);
    l->key.f = free_key;
    l->val.f = free_val;
}

void lut_set_hash(void *lut, LutHash hash) {
    void **p = lut;
    lut_must_exist(*p);
    Lut *l = lut_base(*p);
    l->key.hash = hash;
}

void lut_set_cmp(void *lut, LutCmp cmp) {
    void **p = lut;
    lut_must_exist(*p);
    Lut *l = lut_base(*p);
    l->key.cmp = cmp;
}

LutMeta *_lut_get_item(Lut *lut, void *key, size_t hash, bool intend_to_set) {
    size_t perturb = hash >> 5;
    size_t mask = ~(SIZE_MAX << lut->width);
    size_t i = mask & hash;
    LutMeta *item = lut_get_at(lut, i);
    for(;;) {
        if(intend_to_set && item->hash == LUT_EMPTY) break;
        if(item->hash == hash) {
            void *meta_key = lutmeta_key(item);
            uintptr_t meta_key_real = *(uintptr_t *)meta_key;
            if(!lut->key.cmp((uintptr_t *)meta_key_real, key)) {
                return item;
            }
        }
        perturb >>= 5;
        i = mask & (i * 5 + perturb + 1);
        /* get NEXT item */
        item = lut_get_at(lut, i);
    }
    return item;
}

LutMeta *lut_it_next(void *lut, LutMeta *prev) {
    if(!lut) return 0;
    Lut *l = lut_base(lut);
    size_t i = prev ? (prev - l->data) + 1 : 0;
    if(i >= lut_width_cap(l->width)) {
        return 0;
    }
    size_t cap = lut_width_cap(l->width);
    do {
        if(l->data[i].hash == LUT_EMPTY) continue;
        return &l->data[i];
    } while(++i < cap);
    return 0;
}

size_t lut_cap(void *lut) {
    Lut *l = lut_base(lut);
    return lut_width_cap(l->width);
}

void lut_print(void *lut) {
    Lut *l = lut_base(lut);
    // println("lut base %p (.data %p, .width %zu", l, lut, l->width);
    for(size_t i = 0; i < lut_width_cap(l->width); ++i) {
        LutMeta *item = lut_get_at(l, i);
        printf("  lut item %zu : %p, %p + %p, ", i, item, item->key, item->val);
        if(item->hash == LUT_EMPTY) println("(empty)");
        else println("hash %zx", item->hash);
    }
}

size_t lut_len(void *lut) {
    Lut *l = lut_base(lut);
    return l->used;
}

void *_lut_grow2(void *lut, size_t size_key, size_t size_val, size_t width) {
    Lut zero = {0};
    Lut *l = lut ? lut_base(lut) : &zero;
    if(lut && width <= l->width) return lut;
    size_t cap_old = lut_width_cap(l->width);
    size_t cap = lut_width_cap(width);
    size_t bytes = sizeof(Lut) + (sizeof(LutMeta) + size_key + size_val) * cap;
    Lut *grown = malloc(bytes);
    if(!grown) {
        lut_error("could not allocate lookup table");
    }
    memcpy(grown, l, sizeof(*grown));
    grown->used = 0; /* will be increased with activate */
    grown->width = width;
    grown->key.size = size_key;
    grown->data = (LutMeta *)((unsigned char *)grown + sizeof(Lut));
    /* re-add values */
    for(size_t i = 0; i < cap; ++i) {
        LutMeta *item = lut_get_at(grown, i);
        item->hash = LUT_EMPTY;
        item->key = 0;
        item->val = 0;
    }
    if(lut) {
        for(size_t i = 0; i < cap_old; ++i) {
            LutMeta *src = lut_get_at(l, i);
            // println("  re-add src %3zu : %p", i, src);
            if(src->hash == LUT_EMPTY) continue;
            size_t hash = src->hash;
            LutMeta *item = _lut_get_item(grown, lutmeta_key(src), hash, true);
            item->hash = src->hash;
            lutmeta_activate(grown, item, size_val);
            memcpy(item->val, src->val, size_val);
            memcpy(item->key, src->key, size_key);
        }
        free(l);
    }
    return &grown->data;
}

void *_lut_add2(void *lut, size_t size_val, void *key) {
    void **p = lut;
    lut_must_exist(*p);
    Lut *l = lut_base(*p);
    assert(l->key.size);
    size_t size = size_val + l->key.size + sizeof(LutMeta);
    if(3 * l->used / 2 >= lut_width_cap(l->width)) {
        *p = _lut_grow2(*p, l->key.size, size_val, l->width + 2);
    }
    l = lut_base(*p);
    size_t hash = l->key.hash(key) % LUT_EMPTY;
    LutMeta *item = _lut_get_item(l, key, hash, true);
    if(item->hash != LUT_EMPTY) {
        lutmeta_deactivate(l, item);
    }
    lutmeta_activate(l, item, size_val);
    item->hash = hash;
    void *meta_key = lutmeta_key(item);
    memcpy(meta_key, &key, l->key.size);
    void *meta_val = lutmeta_val(item);
    return meta_val;
}

void *_lut_get(void *lut, void *key) {
    Lut *l = lut_base(lut);
    LutMeta *got = _lut_get_item(l, key, l->key.hash(key), false);
    return got->val;
}

void _lut_free(void *lut) {
    void **p = lut;
    if(!*p) return;
    Lut *l = lut_base(*p);
    if(l->key.f || l->val.f) {
        for(size_t i = 0; i < lut_width_cap(l->width); ++i) {
            LutMeta *item = &l->data[i];
            lutmeta_deactivate(l, item);
        }
    }
    free(l);
    *p = 0;
}

void _lut_del(void *lut, void *key) {
    Lut *l = lut_base(lut);
    LutMeta *item = _lut_get_item(l, key, l->key.hash(key), false);
    if(item) {
        lutmeta_deactivate(l, item);
    }
}

