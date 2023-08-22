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

#include "args_parser.h"
#ifdef WITH_YAML_CPP
#include "yaml-cpp/yaml.h"
#endif

#include <stdexcept>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <algorithm>

using namespace std;

//-- UNIT TESTS ----------------------------------------------------------------------------------
#if 1

void print_args(int nargs, const char * const *argv) {
    cout << ":: ";
    for (int i = 0; i < nargs; i++) {
        cout << argv[i];
        if (i != nargs - 1) cout << " ";
    }
    cout << endl;
}

struct CheckParser {
    bool result;
    bool except;
    smart_ptr<args_parser> pparser;
    ostringstream output;
    CheckParser() : result(false), except(false) {}
    args_parser &init(int argc, char ** argv, int mode = 1) {
        print_args(argc, argv);
        switch (mode) {
            case 1: pparser = new args_parser(argc, argv, "-", ' ', output); break;
            case 2: pparser = new args_parser(argc, argv, "--", '=', output); break;
            case 3: pparser = new args_parser(argc, argv, "/", ':', output); break;
            default: assert(false);
        }
        return *pparser;
    }
    args_parser &run() {
        try {
            result = pparser->parse();
        }
        catch(exception &ex) {
            cout << "EXCEPTION: " << ex.what() << endl;
            except = true;
        }
        return *pparser;
    }
    size_t lines_in_output() {
        string s = output.str();
        return count(s.begin(), s.end(), '\n');
    }
    bool output_contains(string what) {
        return output.str().find(what) != string::npos;
    }
};

template <typename T> 
void val2str(T val, string &str)
{
    ostringstream os;
    os << val;
    str = os.str();
}
    
template <> 
void val2str<bool>(bool val, string &str)
{
    str = (val ? "true" : "false");
}

template <typename T> 
void vals2str(vector<T> vals, string delim, string &str)
{
    for (size_t i = 0; i < vals.size(); i++) {
        string tmp;
        val2str<T>(vals[i], tmp);
        str += tmp;
        if (i != vals.size() - 1) {
            str += delim;
        }
    }
}

template <typename T>
int make_args(int start, const char *(&argv)[1024], string opt, const string &sval, int mode)
{
    switch (mode) {
        case 1: { 
                    string a1 = "-" + opt; 
                    argv[start] = strdup(a1.c_str()); 
                    argv[start+1] = strdup(sval.c_str()); 
                    return 2; 
                    break; 
                }
        case 2: { string a1 = "--" + opt + "=" + sval; argv[start] = strdup(a1.c_str()); return 1; break; }
        case 3: { string a1 = "/" + opt + ":" + sval; argv[start] = strdup(a1.c_str()); return 1; break; }
        default: assert(false);
    }
    return 0;
}

template <typename T>
int make_args_scalar(const char *(&argv)[1024], string opt, int mode, T val)
{
    string sval;
    val2str<T>(val, sval);
    argv[0] = strdup("check");
    return make_args<T>(1, argv, opt, sval, mode) + 1;
}
 
template <typename T>
int make_args_vector(const char *(&argv)[1024], string opt, int mode, T val1, T val2)
{
    string sval;
    vector<T> vals;
    vals.push_back(val1);
    vals.push_back(val2);
    vals2str<T>(vals, ",", sval);
    argv[0] = strdup("check");
    return make_args<T>(1, argv, opt, sval, mode) + 1;
}

template <typename T>
void basic_scalar_check(T val) {
    const char * argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        int nargs = make_args_scalar<T>(argv, "aaa", mode, val);
        CheckParser p;
        p.init(nargs, (char * *)argv, mode).add<T>("aaa").set_caption("bbb");
        T result = p.run().get<T>("aaa");
        assert(result == val && p.result && !p.except);
    }
}
 

template <typename T>
void err_scalar_check(T val) {
    const char * argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        string opt, ext;
        int nargs = make_args_scalar<T>(argv, "aaaa", mode, val);
        CheckParser p;
        p.init(nargs, (char * *)argv, mode).add<T>("aaa").set_caption("bbb");
        args_parser::error_t err = p.run().get_last_error(opt, ext);
        assert(!p.result && !p.except && err == args_parser::NO_REQUIRED_OPTION &&
                opt == "aaa"); // && p.lines_in_output() == 2 && p.output_contains("ERROR"));
//        printf(">> %s, %d, %s\n", p.output.str().c_str(), p.lines_in_output(), p.output_contains("ERROR")?"true":"false");
    }
}

