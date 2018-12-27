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

#include <iterator>
#include <algorithm>
#include <string.h>

namespace set_operations {
    // to = to \/ from      (in place set union)
    template <typename T1, typename T2>
    void combine(T1 &to, T2 &from) {
        copy(from.begin(), from.end(), inserter(to, to.end()));
    }
    // from = from \ what   (in place set difference)
    template <typename T1, typename T2>
    void exclude(T1 &from, T2 &what) {
        for (typename T2::iterator what_it = what.begin();
             what_it != what.end(); ++what_it) {
            do {
                typename T1::iterator it = find(from.begin(), from.end(), *what_it);
                if (it != from.end())
                    from.erase(it);
                else 
                    break;
            } while (true);
        }
    }
    // result = one \ two    (set difference)
    template <typename T1, typename T2, typename T3>
    void diff(T1 &one, T2 &two, T3 &result) {
        set_difference(one.begin(), one.end(), two.begin(), two.end(), inserter(result, result.end()));
    }
    static const std::string to_lower(const std::string &in) {
        std::string out = in;
        transform(in.begin(), in.end(), out.begin(), ::tolower);
        return out;
    }
    template <typename T>
    void preprocess_list(T &list) {
        T tmp;
        transform(list.begin(), list.end(), inserter(tmp, tmp.end()), to_lower);
        list = tmp;
    }
    struct case_insens_cmp : public std::binary_function<std::string, std::string, bool> {
        bool operator()(const std::string &lhs, const std::string &rhs) const {
#ifdef WIN_IMB
            return stricmp(lhs.c_str(), rhs.c_str()) < 0;
#else
            return ::strcasecmp(lhs.c_str(), rhs.c_str()) < 0 ;
#endif
        }
    };
}

