#ifndef MAP_SIMPLE_H

#include <stddef.h>
#include <stdbool.h>

/* header stuff {{{ */

typedef size_t (*MapHash)(void *);
typedef int (*MapCmp)(void *, void *);
typedef void (*MapFree)(void *);

typedef struct MapMeta {
    size_t hash;
    void *val;
    void *key;
} MapMeta;

/*}}}*/

/* available functions + utility {{{ */

size_t map_len(void *map);
size_t map_cap(void *map);
void map_clear(void *map);

#define map_config_key(map, key_type, key_cmp, key_hash)    _map_config_key(&map, sizeof(key_type), key_cmp, key_hash)
#define map_config_free(map, free_key, free_val)            _map_config_free(&map, free_key, free_val)

#define map_set(map, key, val)  (*(typeof(map))(_map_set(&map, sizeof(*map), (void *)(uintptr_t)key)) = val)
#define map_get(map, key)       (typeof(map))(_map_get(map, (void *)(uintptr_t)key))
#define map_del(map, key)       (_map_del(map, (void *)(uintptr_t)key))
#define map_free(map)           _map_free(&map)

#define map_it_all(map, out_type, out_key, out_val)   \
        out_type out_key; \
        typeof(* map) out_val; \
        for(MapMeta *it = _map_it_next(map, 0); \
            it, \
                it ? ((out_key = *(typeof(out_key) *)it->key)) || true : 0, \
                it ? ((out_val = *(typeof(out_val) *)it->val)) || true : 0; \
            it = _map_it_next(map, it) \
        ) \

/*}}}*/

/* actual functions, probably don't use directly {{{ */

void _map_config_key(void *map, size_t key_size, MapCmp key_cmp, MapHash key_hash);
void _map_config_free(void *map, MapFree free_key, MapFree free_val);

void *_map_set(void *map, size_t size_val, void *key);
void *_map_get(void *map, void *key);
MapMeta *_map_it_next(void *map, MapMeta *prev);

void _map_del(void *map, void *key);
void _map_free(void *map);

/*}}}*/


#define MAP_SIMPLE_H
#endif

