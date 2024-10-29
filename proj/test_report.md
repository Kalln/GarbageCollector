# Code coverage
Line coverage: 99,3 %
Branch coverage: 86,7%
These numbers were aquired by gcov and can be replicated by writing the command "make coverage" in the terminal.
Underneath, we have explained why some branches are not logical to test.

# Branches not taken
In our allocate function, we have a statement and a branch that is not tested. That is because it will only take the branch if the function calloc fails to allocate memory correctly. The untested part is however correct since it will check that if the pointer to the allocated memory is NULL, then it will return NULL. Therefore this code will work as intended if the imbuilt function calloc would not fail. - Line 265 in refmem.c

We also have a for loop in our get_struct_index function where we never not go through the loop atleast once. That is because in order to get to the for loop code, we need to both have atleast one object and a list. That means that the size of our object_list will always then be atleast one. Meaning the for loop will always execute atleast once. - Line 133 in refmem.c

We have another function called release which has a branch that is not taken. That is because we are checking if the object list is NULL and if the size of the list is equal to zero, in the same if statement. Meaning there will never be a scenario where the object list is NULL and then checking the size. - Line 170 in refmem.c

In our function rc, we like previously have a scenario where a branch is not taken. That is because if the object we take in is NULL, then we will not be able to get a struct from NULL. - Line 181 in refmem.c

Moreover, in our deallocate function we have a if statement where there are branches that are not taken. That is like mentioned previously not possible since we first check if our struct is NULL or not. If that is the case then the other checks will not be executed. - Line 303 in refmem.c

In our allocate_array function we have an if statement with an or, where we check if an size_t is 0 or not. If that is true then we do not check the other conditional since that would divide by 0 if the size_t is 0. - Line 291 in refmem.c

Lastly, in our default_destructor function we have an if statement with an or, where we check if ptr_list is NULL or not. If the ptr_list is NULL then the object list should not have a size bigger than zero, that is why that branch is not taken. Furthermore on line 113 we check if the potential_ptr is NULL or not. Since it is in the range of the memory the branch of it being NULL is not tested. - Lines 108 and 113 in refmem.c

# Unit testing
Each created function is tested in an isolated environment were only the tested function is tested. This ensures that each function functions as they should on an independent level.
The test suite can be run with `make test_refmem`

# Integration testing
We converted the inlupp2 demo to use the refmem library instead of free and malloc/calloc and we used the tests written during inlupp2 to test that the integration was successful. These tests are run with `make test`

# Regression testing
We ran a test suite through GitHub actions on every push which gave a result signaling whether any tests were broken by the change. We tried to only merge changes that passed all tests unless there were special circumstances.
The test suite can be run with `make test`
