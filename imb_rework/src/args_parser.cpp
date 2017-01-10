#include "args_parser.h"
#include "yaml-cpp/yaml.h"

#include <stdexcept>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

using namespace std;

const int args_parser::version = 0;

args_parser::value &args_parser::value::operator=(const args_parser::value &other) {
    assert(other.initialized);
    if (initialized) {
        assert(other.type == type);
    }
    type = other.type;
    switch (type) {
        case STRING: str = other.str; break;
        case INT: i = other.i; break;
        case FLOAT: f = other.f; break;
        case BOOL: b = other.b; break;
        default: assert(NULL == "Impossible case in switch(type)");
    }
    initialized = true;
    return *this;
}
bool args_parser::value::parse(const char *sval, arg_t _type) {
//    if (initialized)
//        return false;
    type = _type;
    int res = 0;
    switch(type) {
        case STRING: str.assign(sval); res = 1; break;
        case INT: res = sscanf(sval, "%d", &i); break;
        case FLOAT: res = sscanf(sval, "%f", &f); break;
        case BOOL: { 
            res = 1;
            string s; s.assign(sval);                                 
            if (s == "on" || s == "yes" || s == "ON" || s == "YES" || 
                s == "true" || s == "TRUE") {
                b = true;
            } else if (s == "off" || s == "no" || s == "OFF" || s == "NO" || 
                s == "false" || s == "FALSE") {
                b = false;
            } else {
                res = 0;
            }
            break;
        }
        default: assert(NULL == "Impossible case in switch(type)");
    }
    bool bres = ((res == 1) ? true : false);
    if (bres) 
        initialized = true;
    return bres;
}
void args_parser::value::sanity_check(arg_t _type) const { 
    assert(type == _type); 
    assert(initialized); 
}
const string args_parser::value::get_type_str(arg_t _type) {
    switch(_type) {
        case STRING: return "STRING"; break;
        case INT: return "INT"; break;
        case FLOAT: return "FLOAT"; break;
        case BOOL: return "BOOL"; break;
        default: assert(NULL == "Impossible case in switch(type)");                    
    }
    return "";
}

YAML::Emitter &operator<< (YAML::Emitter& out, const args_parser::value &v) {
    if (v.is_initialized()) {
        switch(v.type) {
            case args_parser::STRING: out << v.str.c_str(); break;
            case args_parser::INT: out << v.i; break;
            case args_parser::FLOAT: out << v.f; break;
            case args_parser::BOOL: out << v.b; break;
            default: assert(NULL == "Impossible case in switch(type)");
        }
    }
    return out;
}

void operator>> (const YAML::Node& node, args_parser::value &v) {
    switch(v.type) {
        case args_parser::STRING: node >> v.str; break;
        case args_parser::INT: node >> v.i; break;
        case args_parser::FLOAT: node >> v.f; break;
        case args_parser::BOOL: node >> v.b; break;
        default: assert(NULL == "Impossible case in switch(type)");
    }
}

