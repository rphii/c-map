
single = []

with open('src/map.h') as header:
    single.append(header.read())

with open('src/map.c') as source:
    single.append('#ifdef MAP_IMPLEMENTATION')
    single.append(''.join(source.readlines()[1:]))     # skip #include "vec.h"
    single.append('#endif')

single = '\n'.join(single)
with open('map.h', 'w') as dest:
    dest.write(single)

