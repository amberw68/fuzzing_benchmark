/*
 * Copyright (C) 2022-2022 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "pin.H"
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::ofstream;
using std::string;

// This tool fills thread stack in order to activate most memory pages
// using a recursion

// counter for each thread to run the recursion every 1000 instructions
// instead of every instruction to reduce runtime
UINT32 tCount[PIN_MAX_THREADS];

// The pin_stack_fill test failed at some of the Linux server due to lack of resourced (only 2GB swap memory)
// so we increase the spare space (originally exists to avoid stack-overflow) to reduce memory usage
#if (defined(TARGET_LINUX))
#define SPARE_SPACE 25
#else
#define SPARE_SPACE 10
#endif

// This knob defines Pin stack size in the same units as -thread_stack_size Pin knob.
// It is assumed that values of these knob match for the test to work properly.
// Test launcher should ensure this.
KNOB< UINT32 > KnobStackSize(KNOB_MODE_WRITEONCE, "pintool", "stack_size", "256",
                             "Size of the thread stack in K ADDRINT units when in PIN state");

// Runs a recursion with the depth of almost the value of the KnobStackSize knob value to
// avoid stack overflow. Every recursion frame allocates an 8KB array (for 64bit system)
// array on the stack
ADDRINT stackAlloc(UINT32 depth)
{
    ADDRINT arr[1024];
    // It is necessary to fill the array using external non-inlineable function invocation
    // in order to avoid compiler optimizations that may eliminate array allocation in function stack frame.
    memset(arr, (int)depth, sizeof(arr));
    if (depth > 0)
    {
        // Use access to array to further suppress compiler optimizations.
        return stackAlloc(depth - 1) + arr[depth];
    }
    else
    {
        return arr[0];
    }
}

VOID fillThreadStack(THREADID threadid)
{
    // Every thread runs the stack fill once every 1000 instructions to reduce run time
    if (!(tCount[threadid] % 1000))
    {
        stackAlloc(KnobStackSize.Value() - SPARE_SPACE);
    }
    tCount[threadid]++;
}

VOID insCallback(INS ins, void* v) { INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(fillThreadStack), IARG_THREAD_ID, IARG_END); }

VOID ThreadStart(THREADID threadid, CONTEXT* ctxt, INT32 flags, VOID* v) { tCount[threadid] = 0; }

int main(INT32 argc, CHAR** argv)
{
    PIN_Init(argc, argv);
    cerr << std::hex;
    cerr.setf(std::ios::showbase);

    PIN_AddThreadStartFunction(ThreadStart, 0);
    INS_AddInstrumentFunction(insCallback, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