bool args_parser::match(string &arg, string pattern) const {
    if (strncmp(arg.c_str(), option_starter, strlen(option_starter)))
       return false;
    if (strncmp(arg.c_str() + strlen(option_starter), pattern.c_str(), pattern.size()))
       return false;
    return true;
}
bool args_parser::match(string &arg, descr &exp) const {
    return match(arg, exp.str);
}
bool args_parser::get_value(string &arg, descr &exp) {
    size_t offset = 0; 
    if (prev_option_descr == NULL) {
        offset = strlen(option_starter);
        if (option_delimiter == ' ') {
            prev_option_descr = &exp;
            offset += exp.str.size();
            if (*(arg.c_str() + offset) != 0)
                return false;
            return true;
        } else {
            offset += exp.str.size();
            if (*(arg.c_str() + offset) != option_delimiter)
                return false;
            offset += 1;
        }
    }
    prev_option_descr = NULL;
    char *sval = strdup(arg.c_str() + offset);
    if (exp.vec_delimiter == 0) {
        bool res = exp.val[0].parse(sval, exp.type);
        free(sval);
        return res;
    }
    vector<int> positions;
    for (char *s = sval; *s; s++) {
        if (*s == exp.vec_delimiter)
            positions.push_back(s - sval);
    }
    if (positions.size() + 1 < (size_t)exp.vec_min || positions.size() + 1 > (size_t)exp.vec_max) {
        free(sval);
        return false;
    }
    exp.val.resize(max(positions.size() + 1, exp.val.size()));
    size_t i = 0, j = 0;
    bool res = true;
    for (; i < positions.size(); i++) {
        sval[positions[i]] = 0;
        res = res && exp.val[i].parse(sval + j, exp.type);
        j = positions[i] + 1;
    }
    res = res && exp.val[i].parse(sval + j, exp.type);
    free(sval);
    return res;
}
void args_parser::print_err_required_arg(descr &arg) const {
    if (!silent)
        cout << "ERROR: The required argument missing or can't be parsed: " << option_starter << arg.str << endl;
}
void args_parser::print_err_required_extra_arg() const {
    if (!silent)
        cout << "ERROR: The required extra argument missing" << endl;
}
void args_parser::print_err_parse(descr &arg) const {
    if (!silent)
        cout << "ERROR: Parse error on argument: " << option_starter << arg.str << endl;
}
void args_parser::print_err_parse_extra_args() const {
    if (!silent)
        cout << "ERROR: Parse error on an extra argument" << endl;
}
void args_parser::print_err_extra_args() const {
    if (!silent)
        cout << "ERROR: Some extra or unknown arguments or options" << endl;
}
void args_parser::print_help_advice() const {
    cout << "Try \"" <<  basename(argv[0]) << " " << option_starter << "help\" for usage information" << endl;
}

// NOTE: This one is just to loop over expected_args 2-level array in a easier way.
// First call woth FOREACH_FIRST initializes the walk throgh all expected args,
// each next call with FOREACH_NEXT gives a pointer to the next arg from expected_args
// together with the pointer to the group name it belongs
// Two versions are here for ordinary and constant methods, mind the 'const' keyword.
bool args_parser::in_expected_args(enum foreach_t t, const string *&group, descr *&arg) {
    static map<const string, vector<descr> >::iterator it;
    static size_t j = 0;
    if (t == FOREACH_FIRST) {
        it = expected_args.begin();
        j = 0;
        return true;
    }
    if (t == FOREACH_NEXT) {
        while (it != expected_args.end()) {
            vector<descr> &expected_args = it->second;
            if (j >= expected_args.size()) {
               ++it;
               j = 0;
               continue;
            } 
            group = &(it->first);
            arg = &expected_args[j];
            j++;
            return true;
        }
        return false;
    }
    return false;
}

bool args_parser::in_expected_args(enum foreach_t t, const string *&group, const descr *&arg) const {
    static map<const string, vector<descr> >::const_iterator cit;
    static size_t j = 0;
    if (t == FOREACH_FIRST) {
        cit = expected_args.begin();
        j = 0;
        return true;
    }
    if (t == FOREACH_NEXT) {
        while (cit != expected_args.end()) {
            const vector<descr> &expected_args = cit->second;
            if (j >= expected_args.size()) {
               ++cit;
               j = 0;
               continue;
            } 
            group = &(cit->first);
            arg = &expected_args[j];
            j++;
            return true;
        }
        return false;
    }
    return false;
}

void args_parser::print_single_option_usage(const descr &d, size_t header_size, 
        bool is_first, bool no_option_name) const {
    string tab(header_size, ' ');
    const char *open_brace = "[";
    const char *close_brace = "]";
    const char *empty = "";
    const char *op = d.required ? empty : open_brace;
    const char *cl = d.required ? empty : close_brace;
    const string stype = value::get_type_str(d.type);
    const string cap = (d.vec_delimiter == 0 ? 
            d.caption + ":" + stype : d.caption);
    const string cptn = (d.caption.size() == 0 ? stype : cap);
    const string allign = (is_first ? "" : tab);
    if (no_option_name)
        cout << allign << op << cptn << cl << " ";
    else
        cout << allign << op << option_starter << d.str << option_delimiter << cptn << cl << endl;
}

