# C-_Sanitizer_Study
Repositoty to test compiler's options "fsanitize" in different scenarius.

The aim is use differents tools as valgrind, compiler options (-fsanitize), and others to detect undefined behaviors in C/C++ code.


## How to compile

To compile with the default option, just run:

```cmake .. -DCMAKE_BUILD_TYPE=release && make```

To compile with debugging symbols, run:

```cmake .. -DCMAKE_BUILD_TYPE=debug && make```

To compile with debugging symbols and -fsanitize options, run:

```cmake .. -DCMAKE_BUILD_TYPE=debug_sanitize && make```

## Example source/address/stack_usage_out_scope

This app has an error that accesses memory out of scope, an array definided in the stack of a function is used to define a pointer that is returned and used outside the function, causing undefined behavior. 

#### Compiling and running the app in debug mode.

```
./bin/stack_usage_out_scope 
60
0x7ffc765b4a44
Value pointed by pointer 80
Address stored in pointer 0x7ffc765b4a44
Execution time: 72 microseconds
``` 
As we can see, the app runs without any error.

#### Compiling and running the app with valgrind in debug mode 
obs: in release mode works too, but doesn't show the line number where the error occurs.

```
valgrind ./bin/stack_usage_out_scope 
==100159== Memcheck, a memory error detector
==100159== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==100159== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==100159== Command: ./bin/stack_usage_out_scope
==100159== 
60
0x1ffefff954
==100159== Conditional jump or move depends on uninitialised value(s)
==100159==    at 0x49B7A7D: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==100159==    by 0x49C83C1: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==100159==    by 0x109401: main (stack_usage_out_scope.cpp:23)
==100159== 
==100159== Use of uninitialised value of size 8
```
One interesting point to note is that valgrind detect the problem at print line "std::cout << ...", removing this line makes valgrind not detect the problem.


#### Compiling and running the app in debug_sanitize mode

```
./bin/stack_usage_out_scope 
60
0x711349500044
=================================================================
==102097==ERROR: AddressSanitizer: stack-use-after-return on address 0x711349500044 at pc 0x6076d68ffd25 bp 0x7fff37f37510 sp 0x7fff37f37500
WRITE of size 4 at 0x711349500044 thread T0
    #0 0x6076d68ffd24 in main /home/willian/workspace/pessoal_dev/C-_Sanitizer_Study/source/address/stack_usage_out_scope.cpp:22
    #1 0x71134b42a1c9 in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x71134b42a28a in __libc_start_main_impl ../csu/libc-start.c:360
    #3 0x6076d68ff284 in _start (/home/willian/workspace/pessoal_dev/C-_Sanitizer_Study/build/bin/stack_usage_out_scope+0x1284) (BuildId: e9bc69e43845bbc07c0f03580508790f416c667d)

Address 0x711349500044 is located in stack of thread T0 at offset 68 in frame
    #0 0x6076d68ff358 in define_pointer(int**) /home/willian/workspace/pessoal_dev/C-_Sanitizer_Study/source/address/stack_usage_out_scope.cpp:6

  This frame has 1 object(s):
    [48, 128) 'a' (line 8) <== Memory access at offset 68 is inside this variable
```

A important point here is that the AddressSanitizer is able to detect the exact line where the problem occurs (*pointer = 80), even if we remove the print line "std::cout << ...", the sanitizer will still be able to detect the stack-use-after-return error.

Comparing the two tools, valgrind and AddressSanitizer, we can see that the AddressSanitizer present much more details about the problem, this present the address that the problem occurs (0x711349500044), the type of error (stack-use-after-return), and also the function where the variable was defined (define_pointer).



