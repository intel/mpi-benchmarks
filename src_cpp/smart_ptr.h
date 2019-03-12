/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

#pragma once

#include <stdexcept>

template <typename T>
class smart_ptr {
private:
    typedef int counter_t;
public:
    smart_ptr() : pointer(NULL), rc(NULL), detached(false) { }
    smart_ptr(T * p) : pointer(p), rc(p ? new counter_t(0) : NULL), detached(false) { increment(); } 
    smart_ptr(const smart_ptr& rhs) : pointer(rhs.pointer), rc(rhs.rc), detached(rhs.detached) { increment(); }
    ~smart_ptr() {
        if(rc && decrement() == 0) { if (!detached) delete pointer; delete rc; }
    }
    void assign(T *p) {
        if(rc == NULL && pointer == NULL) { 
            pointer = p;
            rc = (p ? new counter_t(0) : NULL);
            increment();
        } else {
            throw std::logic_error("smart_ptr: assign: bad usage");
        }
    }
    void swap(smart_ptr& rhs) {
        std::swap(pointer, rhs.pointer);
        std::swap(rc, rhs.rc);
    }
    void detach() { detached = true; }
    T *get() const { return pointer; }
    smart_ptr& operator=(const smart_ptr& rhs) {
        smart_ptr tmp(rhs);
        this->swap(tmp);
        return *this;
    }
    T& operator*() { return *pointer; }
    const T& operator*() const { return *pointer; }
    T* operator->() { return pointer; }
    const T* operator->() const { return pointer; }
private:
    void increment() { if(rc) ++(*rc); }
    counter_t decrement() { return --(*rc); }

    T *pointer;
    counter_t *rc;
    bool detached;
};