void args_parser::print_help() const {
    cout << "Usage: " << basename(argv[0]) << " ";
    string header;
    header +=  "Usage: ";
    header += basename(argv[0]); 
    header += " ";
    size_t size = min(header.size(), (size_t)16);
   bool is_first = true;
    const string *pgroup;
    const descr *poption;
    in_expected_args(FOREACH_FIRST, pgroup, poption);
    while(in_expected_args(FOREACH_NEXT, pgroup, poption)) {
        if (*pgroup == "EXTRA_ARGS")
            continue;
        print_single_option_usage(*poption, size, is_first);
        is_first = false;
    }
    int num_extra_args = 0, num_required_extra_args = 0;
    const std::vector<descr> &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    for (int j = 0; j < num_extra_args; j++) {
        print_single_option_usage(extra_args[j], size, is_first, true);
    }
    if (num_extra_args)
        cout << endl;
}

template <typename T> T get_val(const args_parser::value &v);
template <> int get_val<int>(const args_parser::value &v) { return v.i; }
template <> float get_val<float>(const args_parser::value &v) { return v.f; }
template <> bool get_val<bool>(const args_parser::value &v) { return v.b; }
template <> string get_val<string>(const args_parser::value &v) { return v.str; }

template <args_parser::arg_t t, typename T>
void vresult_to_vector(const vector<args_parser::value> &in, vector<T> &out) {
    assert(in.size() > 0);
    if (in[0].type != t)
        throw logic_error("args_parse: wrong option type");
    for (size_t i = 0; i < in.size(); i++)
        out.push_back(get_val<T>(in[i]));
}
void args_parser::print() const {
    const string *pgroup;
    const descr *parg;
    in_expected_args(FOREACH_FIRST, pgroup, parg);
    while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
        parg->print();
    }
}

const vector<args_parser::descr> &args_parser::get_extra_args_info(int &num_extra_args, int &num_required_extra_args) const {
    const vector<descr> &extra_args = expected_args.find("EXTRA_ARGS")->second;
    bool required_args_ended = false;
    for (int j = 0; j < extra_args.size(); j++) {
        if (extra_args[j].required) {
            if (required_args_ended)
                throw logic_error("args_parser: all required extra args must precede non-required args");
            num_required_extra_args++;
        } else {
            required_args_ended = true;
        }

    } 
    num_extra_args = extra_args.size();
    return extra_args;
}

vector<args_parser::descr> &args_parser::get_extra_args_info(int &num_extra_args, int &num_required_extra_args) {
    vector<descr> &extra_args = expected_args["EXTRA_ARGS"];
    for (int j = 0; j < extra_args.size(); j++) {
        if (extra_args[j].required)
            num_required_extra_args++;
    } 
    num_extra_args = extra_args.size();
    return extra_args;
}

// NOTE: this is aproppriate action for vector values only. It is reasonable to fill
// the vector first with default values, and only then parse what is given, so we
// have all set of good values in vector even if parsed string sets only a few values
// explicitely
void args_parser::get_default_value(descr &d) {
    descr *old = prev_option_descr;
    prev_option_descr = &d;
    if (d.vec_delimiter != 0) {
        bool res = get_value(d.vec_def, d);
        if (!res)
            throw logic_error("args_parse: wrong default value on vector option");
    }
    prev_option_descr = old;
}

