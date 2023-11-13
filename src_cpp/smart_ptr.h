/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/

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
