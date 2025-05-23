#ifndef VEC_SIMPLE_H

#include <stddef.h>

/* debug optimization {{{ */

#if defined(NDEBUG)
#define VEC_DEBUG_INFO
#define VEC_DEBUG_ARG
#define VEC_DEBUG_ARGS
#define VEC_DEBUG_DEF
#define VEC_DEBUG_DEFS
#define VEC_DEBUG_FMT
#else
#define VEC_DEBUG_INFO  , __FILE__, __LINE__, __func__
#define VEC_DEBUG_ARG   file, line, func
#define VEC_DEBUG_ARGS  , VEC_DEBUG_ARG
#define VEC_DEBUG_DEF   const char *file, const int line, const char *func
#define VEC_DEBUG_DEFS  , VEC_DEBUG_DEF
#define VEC_DEBUG_FMT   "%s:%u:%s() "
#endif

/*}}}*/

/* functions for use {{{ */

#define vec_grow(vec, capacity)     _vec_grow(&vec VEC_DEBUG_INFO, sizeof(*vec), capacity)
#define vec_resize(vec, length)     _vec_resize(&vec VEC_DEBUG_INFO, sizeof(*vec), length)
#define vec_copy(vec)               _vec_copy(vec VEC_DEBUG_INFO, sizeof(*vec))
#define vec_push(vec, item)         (*(typeof(vec))_vec_push(&vec VEC_DEBUG_INFO, sizeof(*vec)) = item)
#define vec_pop(vec, item)          *(typeof(vec))_vec_pop(vec VEC_DEBUG_INFO, sizeof(*vec))
#define vec_at(vec, index)          *(typeof(vec))_vec_addr(vec VEC_DEBUG_INFO, sizeof(*vec), index)
#define vec_it(vec, index)          (typeof(vec))_vec_addr(vec VEC_DEBUG_INFO, sizeof(*vec), index)
#define vec_len(vec)                _vec_len(vec)
#define vec_cap(vec)                _vec_cap(vec)
#define vec_clear(vec)              _vec_clear(vec)
#define vec_free(vec)               _vec_free(&vec)

/*}}}*/

/* internal functions {{{ */

void _vec_grow(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity);
void _vec_resize(void *vec VEC_DEBUG_DEFS, size_t size, size_t length);
void *_vec_copy(void *vec VEC_DEBUG_DEFS, size_t size);
void *_vec_push(void *vec VEC_DEBUG_DEFS, size_t size);
void *_vec_pop(void *vec VEC_DEBUG_DEFS, size_t size);
void *_vec_addr(const void *vec VEC_DEBUG_DEFS, size_t size, size_t index);
size_t _vec_len(const void *vec);
size_t _vec_cap(const void *vec);
void _vec_clear(void *vec);
void _vec_free(void *vec);

/*}}}*/

#define VEC_SIMPLE_H
#endif


#ifdef VECTOR_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

static inline void *vec_init(VEC_DEBUG_DEF);
static inline void *_vec_grow2(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity);

#define vec_assert_arg(arg)     assert(arg && "null pointer argument!");
#define vec_base(vec)   ((vec) - offsetof(Vec, data))

#define vec_error(msg, ...)     do { \
        fprintf(stderr, "\n" VEC_DEBUG_FMT "vector error: " msg "\n" VEC_DEBUG_ARGS, ##__VA_ARGS__); \
        exit(1); \
    } while(0)

typedef struct Vec {
    size_t length;
    size_t capacity;
    void *data;
} Vec;

static inline void *vec_init(VEC_DEBUG_DEF) {
    Vec *v = malloc(sizeof(Vec));
    if(!v) {
        vec_error("failed creating vector");
    }
    memset(v, 0, sizeof(*v));
    return &v->data;
}

static inline void *_vec_grow2(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity) {
    if(!vec) {
        vec = vec_init(VEC_DEBUG_ARG);
    }
    Vec *v = vec_base(vec);
    if(capacity <= v->capacity) return vec;
    if(capacity * 2 < capacity) {
        vec_error("invalid allocation size: %zu", capacity);
    }
    size_t require = 2;
    while(require < capacity) require *= 2;
    size_t bytes = sizeof(Vec) + size * require;
    if((bytes - sizeof(Vec)) / size != require) {
        vec_error("array member of %zu bytes can't allocate %zu elements", size, require);
    }
    void *temp = realloc(v, bytes);
    if(!temp) {
        vec_error("failed allocation of: %zu elements (%zu bytes)", require, bytes);
    }
    v = temp;
    memset((void *)&v->data + size * v->capacity, 0, size * (require - v->capacity));
    v->capacity = require;
    return &v->data;
}

void _vec_grow(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity) {
    vec_assert_arg(vec);
    void **p = vec;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, capacity);
}

void _vec_resize(void *vec VEC_DEBUG_DEFS, size_t size, size_t length) {
    vec_assert_arg(vec);
    void **p = vec;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, length);
    Vec *v = vec_base(*p);
    v->length = length;
}

void *_vec_copy(void *vec VEC_DEBUG_DEFS, size_t size) {
    if(!vec) return 0;
    void *v = vec_init(VEC_DEBUG_ARG);
    size_t len = vec_len(vec);
    _vec_grow(&v VEC_DEBUG_ARGS, size, len);
    memcpy(v, vec, size * len);
    return v;
}

void *_vec_addr(const void *vec VEC_DEBUG_DEFS, size_t size, size_t index) {
    vec_assert_arg(vec);
#if !defined(NDEBUG)
    Vec *v = (void *)vec_base(vec);
    if(!(index < v->length)) {
        vec_error("index %zu is out of bounds %zu", index, v->length);
    }
#endif
    return (void *)vec + size * index;
}

void *_vec_push(void *vec VEC_DEBUG_DEFS, size_t size) {
    void **p = vec; Vec *v = *p ? vec_base(*p) : 0;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, v ? v->length + 1 : 1);
    v = vec_base(*p);
    size_t index = v->length++;
    return (void *)&v->data + size * index;
}

void *_vec_pop(void *vec VEC_DEBUG_DEFS, size_t size) {
    vec_assert_arg(vec);
    Vec *v = vec_base(vec);
#if !defined(NDEBUG)
    if(!v->length) {
        vec_error("no elements left to pop");
    }
#endif
    size_t index = --v->length;
    return vec + size * index;
}

void _vec_free(void *vec) {
    vec_assert_arg(vec);
    void **p = vec;
    if(!*p) return;
    Vec *v = vec_base(*p);
    free(v);
    *p = 0;
}

size_t _vec_len(const void *vec) {
    if(!vec) return 0;
    Vec *v = (Vec *)vec_base(vec);
    return v->length;
}

size_t _vec_cap(const void *vec) {
    if(!vec) return 0;
    Vec *v = (Vec *)vec_base(vec);
    return v->capacity;
}

void _vec_clear(void *vec) {
    if(!vec) return;
    Vec *v = vec_base(vec);
    v->length = 0;
}

#endif
