# c-map

Very simple C map / lookup table [single file](map.h) implementation, works for any type.

## Functions

```c
```

## Error Handling

There is no error handling, BUT a sophisticated amount of errors can happen:

- Different kinds of memory allocation / reallocation
- Out of bounds access

If any error occurs, it is one that should never even happen in the first
place, hence I just `exit(1)`. For that, see [Debuggability](#Debuggability).

## Debuggability

All but the query- and freeing functions handle the `NDEBUG` preprocessor
definition.

If it is not defined, and an error occurs (out of bounds, allocation) a message
will be printed describing the error and from where it originated, in the form
of `file:line-nb:function() map error: message`.

If it is defined, the error from where it originated is left out in any case.
Error checking is still enabled.

## Example

See [examples/main.c](examples/main.c):

## Single-Header Generation

I've kept the actual [source](src/array.c) and [header](src/array.h) code in the subdirectory `src`.

To create the [single-header file](array.h), use the [python script](gen-single-file.py).

## See also

- <https://github.com/rphii/c-array>