template <typename T>
void basic_vector_check(T val1, T val2) {
    const char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        int nargs = make_args_vector<T>(argv, "aaa", mode, val1, val2);
        CheckParser p;
        p.init(nargs,  (char * *)argv, mode).add_vector<T>("aaa", ',').set_caption("bbb");
        vector<T> result;
        p.run().get<T>("aaa", result);
        assert(result.size() == 2 && result[0] == val1 && result[1] == val2 && p.result && !p.except);
    }
}

template <typename T>
void default_scalar_check(T def) {
    const char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        CheckParser p;
        p.init(1,  (char * *)argv, mode).add<T>("aaa", def).set_caption("bbb");
        T res = p.run().get<T>("aaa");
        assert(res == def && p.result && !p.except);
    }
}

template <typename T>
void default_vector_check(const char *def, size_t n) {
    const char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        CheckParser p;
        p.init(1,  (char * *)argv, mode).add_vector<T>("aaa", def).set_caption("bbb");
        vector<T> result;
        p.run().get<T>("aaa", result);
        assert(result.size() == n && p.result && !p.except);
    }
}

template <typename T>
void default_vector_check_ext(const char *def, const char *sval, size_t n, T val1, T val2) {
    const char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        int nargs = make_args<string>(1, argv, "aaa", sval, mode) + 1;
        CheckParser p;
        p.init(nargs,  (char * *)argv, mode).add_vector<T>("aaa", def).set_caption("bbb");
        vector<T> result;
        p.run().get<T>("aaa", result);
        assert(result.size() == n && p.result && !p.except);
        assert(result[0] == val1 && result[1] == val2);
    }
}

void check_parser()
{
    basic_scalar_check<int>(5);
    basic_scalar_check<int>(-5);
    basic_scalar_check<float>(5.5);
    basic_scalar_check<float>(-5.5);
    basic_scalar_check<bool>(true);
    basic_scalar_check<bool>(true);
    basic_scalar_check<string>("ccc");
    
    basic_vector_check<int>(5, 5);
    basic_vector_check<int>(5, -5);
    basic_vector_check<int>(-5, 5);
    basic_vector_check<float>(5.5, 5.5);
    basic_vector_check<float>(-5.5, 5.5);
    basic_vector_check<bool>(true, false);
    basic_vector_check<bool>(true, false);
    basic_vector_check<string>("ccc", "ddd");

    default_scalar_check<int>(5);
    default_scalar_check<float>(5.5);
    default_scalar_check<bool>(true);
    default_scalar_check<string>("ccc");

    default_vector_check<int>("", 0);
    default_vector_check<int>("5", 1);
    default_vector_check<int>("5,-5", 2);
    default_vector_check<float>("", 0);
    default_vector_check<float>("5.5", 1);
    default_vector_check<float>("5.5,.3", 2);
    default_vector_check<bool>("", 0);
    default_vector_check<bool>("true", 1);
    default_vector_check<bool>("true,false", 2);
    default_vector_check<string>("", 0);
    default_vector_check<string>("ccc", 1);
    default_vector_check<string>("ccc,ddd", 2);

    default_vector_check_ext<int>("5,-5", "1", 2, 1, -5);
    default_vector_check_ext<int>("5,-5", "1,1", 2, 1, 1);
    default_vector_check_ext<float>("5.,-5.0e0", ".1", 2, 0.1, -5.0);
    default_vector_check_ext<float>("5.", ".1,1e-6", 2, 0.1, 1e-6);
    default_vector_check_ext<bool>("true,false", "false", 2, false, false);
    default_vector_check_ext<bool>("true", "false,true", 2, false, true);
    default_vector_check_ext<string>("aaa,bbbb", "ccc", 2, "ccc", "bbbb");
    default_vector_check_ext<string>("aaa", "ccc,ddd", 2, "ccc", "ddd");

    err_scalar_check<string>("ccc");

}
#endif

