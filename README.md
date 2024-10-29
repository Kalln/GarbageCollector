# Church
## Reference counter garbage collector
This reference counting garbage collector is simpler way of managing memory when developing a C program. It eliminates the use of free and malloc/calloc and will ensure the freeing of memory at the end of the program, eliminating any memory leaks. It eliminates memory leaks by keeping track of what objects have no reference count, making it garbage.

This module can be used in another application to avoid thinking as much about memory leaks and letting the garbage collector do its work. It's makes memory management more simple.

## Build

This project uses gcc to compile the program.

To be able to run the program the following commands can be executed:
### Compile
To compile this project the following command can be run:
```
    make
```

### Run demonstration
With this project a demo has been supplied to showcase the reference counter garbage collector. [Some more information about the demo?]

To run this demo the following command can be run:

```
    make demo
```
### Run unit tests
Multiple unit- and integrationtests have been created to test the reference counter. To run the tests, the following command can be executed:

```
    make test
```

### Run memory leak test
Checking memory leaks on the previous tests can be run with following command:
```
    make memtest
```



### Contributors:
- Alicia S.
- Emil E.
- Emmeli B.
- Karl Widborg Kortelainen.
- Mauritz H. S.
- Thomas N.

(names removed for privacy reasons)