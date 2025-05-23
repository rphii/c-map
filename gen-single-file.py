
single = []

with open('src/lut.h') as header:
    single.append(header.read())

with open('src/lut.c') as source:
    single.append('#ifdef LOOKUP_IMPLEMENTATION')
    single.append(''.join(source.readlines()[1:]))     # skip #include "vec.h"
    single.append('#endif')

single = '\n'.join(single)
with open('lookup.h', 'w') as dest:
    dest.write(single)

