/*
* Copyright 2015 Valentin Milea
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#if defined(_MSC_VER) && _MSC_VER >= 1900

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code
#pragma warning(disable : 4701) // potentially uninitialized local

#include "Common.h"
#include "util/IO.h"
#include <CppAsync/experimental/CoroutineTraits.h>
#include <climits>

namespace {

static ut::Coroutine genFibo(int n) {
    int a = 0;
    int b = 1;

    for (int i = 0; i < n; i++) {
        // Suspend coroutine.
        yield &b;

        // Coroutine has been resumed, generate next value.
        int t = a;
        a = b;
        b += t;

        if (b < a) {
            // WARNING: The implementation of resumable functions in MSVC 14.0 is incomplete and
            //          doesn't deal with exceptions well. This example may fail miserably from
            //          improper stack unwinding.
            throw std::runtime_error("overflow");
        }
    }
}

}

void ex_fibo_n4402()
{
    // Generate an "infinite" Fibonacci sequence.
    const int n = INT_MAX;

    // Create a coroutine on top of stackless 'resumable functions', which are a proposed addition
    // to C++17. See: https://isocpp.org/files/papers/N4402.pdf.
    ut::Coroutine fibo = genFibo(n);

    try {
        // Resume coroutine. Possible outcomes:
        // a) fibo() returns true. Coroutine has yielded some value and suspended itself.
        // b) fibo() returns false. Coroutine has finished.
        // c) fibo() propagates an exception. Coroutine has ended in error.
        //
        while (fibo()) {
            // Coroutine has yielded, print value.
            printf("%d\n", fibo.valueAs<int>());
        }
    } catch (const std::exception& e) {
        printf("exception: %s\n", e.what());
    }
}

#pragma warning(pop)

#endif
