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
