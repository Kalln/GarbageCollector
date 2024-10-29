# Design Report

## **Overview**

The general idea of the garbage collector is that it should eliminate memory leaks when allocating data on the heap. This is done by seeing every allocation as an object. The object is thereafter stored in list where the reference counter can keep track of the object. By doing this it is possible to know what we have stored on the heap and make it easier for the user to code programs. A natural drawback is ofcourse that by letting a reference counter do this work for you, it's not as effective as handling the memory yourself.

## **Functions**

### void retain(obj *object);
Increase an objects reference counter. When creating a pointer (a reference). The function retain should be called for the obj pointer that was returned by allocate or allocate_array function before any other allocation, unless it should be free'd.

### void release(obj *object);
Whenever an object reference is reduced in any way. Release should be called on that object pointer. This will in the end, when the object pointer has 0 references, be free'd since there is nothing in the program that has a reference to that and will not be called anymore.

### size_t rc(obj *);
This function returns an integer that represents the amount of pointers pointing towards the given argument. If the given argument is NULL it returns 0.

### obj *allocate(size_t bytes, function1_t destructor);
Returns memory on the heap with a given byte size. IF the user wants, and it's encouraged, it is possible to supply the function with a destructor. This will make the garbage collector more effective. IF no destructor is given, the defualt destructor will be used as a default.

### obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);
This function is for when you want to allocate space for multiple objects of a certain size.
It calculates their total size

### void deallocate(obj *);
Delllocate is used to deallocate a given object. This will free that object and also release according to the given destructor of the object.


### void set_cascade_limit(size_t limit);
In different programs it might be useful to change the cacade limit in case the objects are fairly big. This can eliminate "hiccups" during runtime eliminating a domino effect when freeing large objects. On default this is set to biggest integer a size_t can hold which is different on different systems.


### size_t get_cascade_limit(void);
To be able to recieve what the current cascade limit is this function can be called.


### void cleanup(void);
If there have been no allocation in recent time and it a lot of unnessecery memory is used it's possible to run cleanup to clean all "garbage" also known as objects with zero references. NOTE. if program is not compiled with any optimization flags it's possible that stack overflow could happen since it's recursive.

### void shutdown(void);
Shutdown will remove all of the objects that has been allocated. It does not care about an objects reference. If all allocations have been correct it should be enough to run shutdown last in a program.


## Garbage collector process
The garbage collection process starts with the user using the defined allocate function that takes a size (in bytes) and a destructor. If no destructor is given the default destructor will be used. The allocation will have to be followed up with a retain function call to not "throw away" the recently allocated value. The retain is nesseccery to increase the reference counter of that object, else it might be free'd in the next allocation.

Allocation does also free up memory before allocating any new memory. This is to make sure that we always minimize our memory use. Another way to free memory is to release the object that has been allocated. When it reaches reference count 0 it will be deallocated and free'd. To be sure to use the least allocated memory it's advised for the user to retain and release objects as early as possible. If there is no need anymore for an object it's more effective to release it as soon as possible and therefore freeing up memory. Another, not as effective, way is to simply call shutdown that will free all allocated memory.

## **Design Choices**

**Reference counter size**
We chose to have our reference counter be a size_t since it's specifically designed to hold all possible sizes of something stored in memory.
This means it's perfectly capable of holding the amount of pointers towards an object.

**Default destructor**

To make it easier for a user to use our reference counter we decided to implement a default destructor. The destructor, that is described further in detail below, makes it possible for the user to forget about what kind of data will have to be freed. For example: object A references a (1) string and another (2) object B. This means that the string has 1 reference (from object A) and same with object B. If we decide that we no longer want to keep object A. Then the user can release that object and it will in some future be freed. Now since the string (1) and object B (2) had a reference from object A it means that these now have 0 references resulting in their free as well.

This is made possible by the help of a default destructor and is used when no given destructor is given when allocating data. Now it's important to note that using a user defined destructor is almost always better. Since the default destructor search for the whole allocated object for potential pointers it could, theoretically, take a while to find possible pointers because of the required search operation. This is not the case with a given destructor since it already knows the structure of the object and can thereby instantly release the needed allocated objects.

**Allocation**

The allocation process is done by using an object meta data that keeps track of the allocated object, size and reference counter. This makes it possible to keep track of how many other objects are referencing another object. The use of object meta data is nessecery to know when to free an object and also to know how many bytes are allocated if a certain amount of space needs to be deallocated.

**Cascade Limit**

A Cascade limit has been built that makes it possible to pause the freeing of objects. This is used when allocating a new object, where the collected garbage will be thrown out. The default cascade limit is SIZE_MAX so all objects will be freed immediately but it can be changed by the user if performance problems are encountered.

A Cascade limit has been built that makes it possible to pause the freeing of objects. This is used when allocating a new object, where the collected garbage will be thrown out. 

## Datastructures

The garbage collector uses linked list to store all of the object meta data. There is no other datastrcture that is used. Whenever we store an object its appended to the end of the list. Thereafter when the object is thrown out its removed from the list and free'd.

