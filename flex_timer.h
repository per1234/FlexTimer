//
//  flextimer.h
//  TimeInterrupt
//
//  Created by Ludovic Bertsch on 19/06/2015.
//  Copyright (c) 2015 Ludovic Bertsch. All rights reserved.
//

#ifndef TimeInterrupt_flextimer_h
#define TimeInterrupt_flextimer_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <limits.h>

// Not really used: for further extensions!
typedef enum {
    FT_CATCH_UP = 0,
    FT_DO_NOT_CATCH_UP = 1,
    FT_REMOVE_WHEN_FINISHED = 4,
    FT_DEACTIVATE_WHEN_FINISHED = 8
} FT_timer_option_t;

// Not really used
#define FT_DEFAULT_PARAMETERS (CATCH_UP|REMOVE_WHEN_FINISHED)

// Super important value: to be used for "repeat" parameter!
#define FT_RUN_FOREVER (-1)

// Super important value: size of timer array (if using limited memory allocation scheme)
#define FT_NB_MAX_TIMERS (10)


//////////////////////////////////
// Configuration section!
//////////////////////////////////

//#define _FT_ARDUINO_MS
//#define _FT_ARDUINO_uS
//#define _FT_NORMAL_SECOND
//#define _FT_NORMAL_MS
//#define _FT_NORMAL_uS
//#define _FT_EXPERIMENTAL1
#define _FT_EXPERIMENTAL2

// Define the following for more checks! Basically these will be asserts...
// Use this while debugging, then remove it when you're sufficiently confident!
#define FT_PARANOIA

#ifdef _FT_ARDUINO_MS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000)
#define _FT_ARDUINO
#endif

#ifdef _FT_ARDUINO_uS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000000)
#define _FT_ARDUINO
#endif

#ifdef _FT_NORMAL_SECOND
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1)
#define _FT_UNIX
#endif

#ifdef _FT_NORMAL_MS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000)
#define _FT_UNIX
#endif

#ifdef _FT_NORMAL_uS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000000)
#define _FT_UNIX
#endif

#ifdef _FT_EXPERIMENTAL1
// 11 bit depth for testing to the limits!
typedef unsigned int time_measure_t;
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000)
#define _FT_EXPERIMENTAL
#define _FT_UNIX
#endif

#ifdef _FT_EXPERIMENTAL2
// 11 bit depth for testing to the limits!
typedef unsigned short time_measure_t;
//#define FT_TIME_MEASURE_COMPLETE_MASK (2047)
#define FT_TIME_MEASURE_COMPLETE_MASK (8191)
#define FT_LONGEST_SLEEP (100)
#define FT_ONE_SECOND (1000)
#define _FT_EXPERIMENTAL
#define _FT_UNIX
#endif

#ifdef _FT_EXPERIMENTAL3
typedef unsigned int time_measure_t;
#define FT_TIME_MEASURE_COMPLETE_MASK (UINT_MAX)
#define FT_LONGEST_SLEEP (1000000000)
#define FT_ONE_SECOND (1000000000)
#define _FT_EXPERIMENTAL
#define _FT_UNIX
#endif

#ifdef _FT_UNIX_GENERIC
#define FT_ONE_UNIT_IN_NANOS (1000000)
#endif

//////////////////////////////////

// Constants used internally!
#define FT_TIME_MEASURE_HALF_MASK (FT_TIME_MEASURE_COMPLETE_MASK/2)
#define FT_TIME_MEASURE_QUARTER_MASK (FT_TIME_MEASURE_COMPLETE_MASK/4)

// The following constant is an important limit: the way the algorithm is written,
// we *need* a limit. The theoretical limit is COMPLETE_MASK/2, but it's a little
// dangerous, as if we miss this timer from 1 unit (say 1ms), it won't work
// as expected. So we set this limit to COMPLETE_MASK/3. Why 3? Why not!
// This is a lot safer!
#define FT_MAX_DELAY (FT_TIME_MEASURE_COMPLETE_MASK/3)

#define _FT_SIMPLE_TIMER_ALLOCATION
//#define _FT_SIMPLE_TIMER_WITH_MALLOC : NOT IMPLEMENTED!!!
//#define _FT_TIMER_MALLOC

/////////////////////////////////

#define FT_ONE_MINUTE ((FT_ONE_SECOND)*60)
#define FT_ONE_HOUR   ((FT_ONE_SECOND)*60*60)
#define FT_ONE_DAY    ((FT_ONE_SECOND)*60*60*24)
#define FT_ONE_WEEK   ((FT_ONE_SECOND)*60*60*24*7)

typedef struct FT_timer_t {
    time_measure_t delay;           // Delay between 2 ticks! RUN_FOREVER otherwise
    time_measure_t next_interrupt;  // Next tick time
    int repeat;                     // How many repeats
    struct FT_timer_t* next;        // Next timer in list (chronologically ordered)
    char display;           // 'A' to 'Z' character for display/debug
    void (*do_it)();        // Action to perform at tick
    void* parameter;        // Parameter to do_it: void* --> can be a pointer to any object!
} FT_timer_t;

// Display all timers in a reader friendly way
extern void FT_debug_timers();

// Published interface:
extern void FT_init();
extern char FT_init_string[100];
extern int FT_at_least_one_timer();
extern void FT_check_and_do(); // Check if there is a timer to fire
extern void FT_sleep_and_do(); // Sleep just what you need, the check...
extern void FT_loop();
extern void FT_infinite_loop();
extern FT_timer_t* FT_insert_timer(time_measure_t delay, int repeat, void (*do_something)(), void* do_it_paramter);

// Examples/tools
extern void FT_do_tick(void* param, FT_timer_t* timer);
extern void FT_do_nothing(void* param, FT_timer_t* timer);

// Crazy useful: force timers that may be in synchrony to fire at different times!
extern void FT_desynchronize_timers(time_t* timer[], int nb_timers);

// Crazy useful: desynchronize with a simpler algorithm (result not garanteed but likely!)
extern void FT_randomize_all_timers();
extern void FT_randomize_timer(FT_timer_t* timer);

// Maybe it's better to use these than the direct ones?
extern time_measure_t FT_get_time();
extern void FT_sleep(time_measure_t delay);

// Forcing!!
extern time_measure_t FT_force_get_time(); // The value will be bounded
extern void FT_force_sleep(time_measure_t delay); // *Will* sleep

// Use this one if you really understand its purpose!
extern void FT_force_sleep_and_do(); // Force the duration of the sleep, then check...

////
extern int FT_compare_to(time_measure_t a, time_measure_t b);

#endif

