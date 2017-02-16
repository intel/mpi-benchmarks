#include "args_parser.h"
#include "yaml-cpp/yaml.h"

#include <stdexcept>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

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
    CheckParser() : result(false), except(false) {}
    args_parser &init(int argc, char ** argv, int mode = 1) {
        print_args(argc, argv);
        switch (mode) {
            case 1: pparser = new args_parser(argc, argv, "-", ' '); break;
            case 2: pparser = new args_parser(argc, argv, "--", '='); break;
            case 3: pparser = new args_parser(argc, argv, "/", ':'); break;
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
int make_args(int start, char ** const &argv, string opt, const string &sval, int mode)
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
int make_args_scalar(char ** const &argv, string opt, int mode, T val)
{
    string sval;
    val2str<T>(val, sval);
    argv[0] = strdup("check");
    return make_args<T>(1, argv, opt, sval, mode) + 1;
}
 
template <typename T>
int make_args_vector(char ** const &argv, string opt, int mode, T val1, T val2)
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
void basic_scalar_check(const char *sval, T val) {
    char * argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        int nargs = make_args_scalar<T>(argv, "aaa", mode, val);
        CheckParser p;
        p.init(nargs, argv, mode).add_required_option<T>("aaa").set_caption("bbb");
        T result = p.run().get_result<T>("aaa");
        assert(result == val && p.result && !p.except);
    }
}
 
template <typename T>
void basic_vector_check(const char *sval, T val1, T val2) {
    char * argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        int nargs = make_args_vector<T>(argv, "aaa", mode, val1, val2);
        CheckParser p;
        p.init(nargs, argv, mode).add_required_option_vec<T>("aaa", ',').set_caption("bbb");
        vector<T> result;
        p.run().get_result_vec<T>("aaa", result);
        assert(result.size() == 2 && result[0] == val1 && result[1] == val2 && p.result && !p.except);
    }
}

template <typename T>
void default_scalar_check(T def) {
    char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        CheckParser p;
        p.init(1, argv, mode).add_option_with_defaults<T>("aaa", def).set_caption("bbb");
        T res = p.run().get_result<T>("aaa");
        assert(res == def && p.result && !p.except);
    }
}

template <typename T>
void default_vector_check(const char *def, size_t n) {
    char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        CheckParser p;
        p.init(1, argv, mode).add_option_with_defaults_vec<T>("aaa", def).set_caption("bbb");
        vector<T> result;
        p.run().get_result_vec<T>("aaa", result);
        assert(result.size() == n && p.result && !p.except);
    }
}

template <typename T>
void default_vector_check_ext(const char *def, const char *sval, size_t n, T val1, T val2) {
    char *argv[1024]; 
    for (int mode = 1; mode <= 3; mode++) {
        argv[0] = "check";
        int nargs = make_args<string>(1, argv, "aaa", sval, mode) + 1;
        CheckParser p;
        p.init(nargs, argv, mode).add_option_with_defaults_vec<T>("aaa", def).set_caption("bbb");
        vector<T> result;
        p.run().get_result_vec<T>("aaa", result);
        assert(result.size() == n && p.result && !p.except);
        assert(result[0] == val1 && result[1] == val2);
    }
}

void check_parser()
{
    basic_scalar_check<int>("5", 5);
    basic_scalar_check<int>("-5", -5);
    basic_scalar_check<float>("5.5", 5.5);
    basic_scalar_check<float>("-5.5", -5.5);
    basic_scalar_check<bool>("true", true);
    basic_scalar_check<bool>("on", true);
    basic_scalar_check<string>("ccc", "ccc");
    
    basic_vector_check<int>("5,5", 5, 5);
    basic_vector_check<int>("5,-5", 5, -5);
    basic_vector_check<int>("-5,-5", -5, 5);
    basic_vector_check<float>("5.5,5.5", 5.5, 5.5);
    basic_vector_check<float>("-5.5,5.5", -5.5, 5.5);
    basic_vector_check<bool>("true,false", true, false);
    basic_vector_check<bool>("on,off", true, false);
    basic_vector_check<string>("ccc,ddd", "ccc", "ddd");

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
}
#endif

