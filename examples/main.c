#define VECTOR_IMPLEMENTATION
#define MAP_IMPLEMENTATION
#include "vector.h"
#include "../map.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define println(msg, ...)   printf(msg "\n", ##__VA_ARGS__)

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

int str_cmp(char **a, char **b) {
    return strcmp(*a, *b);
}

void str_free(char **s) {
    if(!*s) return;
    vec_free(*s);
}

int main(void) {

    unsigned long *ages = 0; /* set to 0! */
    ///map_config_key(ages, char *, (MapCmp)str_cmp, (MapHash)str_hash, (MapFree)str_free);
    map_config_key(ages, char *, (MapCmp)str_cmp, (MapHash)str_hash, 0);

    char *names[] = { /* random names from https://1000randomnames.com/ */
        str_create("Maxine Houston"),
        str_create("Sylas Davis"),
        str_create("Mia Cross"),
        str_create("Fabian Hubbard"),
        str_create("Rosie Compton"),
        str_create("Abner Sanders"),
        str_create("Everleigh York"),
        str_create("Leandro Meyer"),
        str_create("Sara Ayers"),
        str_create("Ulises Saunders"),
    };
    const int n_names = sizeof(names)/sizeof(*names);

    println("/* add all names from the array */");
    for(size_t i = 0; i < n_names; ++i) {
        map_set(ages, &names[i], rand() % 100);
    }

    println("/* clear all names */");
    map_clear(ages);
    map_config_key(ages, char *, (MapCmp)str_cmp, (MapHash)str_hash, (MapFree)str_free);

    println("/* add all names again */");
    for(size_t i = 0; i < n_names; ++i) {
        map_set(ages, &names[i], rand() % 100);
    }

    println("/* delete every second person */");
    for(size_t i = 0; i < n_names; i += 2) {
        map_del(ages, &names[i]);
    }

    println("/* print everything remaining */");
    char *name;
    unsigned long age;
    map_it_all(ages, name, age) {
        println("%s is %lu years old", name, age);
    }

    println("/* print everything by accessing the array */");
    for(size_t i = 0; i < n_names; ++i) {
        char *name = names[i];
        unsigned long *age = map_get(ages, &name);
        if(age) {
            println("%s is %lu years old", name, *age);
        } else {
            println("%s has no entry", name);
        }
    }
    printf("%s", *&"lol");

    println("Length of table %zu, capacity %zu", map_len(ages), map_cap(ages));
    
    println("/* print everything by accessing the array */");
    map_free(ages);
    println("Final pointer %p", ages);

    return 0;
}