bool args_parser::parse() {
    bool parse_result = true;
    bool help_printed = false;
    unknown_args.resize(0);
    // go through all given args
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        // if there is a pointer to a descriptor which corresponds to previous argv[i]
        if (prev_option_descr) {
            // the option itself was given as a previous argv[i] or was just pre-stored
            // (for special cases)
            // now only parse the option argument
            descr &d = *prev_option_descr;
            if (!d.required) 
                get_default_value(d);
            if (!get_value(arg, d)) {
                print_err_parse(d);
                parse_result = false;
            }
            d.defaulted = false;
            continue;
        }
        // help is hardcoded as and optional 1st arg
        // NOTE: may be change this to be optional?
        if (i == 1 && match(arg, string("help"))) {
            print_help();
            parse_result = false;
            help_printed = true;
        }
        // go throwgh all expected_args[] elements to find the option by pattern
        bool found = false;
        const string *pgroup;
        descr *parg;
        in_expected_args(FOREACH_FIRST, pgroup, parg);
        while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
            if (*pgroup == "EXTRA_ARGS")
                continue;
            if (match(arg, *parg)) {
                if (!parg->required)
                    get_default_value(*parg);
                if (!get_value(arg, *parg)) {
                    print_err_parse(*parg);
                    parse_result = false;
                }
                parg->defaulted = false;
                found = true;
                break;
            }
        }
        // all unmatched args are stored in a separate array to handle them later
        if (!found)
            unknown_args.push_back(arg);
    }

    // the case when cmdline args ended too early
    if (prev_option_descr != NULL) {
        print_err_parse(*prev_option_descr);
        parse_result = false;
    }

    // now parse the expected extra agrs
    int num_extra_args = 0, num_required_extra_args = 0;
    std::vector<descr> &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (unknown_args.size() < (size_t)num_required_extra_args) {
        print_err_required_extra_arg();
        parse_result = false;
    } else {
        int num_processed_extra_args = 0;
        for (int j = 0; j < extra_args.size(); j++) {
            if (j >= unknown_args.size())
               break;
            if (match(unknown_args[j], "")) 
                continue;
            if (!extra_args[j].required)
                get_default_value(extra_args[j]);
            prev_option_descr = &extra_args[j];
            if (!get_value(unknown_args[j], extra_args[j])) {
                print_err_parse_extra_args();
                parse_result = false;
                break;
            }
            extra_args[j].defaulted = false;
            num_processed_extra_args++;
        }
        assert(num_processed_extra_args <= unknown_args.size());
        unknown_args.erase(unknown_args.begin(), unknown_args.begin() + num_processed_extra_args);
    }

    // loop again through all in expected_args[] to find options which were not given in cmdline
    const string *pgroup;
    descr *parg;
    in_expected_args(FOREACH_FIRST, pgroup, parg);
    while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
        if (parg->vec_delimiter != 0 && parg->val.size() == 0 && !parg->required) {
            get_default_value(*parg);
            parg->defaulted = true;
            continue;
        }
        if (!parg->val[0].is_initialized()) {
            if (parg->required) {
                print_err_required_arg(*parg);
                parse_result = false;
            } else {
                // for a not required option proceed with the default value copying
                if (parg->vec_delimiter == 0) {
                    parg->val[0] = parg->def;
                    parg->defaulted = true;
                } else {
                    get_default_value(*parg);
                    parg->defaulted = true;
//                    prev_option_descr = parg;
//                    bool res = get_value(parg->vec_def, *parg);
//                    if (!res) 
//                        throw logic_error("args_parse: wrong default value on vector option");
                }
            }
        }
    }
    // if there are too many unexpected args, raise error
    // NOTE: may be change this to me optional?
    if (parse_result && unknown_args.size()) {
        print_err_extra_args();
        parse_result = false;
    }
    // NOTE: silent flag is a bit outstanding as for now, may be change to a more common flag scheme?
    if (!parse_result && !silent && !help_printed)
        print_help_advice();
    return parse_result;
}
args_parser &args_parser::add_required_option(const char *s, arg_t type) {
    expected_args[current_group].push_back(args_parser::descr(s, type));
    return *this;
}
args_parser &args_parser::add_option_with_defaults(const char *s, arg_t type, value v) {
    expected_args[current_group].push_back(args_parser::descr(s, type, v));
    return *this;
}
args_parser &args_parser::add_required_option_vec(const char *s, arg_t type, char delim, int min, int max) {
    if (max > descr::MAX_VEC_SIZE)
        throw logic_error("args_parser: maximum allowed vector size for vector argument exceeded");
    expected_args[current_group].push_back(args_parser::descr(s, type, delim, min, max));
    return *this;
}
args_parser &args_parser::add_option_with_defaults_vec(const char *s, arg_t type, const string &defaults, char delim, int min, int max) {
    if (max > descr::MAX_VEC_SIZE)
        throw logic_error("args_parser: maximum allowed vector size for vector argument exceeded");
    expected_args[current_group].push_back(args_parser::descr(s, type, delim, min, max, defaults));
    return *this;
}
args_parser &args_parser::set_caption(const char *s, const char *cap) {
    const string *pgroup;
    descr *parg;
    in_expected_args(FOREACH_FIRST, pgroup, parg);
    while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
        if (parg->str == s) {
            parg->caption.assign(cap);
            return *this;
        }
    }
    throw logic_error("args_parser: no such option");
}
args_parser &args_parser::set_caption(int n, const char *cap) {
    int num_extra_args = 0, num_required_extra_args = 0;
    vector<descr> &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (n >= num_extra_args)
        throw logic_error("args_parser: no such extra argument");
    extra_args[n].caption.assign(cap);
    return *this;
}
const vector<args_parser::value> &args_parser::get_result(const string &s) {
    const string *pgroup;
    descr *parg;
    in_expected_args(FOREACH_FIRST, pgroup, parg);
    while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
        if (parg->str == s) {
            return parg->val;
        }
    }
    throw logic_error("args_parser: no such option");
}
int args_parser::get_result_int(const string &s) {
    vector<int> r;
    get_result_vec_int(s, r);
    return r[0];
}
float args_parser::get_result_float(const string &s) {
    vector<float> r;
    get_result_vec_float(s, r);
    return r[0];
}
bool args_parser::get_result_bool(const string &s) {        
    vector<bool> r;
    get_result_vec_bool(s, r);
    return r[0];
}
string args_parser::get_result_string(const string &s) {        
    vector<string> r;
    get_result_vec_string(s, r);
    return r[0];
}
void args_parser::get_result_vec_int(const string &s, vector<int> &r) {
    const vector<value> &v = get_result(s);
    vresult_to_vector<INT, int>(v, r);
}
void args_parser::get_result_vec_float(const string &s, vector<float> &r) {
    const vector<value> &v = get_result(s);
    vresult_to_vector<FLOAT, float>(v, r);
}
void args_parser::get_result_vec_bool(const string &s, vector<bool> &r) {
    const vector<value> &v = get_result(s);
    vresult_to_vector<BOOL, bool>(v, r);
}
void args_parser::get_result_vec_string(const string &s, vector<string> &r) {
    const vector<value> &v = get_result(s);
    vresult_to_vector<STRING, string>(v, r);
}
void args_parser::get_unknown_args(vector<string> &r) {
    for (size_t j = 0; j < unknown_args.size(); j++) {
        r.push_back(unknown_args[j]);
    }
}
bool args_parser::parse_special_int(string &s, int &r) {
    descr d("[FREE ARG]", INT);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res)
        r = get_val<int>(d.val[0]);
    return res;
}
bool args_parser::parse_special_float(string &s, float &r) {
    descr d("[FREE ARG]", FLOAT);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res)
        r = get_val<float>(d.val[0]);
    return res;
}
bool args_parser::parse_special_bool(string &s, bool &r) {
    descr d("[FREE ARG]", BOOL);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res)
        r = get_val<bool>(d.val[0]);
    return res;
}
bool args_parser::parse_special_string(string &s, string &r) {
    descr d("[FREE ARG]", STRING);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res)
        r = get_val<string>(d.val[0]);
    return res;
}
bool args_parser::parse_special_vec_int(string &s, vector<int> &r, char delim, int min, int max) {
    descr d("[FREE ARG]", INT, delim, min, max);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res) 
        vresult_to_vector<INT, int>(d.val, r);
    return res;
}
bool args_parser::parse_special_vec_float(string &s, vector<float> &r, char delim, int min, int max) {
    descr d("[FREE ARG]", FLOAT, delim, min, max);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res) 
        vresult_to_vector<FLOAT, float>(d.val, r);
    return res;
}
bool args_parser::parse_special_vec_bool(string &s, vector<bool> &r, char delim, int min, int max) {
    descr d("[FREE ARG]", BOOL, delim, min, max);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res) 
        vresult_to_vector<BOOL, bool>(d.val, r);
    return res;
}
bool args_parser::parse_special_vec_string(string &s, vector<string> &r, char delim, int min, int max) {
    descr d("[FREE ARG]", STRING, delim, min, max);
    prev_option_descr = &d;
    bool res = get_value(s, d);
    if (res) 
        vresult_to_vector<STRING, string>(d.val, r);
    return res;
}
bool args_parser::load(istream &stream) {
    try {
        YAML::Parser parser(stream);
        YAML::Node node;
        parser.GetNextDocument(node);
        // loop through all in expected_args[] to find each option in file 
        const string *pgroup;
        descr *parg;
        in_expected_args(FOREACH_FIRST, pgroup, parg);
        while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
            if (*pgroup == "SYS" || *pgroup == "EXTRA_ARGS")
                continue;
            if(const YAML::Node *pName = node.FindValue(parg->str.c_str())) {
                if (!parg->vec_delimiter) {
                    *pName >> parg->val[0];
                    parg->val[0].initialized = true;
                    parg->defaulted = false;
                } else {
                    *pName >> parg->val;
                    if (parg->val.size() == 0) {
                        cout << "ERROR: input YAML file parsing error: " << parg->str << ": vector must have at least one value" << endl;
                        return false;
                    }
                    parg->val[0].initialized = true;
                    parg->defaulted = false;
                }
            }
        }
        int num_extra_args = 0, num_required_extra_args = 0;
        std::vector<descr> &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
        if(const YAML::Node *pName = node.FindValue("extra_args")) {
            int j = 0;
            for(YAML::Iterator it = pName->begin(); it != pName->end(); ++it) {
                if (j == num_extra_args) 
                    break;
                parg = &extra_args[j];
                if (!parg->vec_delimiter) {
                    *it >> parg->val[0];
                    parg->val[0].initialized = true;
                    parg->defaulted = false;
                } else {
                    *it >> parg->val;
                    if (parg->val.size() == 0) {
                        cout << "ERROR: input YAML file parsing error: vector extra arg must have at least one value" << endl;
                        return false;
                   }

                    parg->val[0].initialized = true;
                    parg->defaulted = false;
                }
            }
        }
    }
    catch (const YAML::Exception& e) {
        cout << "ERROR: input YAML file parsing error: " << e.what() << endl;
        return false;
    }
    // now do regular parse procedure to do: 1) cmdline options ovelapping: what is given 
    // in cmdline has a priority; 2) filling in non-required options with defaults.
    // NOTE: if cmdline parsing is unwanted, you can defeat it with a previous
    // call to clean_args()
    return parse();
}
bool args_parser::load(const string &input) {
    stringstream stream(input.c_str());
    return load(stream);
}
string args_parser::dump() const {
    YAML::Emitter out;
    out << YAML::BeginDoc << YAML::Comment("IMB config file");
    out << YAML::BeginMap;
    out << YAML::Flow;
    out << YAML::Key << "version";
    out << YAML::Value << version;
    const string *pgroup;
    const descr *parg;
    in_expected_args(FOREACH_FIRST, pgroup, parg);
    while(in_expected_args(FOREACH_NEXT, pgroup, parg)) {
        if (*pgroup == "SYS" || *pgroup == "EXTRA_ARGS")
            continue;
        if (!parg->vec_delimiter) {
            if (parg->defaulted) {
                YAML::Emitter comment;
                comment << YAML::BeginMap;
                comment << YAML::Flow << YAML::Key << parg->str.c_str();
                comment << YAML::Flow << YAML::Value << parg->val[0];
                comment << YAML::EndMap;
                out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
            } else {
                out << YAML::Key << parg->str.c_str();
                out << YAML::Value << parg->val[0];
            }
        } else {
            if (parg->defaulted) {
                YAML::Emitter comment;
                comment << YAML::BeginMap;
                comment << YAML::Flow << YAML::Key << parg->str.c_str();
                comment << YAML::Flow << YAML::Value << parg->val;
                comment << YAML::EndMap;
                out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
            } else {
                out << YAML::Key << parg->str.c_str();
                out << YAML::Flow << YAML::Value << parg->val;
            }
        }
    }
    int num_extra_args = 0, num_required_extra_args = 0;
    const std::vector<descr> &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (num_extra_args > 0) {
        out << YAML::Key << "extra_args";
        out << YAML::Value << YAML::BeginSeq << YAML::Newline;
        for (int i = 0; i < num_extra_args; i++) {
            parg = &extra_args[i];
            if (!parg->vec_delimiter) {
                if (parg->defaulted) {
                    YAML::Emitter comment;
                    comment << YAML::Flow << parg->val[0];
                    out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
                } else {
                    out << parg->val[0];
                }
            } else {
                if (parg->defaulted) {
                    YAML::Emitter comment;
                    comment << YAML::Flow << parg->val;
                    out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
                } else {
                    out << YAML::Flow << parg->val;
                }
            }
        }
        out << YAML::Newline << YAML::EndSeq;
    }
    out << YAML::EndMap;
    out << YAML::Newline;
    return string(out.c_str());
}


