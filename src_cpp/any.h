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
#include <typeinfo>
#include "smart_ptr.h"

class any
{
    struct holder_base
    {
        virtual void *get() const { return NULL; }
        virtual const std::type_info &get_type_id() const { return typeid(void); }
        virtual ~holder_base() {}
        int dummy;
    };
    template <class type>
    struct holder : holder_base
    {
        smart_ptr<type> storedObject;
        holder(smart_ptr<type> pobject) : storedObject(pobject) {}
        virtual void *get() const { return storedObject.get(); }
        virtual const std::type_info &get_type_id() const { return typeid(type); }
    };
    smart_ptr<holder_base> held;
public:
    any() {}
    template <class type>
    any(smart_ptr<type> objectToStore) : held(new holder<type>(objectToStore))
  {}
    template <class type>
    type *as() const { 
        if (held.get() == NULL)
            return NULL;
        if (typeid(type) == held->get_type_id()) 
            return static_cast<type *>(held->get()); 
        else 
            return NULL;
    }
    void detach_ptr() { held.detach(); }
};


/*
#include <iostream>
#include <vector>

struct A
{
  void say() { std::cout << "I am A" << std::endl; }
  ~A() { std::cout << "Destroyed an A" << std::endl; }
};

struct B
{
  void say() { std::cout << "I am B" << std::endl; }
  ~B() { std::cout << "Destroyed a B" << std::endl; }
};

int main()
{
    std::vector<any> collection(4);
    collection[0] = smart_ptr<A>(new A);
    collection[1] = smart_ptr<B>(new B);
    collection[2] = smart_ptr<A>(new A);
    collection[3] = smart_ptr<A>(new A);
    for (size_t i = 0; i < collection.size(); i++) {
        A *A_ptr = collection[i].as<A>();
        B *B_ptr = collection[i].as<B>();
        if (A_ptr) A_ptr->say();        
        if (B_ptr) B_ptr->say();
    }
    return 0;
}
*/
