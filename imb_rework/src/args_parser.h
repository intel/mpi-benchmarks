#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <string>
#include <map>
#include <stdexcept>
#include "smart_ptr.h"
#include "yaml-cpp/yaml.h"

// TODO:
// proposed moding to add:
// SILENT -- no output
// NOHELP -- don't detect help option
// ALLOW_EXTRA -- don't regards extra args as an error
// NODUPLICATE -- don't allow duplicate options
// NODEFAULTSDUMP -- don't put defaults into dump


class args_parser {
    protected:
    int argc;
    const char * const *argv;
    const char *option_starter;
    const char option_delimiter;
    const static int version;

    public:
    args_parser(int &_argc, char **&_argv, const char *opt_st = "--", char opt_delim = '=', bool silnt = false) : argc(_argc), argv(_argv), option_starter(opt_st), option_delimiter(opt_delim),
                                             prev_option(NULL), silent(silnt) {}
    typedef enum { STRING, INT, FLOAT, BOOL } arg_t;

    class value {
        public:
            value() : initialized(false) {}
            value(float v) : initialized(true), i(0), str("(none)"), b(false) { type = FLOAT; f = v; }
            value(int v) : initialized(true), f(0), str("(none)"), b(false) { type = INT; i = v; }
            value(bool v) : initialized(true), i(0), f(0), str("(none)") { type = BOOL; b = v; }
            value(std::string v) : initialized(true), i(0), f(0), b(false) { type = STRING; str = v; }
            value(const char *v) : initialized(true), i(0), f(0), b(false) { type = STRING; str.assign(v); }
        public:
            bool initialized;
            int i;
            float f;
            std::string str;
            bool b;
            arg_t type;
        public:
            bool is_initialized() const { return initialized; };
            value &operator=(const value &other);
            bool parse(const char *sval, arg_t _type);
            friend std::ostream &operator<<(std::ostream &s, const args_parser::value &val);
            void sanity_check(arg_t _type) const;
            static const std::string get_type_str(arg_t _type); 
    };
    struct option {
        enum mode { APPLY_DEFAULTS_ONLY_WHEN_MISSING };
        std::string str;
        arg_t type;
        bool required;
        bool defaultize_before_parsing;
        bool defaulted;
        std::string caption;
        std::string description;
        option() {};
        option(std::string _str, arg_t _type, bool _required) : str(_str), type(_type), 
                                                               required(_required), defaultize_before_parsing(true), 
                                                               defaulted(false) {};
        virtual void print() const = 0;
        virtual bool do_parse(const char *sval) = 0;
        virtual bool is_scalar() const = 0;
        virtual void set_default_value() = 0;
        virtual option &set_caption(const char *cap) { caption.assign(cap); return *this; }
        virtual option &set_description(const char *descr) { description.assign(descr); return *this; }
        virtual option &set_mode(mode m) { 
            if (m == APPLY_DEFAULTS_ONLY_WHEN_MISSING) 
                defaultize_before_parsing = false; 
            return *this; 
        }
        virtual bool is_default_setting_required() = 0;
        virtual bool is_required_but_not_set() = 0;
        virtual std::vector<args_parser::value> get_value_as_vector() const = 0; 
        virtual void to_ostream(std::ostream &s) const = 0;
        friend std::ostream &operator<<(std::ostream &s, const args_parser::option &d);
        virtual void to_yaml(YAML::Emitter& out) const = 0;
        virtual void from_yaml(const YAML::Node& node) = 0;
        virtual ~option() {}
        private:
        option(const option &) {}
        option &operator=(const option &) { return *this; }
    };
    struct option_scalar : public option {
        args_parser::value def;
        args_parser::value val;
        option_scalar(std::string _str, arg_t _type) : option(_str, _type, true) { }
        option_scalar(std::string _str, arg_t _type, value _def) : option(_str, _type, false), def(_def)
        { def.sanity_check(type); }
        virtual ~option_scalar() {}
        virtual void print() const { std::cout << str << ": " << val << std::endl; }
        virtual bool do_parse(const char *sval) { return val.parse(sval, type); }
        virtual bool is_scalar() const { return true; }
        virtual void to_ostream(std::ostream &s) const { s << val; }
        virtual void to_yaml(YAML::Emitter& out) const; 
        virtual void from_yaml(const YAML::Node& node); 
        virtual void set_default_value() { val = def; defaulted = true; }
        virtual bool is_default_setting_required() { return !val.is_initialized() && !required; }
        virtual bool is_required_but_not_set() { return required && !val.is_initialized(); }
        virtual std::vector<args_parser::value> get_value_as_vector() const { std::vector<args_parser::value> r; r.push_back(val); return r; }
    };
    struct option_vector : public option {
        enum { MAX_VEC_SIZE = 1024 };
        char vec_delimiter;
        int vec_min;
        int vec_max;
        args_parser::value def;
        std::vector<args_parser::value> val;
        std::string vec_def;
        option_vector() {};
        option_vector(std::string _str, arg_t _type, 
                     char _vec_delimiter, int _vec_min, int _vec_max)  :
            option(_str, _type, true), vec_delimiter(_vec_delimiter), vec_min(_vec_min), vec_max(_vec_max)
        { }
        option_vector(std::string _str, arg_t _type, 
                     char _vec_delimiter, int _vec_min, int _vec_max, 
                     const std::string &_vec_def)  :
            option(_str, _type, false), vec_delimiter(_vec_delimiter), vec_min(_vec_min), vec_max(_vec_max), 
            vec_def(_vec_def)
        { }
        virtual ~option_vector() {}
        virtual void print() const { 
            std::cout << str << ": ";
            to_ostream(std::cout);
            std::cout << std::endl; 
        }
        virtual bool do_parse(const char *sval);
        virtual bool is_scalar() const { return false; }
        virtual void to_ostream(std::ostream &s) const { for (size_t i = 0; i < val.size(); i++) { s << val[i]; if (i != val.size()) s << ", "; } }
        virtual void to_yaml(YAML::Emitter& out) const; 
        virtual void from_yaml(const YAML::Node& node); 
        virtual void set_default_value();
        virtual bool is_default_setting_required() { return val.size() == 0 && !required; }
        virtual bool is_required_but_not_set() { return required && vec_min != 0 && !val.size() ==0; }
        virtual std::vector<args_parser::value> get_value_as_vector() const { return val; }
    };

