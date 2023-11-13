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