ostream &operator<<(ostream &s, const args_parser::value &val) {
    switch(val.type) {
        case args_parser::STRING: s << val.str; break;
        case args_parser::INT: s << val.i; break;
        case args_parser::FLOAT: s << val.f; break;
        case args_parser::BOOL: s << val.b; break;
        default: assert(NULL == "Impossible case in switch(type)");
    }
    return s;
}
#if 0
int main(int argc, char **argv)
{
    try {
        //args_parser parser(argc, argv, "/", ':');
        //args_parser parser(argc, argv, "--", '=');
        args_parser parser(argc, argv, "-", ' ');
        parser.set_extra_args_number(2).set_caption(0, "name[,name]").set_caption(1, "[second_arg]");
        parser.add_required_option("name", args_parser::STRING).set_caption("name", "FullName");
        parser.add_option_with_defaults("bbb", args_parser::INT, args_parser::value(-10));
        parser.add_option_with_defaults("ccc", args_parser::FLOAT, args_parser::value(123.5f));
        parser.add_option_with_defaults("xxx", args_parser::BOOL, args_parser::value(false));
        parser.add_option_with_defaults("ddd", args_parser::STRING, args_parser::value("some_default_str"));
        parser.add_option_vector("yyy", args_parser::INT, ',', 0, 4).set_caption("yyy", "int,int,...");
        if (parser.parse()) {
            //parser.print();
            vector<int> r; 
            parser.get_result_vec_int("yyy", r);
            for (size_t i = 0; i < r.size(); i++)
                cout << r[i] << endl;
    
            vector<string> extra_args; 
            parser.get_unknown_args(extra_args);
            vector<string> r2;
            if ((extra_args.size() == 2 || extra_args.size() == 1) && 
                parser.parse_special_vec_string(extra_args[0], r2, ',', 1, 2)) {
                for (size_t i = 0; i < r2.size(); i++)
                    cout << "r2:" << r2[i] << endl;
            } else {
                parser.print_help_advice();
            }
        } 
    } 
    catch(exception &ex) {
        cout << "EXCEPTION: " << ex.what() << endl;
    }
}
#endif

// Makefile:
// ---------
// #CFLAGS=-g -O0 -Wall -Wextra -pedantic
// CFLAGS=-O3 -Wall -Wextra -pedantic
// 
// args_parser: args_parser.o
//     c++ $(CFLAGS) -o $@ $^
//
// %.o: %.cpp
//     c++ $(CFLAGS) -c -o $@ $^
//
// clean:
//     rm -f args_parser args_parser.o