    protected:
    std::string current_group;
    std::map<std::string, std::vector<smart_ptr<option> > > expected_args;
    std::vector<std::string> unknown_args;
    option *prev_option;
    bool silent;
   
    bool match(std::string &arg, std::string pattern) const;
    bool match(std::string &arg, option &exp) const;
    bool get_value(std::string &arg, option &exp);
    void get_default_value(option &d);
    
    const std::vector<smart_ptr<args_parser::option> > &get_extra_args_info(int &num_extra_args, int &num_required_extra_args) const;
    std::vector<smart_ptr<args_parser::option> > &get_extra_args_info(int &num_extra_args, int &num_required_extra_args);

    void print_err_required_arg(const option &arg) const;
    void print_err_required_extra_arg() const;
    void print_err_parse(const option &arg) const; 
    void print_err_parse_extra_args() const;
    void print_err_extra_args() const;
    void print_single_option_usage(const smart_ptr<option> &d, size_t header_size, bool is_first, bool no_option_name = false) const;

    std::vector<value> get_result_value(const std::string &s) const;

    public:
    void print_help_advice() const;
    void print_help() const;
    void print() const;
    bool parse();
    template <typename T>
    option &add_required_option(const char *s);
    template <typename T>
    option &add_option_with_defaults(const char *s, T v);
    template <typename T>
    option &add_required_option_vec(const char *s, char delim = ',', int min = 0, int max = option_vector::MAX_VEC_SIZE);
    template <typename T>
    option &add_option_with_defaults_vec(const char *s, const char *defaults = "", char delim = ',', int min = 0, int max = option_vector::MAX_VEC_SIZE);

    args_parser &set_current_group(const std::string &g) { current_group = g; return *this; }
    args_parser &set_default_current_group() { current_group = ""; return *this; }

//    option &set_caption(const char *cap);
    option &set_caption(int n, const char *cap);

    template <typename T>
    T get_result(const std::string &s) const;
    template <typename T>
    void get_result_vec(const std::string &s, std::vector<T> &r) const;
    void get_unknown_args(std::vector<std::string> &r) const;

