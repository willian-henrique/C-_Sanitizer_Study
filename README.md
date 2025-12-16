# C++ Sanitizer and Valgrind Study

Repositoty to test compiler options "fsanitize=(memory or thread)". The book [Asynchronous Programming with C++](https://www.packtpub.com/en-in/product/asynchronous-programming-with-c-9781835884256), was used as reference to create it. Preseting collection of small C++ programs with common bugs, designed to demonstrate and compare the capabilities of GCC Sanitizers and Valgrind.

The goal is to show:
-   How to compile code with Sanitizers.
-   What the error reports look like.
-   How the same bugs are detected by different tools.

## Tools Compared

1.  **Compiler Sanitizers (`-fsanitize=...`)**: A family of powerful bug detectors built into GCC and Clang. They work by instrumenting the code at compile time.
    *   **AddressSanitizer (ASan)**: Detects memory errors like use-after-free, buffer overflows, and use-after-return.
    *   **ThreadSanitizer (TSan)**: Detects data races in multi-threaded code.
2.  **Valgrind**: A dynamic analysis tool.
    *   **Memcheck**: The default tool, detects memory errors. 
    *   **Helgrind/DRD**: Tools for detecting threading errors.


## How to compile

To compile with the default option, just run:

```cmake . -DCMAKE_BUILD_TYPE=release && make```

To compile with debugging symbols, run:

```cmake . -DCMAKE_BUILD_TYPE=debug && make```

To compile with debugging symbols and -fsanitize=address options, run:

```cmake . -DCMAKE_BUILD_TYPE=debug_memory && make```

or, debbuging symbles and -fsanitize=thread:

```cmake . -DCMAKE_BUILD_TYPE=debug_thread && make```



## Example source/address/stack_usage_out_scope (memory)

This app has an error that accesses memory out of scope. An array defined in the stack of a function is used to define a pointer that is returned and used outside the function, causing undefined behavior. 

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
Obs: In release mode, it works too, but doesn't show the line number where the error occurs.

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
```
One interesting point to note, Valgrind detects the problem at print line "std::cout << ...", removing this line makes Valgrind not detect the problem.
```


#### Compiling and running the app in debug_memory mode

```
./bin/stack_usage_out_scope 
60
0x711349500044
=================================================================
==102097==ERROR: AddressSanitizer: stack-use-after-return on address 0x711349500044 at pc 0x6076d68ffd25 bp 0x7fff37f37510 sp 0x7fff37f37500
WRITE of size 4 at 0x711349500044 thread T0
    #0 0x6076d68ffd24 in main {...}/C-_Sanitizer_Study/source/address/stack_usage_out_scope.cpp:22
    #1 0x71134b42a1c9 in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #2 0x71134b42a28a in __libc_start_main_impl ../csu/libc-start.c:360
    #3 0x6076d68ff284 in _start ({...}/C-_Sanitizer_Study/build/bin/stack_usage_out_scope+0x1284) (BuildId: e9bc69e43845bbc07c0f03580508790f416c667d)

Address 0x711349500044 is located in stack of thread T0 at offset 68 in frame
    #0 0x6076d68ff358 in define_pointer(int**) {...}/C-_Sanitizer_Study/source/address/stack_usage_out_scope.cpp:6

  This frame has 1 object(s):
    [48, 128) 'a' (line 8) <== Memory access at offset 68 is inside this variable
```

An important point here is that the AddressSanitizer is able to detect the exact line where the problem occurs (*pointer = 80), even if we remove the print line "std::cout << ...", the sanitizer is still able to detect the stack-use-after-return error.

Comparing the two tools, Valgrind and AddressSanitizer, we can see that AddressSanitizer presents more details about the problem. This presents the address that the problem occurs (0x711349500044), the type of error (stack-use-after-return), and also the function where the variable was defined (define_pointer).

## Example source/thread/concurrency_problem (thread)

#### Compiling and running the app with valgrind in debug mode 

```
valgrind --tool=helgrind ./bin/concurrency_problem 
==1396921== Helgrind, a thread error detector
==1396921== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==1396921== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==1396921== Command: ./bin/concurrency_problem
==1396921== 
==1396921== ---Thread-Announcement------------------------------------------
==1396921== 
==1396921== Thread #3 was created
==1396921==    at 0x4C4FA53: clone (clone.S:76)
==1396921==    by 0x4C4FBD2: __clone_internal_fallback (clone-internal.c:64)
==1396921==    by 0x4C4FBD2: __clone_internal (clone-internal.c:109)
==1396921==    by 0x4BC254F: create_thread (pthread_create.c:297)
==1396921==    by 0x4BC31A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==1396921==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==    by 0x4966EB0: std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x109494: std::thread::thread<void (&)(), , void>(void (&)()) (std_thread.h:173)
==1396921==    by 0x1092A4: main (concurrency_problem.cpp:15)
==1396921== 
==1396921== ---Thread-Announcement------------------------------------------
==1396921== 
==1396921== Thread #2 was created
==1396921==    at 0x4C4FA53: clone (clone.S:76)
==1396921==    by 0x4C4FBD2: __clone_internal_fallback (clone-internal.c:64)
==1396921==    by 0x4C4FBD2: __clone_internal (clone-internal.c:109)
==1396921==    by 0x4BC254F: create_thread (pthread_create.c:297)
==1396921==    by 0x4BC31A4: pthread_create@@GLIBC_2.34 (pthread_create.c:836)
==1396921==    by 0x4854975: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==    by 0x4966EB0: std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x109494: std::thread::thread<void (&)(), , void>(void (&)()) (std_thread.h:173)
==1396921==    by 0x10928E: main (concurrency_problem.cpp:14)
==1396921== 
==1396921== ----------------------------------------------------------------
==1396921== 
==1396921== Possible data race during read of size 4 at 0x10C01C by thread #3
==1396921== Locks held: none
==1396921==    at 0x10924B: decrease() (concurrency_problem.cpp:10)
==1396921==    by 0x109AFE: void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) (invoke.h:61)
==1396921==    by 0x109AAA: std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) (invoke.h:96)
==1396921==    by 0x109A4B: void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) (std_thread.h:301)
==1396921==    by 0x109A1B: std::thread::_Invoker<std::tuple<void (*)()> >::operator()() (std_thread.h:308)
==1396921==    by 0x1099FB: std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() (std_thread.h:253)
==1396921==    by 0x4966DB3: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==    by 0x4BC2AA3: start_thread (pthread_create.c:447)
==1396921==    by 0x4C4FA63: clone (clone.S:100)
==1396921== 
==1396921== This conflicts with a previous write of size 4 by thread #2
==1396921== Locks held: none
==1396921==    at 0x10923A: increase() (concurrency_problem.cpp:6)
==1396921==    by 0x109AFE: void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) (invoke.h:61)
==1396921==    by 0x109AAA: std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) (invoke.h:96)
==1396921==    by 0x109A4B: void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) (std_thread.h:301)
==1396921==    by 0x109A1B: std::thread::_Invoker<std::tuple<void (*)()> >::operator()() (std_thread.h:308)
==1396921==    by 0x1099FB: std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() (std_thread.h:253)
==1396921==    by 0x4966DB3: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==  Address 0x10c01c is 0 bytes inside data symbol "globalVar"
==1396921== 
==1396921== ----------------------------------------------------------------
==1396921== 
==1396921== Possible data race during write of size 4 at 0x10C01C by thread #3
==1396921== Locks held: none
==1396921==    at 0x109254: decrease() (concurrency_problem.cpp:10)
==1396921==    by 0x109AFE: void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) (invoke.h:61)
==1396921==    by 0x109AAA: std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) (invoke.h:96)
==1396921==    by 0x109A4B: void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) (std_thread.h:301)
==1396921==    by 0x109A1B: std::thread::_Invoker<std::tuple<void (*)()> >::operator()() (std_thread.h:308)
==1396921==    by 0x1099FB: std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() (std_thread.h:253)
==1396921==    by 0x4966DB3: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==    by 0x4BC2AA3: start_thread (pthread_create.c:447)
==1396921==    by 0x4C4FA63: clone (clone.S:100)
==1396921== 
==1396921== This conflicts with a previous write of size 4 by thread #2
==1396921== Locks held: none
==1396921==    at 0x10923A: increase() (concurrency_problem.cpp:6)
==1396921==    by 0x109AFE: void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) (invoke.h:61)
==1396921==    by 0x109AAA: std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) (invoke.h:96)
==1396921==    by 0x109A4B: void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) (std_thread.h:301)
==1396921==    by 0x109A1B: std::thread::_Invoker<std::tuple<void (*)()> >::operator()() (std_thread.h:308)
==1396921==    by 0x1099FB: std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() (std_thread.h:253)
==1396921==    by 0x4966DB3: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33)
==1396921==    by 0x4854B7A: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==1396921==  Address 0x10c01c is 0 bytes inside data symbol "globalVar"
==1396921== 
==1396921== 
==1396921== Use --history-level=approx or =none to gain increased speed, at
==1396921== the cost of reduced accuracy of conflicting-access information
==1396921== For lists of detected and suppressed errors, rerun with: -s
==1396921== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 1 from 1)
```

### Compiling and running the app with in debug_thread mode.

```
setarch $(uname -m) -R ./bin/concurrency_problem
==================
WARNING: ThreadSanitizer: data race (pid=1383374)
  Read of size 4 at 0x55555555901c by thread T2:
    #0 decrease() {...}/C-_Sanitizer_Study/source/thread/concurrency_problem.cpp:10 (concurrency_problem+0x13a2) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #1 void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) /usr/include/c++/14/bits/invoke.h:61 (concurrency_problem+0x214c) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #2 std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) /usr/include/c++/14/bits/invoke.h:96 (concurrency_problem+0x20a1) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #3 void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) /usr/include/c++/14/bits/std_thread.h:301 (concurrency_problem+0x1ff6) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #4 std::thread::_Invoker<std::tuple<void (*)()> >::operator()() /usr/include/c++/14/bits/std_thread.h:308 (concurrency_problem+0x1f98) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #5 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() /usr/include/c++/14/bits/std_thread.h:253 (concurrency_problem+0x1f4a) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #6 <null> <null> (libstdc++.so.6+0xecdb3) (BuildId: ca77dae775ec87540acd7218fa990c40d1c94ab1)

  Previous write of size 4 at 0x55555555901c by thread T1:
    #0 increase() {...}/C-_Sanitizer_Study/source/thread/concurrency_problem.cpp:6 (concurrency_problem+0x1368) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #1 void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) /usr/include/c++/14/bits/invoke.h:61 (concurrency_problem+0x214c) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #2 std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) /usr/include/c++/14/bits/invoke.h:96 (concurrency_problem+0x20a1) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #3 void std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) /usr/include/c++/14/bits/std_thread.h:301 (concurrency_problem+0x1ff6) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #4 std::thread::_Invoker<std::tuple<void (*)()> >::operator()() /usr/include/c++/14/bits/std_thread.h:308 (concurrency_problem+0x1f98) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #5 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() /usr/include/c++/14/bits/std_thread.h:253 (concurrency_problem+0x1f4a) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)
    #6 <null> <null> (libstdc++.so.6+0xecdb3) (BuildId: ca77dae775ec87540acd7218fa990c40d1c94ab1)

  Location is global 'globalVar' of size 4 at 0x55555555901c (concurrency_problem+0x501c)

  Thread T2 (tid=1383377, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1022 (libtsan.so.2+0x5ac1a) (BuildId: 38097064631f7912bd33117a9c83d08b42e15571)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xeceb0) (BuildId: ca77dae775ec87540acd7218fa990c40d1c94ab1)
    #2 main {...}/C-_Sanitizer_Study/source/thread/concurrency_problem.cpp:15 (concurrency_problem+0x1420) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)

  Thread T1 (tid=1383376, finished) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1022 (libtsan.so.2+0x5ac1a) (BuildId: 38097064631f7912bd33117a9c83d08b42e15571)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xeceb0) (BuildId: ca77dae775ec87540acd7218fa990c40d1c94ab1)
    #2 main {...}/C-_Sanitizer_Study/source/thread/concurrency_problem.cpp:14 (concurrency_problem+0x140a) (BuildId: 9d2cb41e5fe007f4cf5e1ebf928e7098591ba08a)

SUMMARY: ThreadSanitizer: data race {...}/C-_Sanitizer_Study/source/thread/concurrency_problem.cpp:10 in decrease()
==================
ThreadSanitizer: reported 1 warnings

```

Comparing the two tools, both identify the problem, but the output of the --fsanitizer looks more friendly, less verbose, and presents the name of the variable where the race condition occurs