    template <typename T>
    bool parse_special(const std::string &s, T &r);
    template <typename T>
    bool parse_special_vec(const std::string &s, std::vector<T> &r, char delim = ',', int min = 0, int max = option_vector::MAX_VEC_SIZE);

    void clean_args() { argc = 0; }
    std::string dump() const;
    bool load(const std::string &input);
    bool load(std::istream &in);

    protected:
    // NOTE: see source for usage comments
    enum foreach_t { FOREACH_FIRST, FOREACH_NEXT };
    bool in_expected_args(enum foreach_t t, const std::string *&group, smart_ptr<option> *&arg);    
    bool in_expected_args(enum foreach_t t, const std::string *&group, const smart_ptr<option> *&arg) const;    
};

template <typename T> args_parser::arg_t get_arg_t();
template <> args_parser::arg_t get_arg_t<int>() { return args_parser::INT; }
template <> args_parser::arg_t get_arg_t<float>() { return args_parser::FLOAT; }
template <> args_parser::arg_t get_arg_t<std::string>() { return args_parser::STRING; }
template <> args_parser::arg_t get_arg_t<bool>() { return args_parser::BOOL; }

template <typename T> T get_val(const args_parser::value &v);
template <> int get_val<int>(const args_parser::value &v) { return v.i; }
template <> float get_val<float>(const args_parser::value &v) { return v.f; }
template <> bool get_val<bool>(const args_parser::value &v) { return v.b; }
template <> std::string get_val<std::string>(const args_parser::value &v) { return v.str; }

template <typename T>
void vresult_to_vector(const std::vector<args_parser::value> &in, std::vector<T> &out) {
    args_parser::arg_t t = get_arg_t<T>();
    for (size_t i = 0; i < in.size(); i++)
        out.push_back(get_val<T>(in[i]));
}

template <typename T>
args_parser::option &args_parser::add_required_option(const char *s) {
    smart_ptr<option> pd = new args_parser::option_scalar(s, get_arg_t<T>());
    expected_args[current_group].push_back(pd);
    return *pd.get();
}

template <typename T>
args_parser::option &args_parser::add_option_with_defaults(const char *s, T v) {
    smart_ptr<option> pd = new args_parser::option_scalar(s, get_arg_t<T>(), value(v));
    expected_args[current_group].push_back(pd);
    return *pd.get();
}

template <typename T>
args_parser::option &args_parser::add_required_option_vec(const char *s, char delim, int min, int max) {
    if (max > option_vector::MAX_VEC_SIZE)
        throw std::logic_error("args_parser: maximum allowed vector size for vector argument exceeded");
    smart_ptr<option> pd = new args_parser::option_vector(s, get_arg_t<T>(), delim, min, max);
    expected_args[current_group].push_back(pd);
    return *pd.get();
}

template <typename T>
args_parser::option &args_parser::add_option_with_defaults_vec(const char *s, const char *defaults, char delim, int min, int max) {
    if (max > option_vector::MAX_VEC_SIZE)
        throw std::logic_error("args_parser: maximum allowed vector size for vector argument exceeded");
    smart_ptr<option> pd = new args_parser::option_vector(s, get_arg_t<T>(), delim, min, max, defaults); 
    expected_args[current_group].push_back(pd);
    return *pd.get();
}

template <typename T>
void args_parser::get_result_vec(const std::string &s, std::vector<T> &r) const {
    std::vector<value> v = get_result_value(s);
    vresult_to_vector<T>(v, r);
}

template <typename T>
T args_parser::get_result(const std::string &s) const {
    std::vector<T> r;
    get_result_vec<T>(s, r);
    if (r.size() != 1)
        throw std::logic_error("args_parser: get_result can't get a result: zero-sized vector returned");
    return r[0];
}

template <typename T>
bool args_parser::parse_special(const std::string &s, T &r) {
    option_scalar d("[FREE ARG]", get_arg_t<T>());
    bool res = d.do_parse(s.c_str());
    if (res) {
        r = get_val<T>(d.get_value_as_vector()[0]);
    }
    return res;
}

template <typename T>
bool args_parser::parse_special_vec(const std::string &s, std::vector<T> &r, char delim, int min, int max) {
    option_vector d("[FREE ARG]", get_arg_t<T>(), delim, min, max);
    bool res = d.do_parse(s.c_str());
    if (res) {
        r = vresult_to_vector(d.get_value_as_vector(), r);
    }
    return res;
}

