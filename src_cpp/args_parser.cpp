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
#include <algorithm>

using namespace std;

const int args_parser::version = 1;

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
                s == "true" || s == "enable" || s == "TRUE" || s == "ENABLE" ||
                s == "1") {
                b = true;
            } else if (s == "off" || s == "no" || s == "OFF" || s == "NO" || 
                s == "false" || s == "disable" || s == "FALSE" || s == "DISABLE" ||
                s == "0") {
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

#ifdef WITH_YAML_CPP
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
    v.initialized = true;
}

YAML::Emitter &operator<< (YAML::Emitter& out, const args_parser::option &opt) {
    opt.to_yaml(out);
    return out;
}

void operator>> (const YAML::Node& node, args_parser::option &opt) {
    opt.from_yaml(node);
    opt.defaulted = false;
}

void args_parser::option_scalar::to_yaml(YAML::Emitter& out) const { out << val; }
void args_parser::option_scalar::from_yaml(const YAML::Node& node) { node >> val; }

void args_parser::option_vector::to_yaml(YAML::Emitter& out) const { out << val; }
void args_parser::option_vector::from_yaml(const YAML::Node& node) 
{
    node >> val;
    assert(val.size() >= (size_t)vec_min && val.size() <= (size_t)vec_max);
}
#endif

bool args_parser::option_scalar::do_parse(const char *sval) {
    if (val.initialized && parser.is_flag_set(NODUPLICATE))
        return false;
    return val.parse(sval, type); 
}

bool args_parser::option_vector::do_parse(const char *const_sval) {
    bool res = true;
    string sval(const_sval);
    std::vector<int> positions;
    for (const char *s = sval.c_str(); *s; s++) {
        if (*s == vec_delimiter)
            positions.push_back(s - sval.c_str());
    }
    positions.push_back(sval.size());
    size_t nelems = sval.size() ? positions.size() : 0;
    size_t max_elem = num_already_initialized_elems + nelems;
    if (max_elem < (size_t)vec_min || max_elem > (size_t)vec_max) 
        return false;
    val.resize(std::max(max_elem, val.size()));
    if (nelems == 0) 
        return true;
    for (size_t i = 0, j = 0; i < positions.size(); i++) {
        sval[positions[i]] = 0;
        int n = num_already_initialized_elems + i;
        if (val[n].initialized && parser.is_flag_set(NODUPLICATE))
            return false;
        res = res && val[n].parse(sval.c_str() + j, type);
        j = positions[i] + 1;
    }
    num_already_initialized_elems += positions.size();
    return res;
}

void args_parser::option_vector::set_default_value() {
    if (num_already_initialized_elems == 0) {
        do_parse(vec_def.c_str());
        defaulted = true;
        num_already_initialized_elems = 0;
    }
}

args_parser::option &args_parser::add_flag(const char *s) {
    option &opt = add<bool>(s, false);
    opt.flag = true;
    return opt;
}

bool args_parser::match(string &arg, string pattern) const {
    if (strncmp(arg.c_str(), option_starter, strlen(option_starter)))
        return false;
    if (strncmp(arg.c_str() + strlen(option_starter), pattern.c_str(), pattern.size()))
        return false;
    if (option_delimiter == ' ' && *(arg.c_str() + strlen(option_starter) + pattern.size()) != 0)
        return false;
    return true;
}

bool args_parser::match(string &arg, option &opt) const {
    return match(arg, opt.str);
}

bool args_parser::get_value(const string &arg, option &opt) {
    size_t offset = 0; 
    assert(prev_option == NULL);
    offset = strlen(option_starter);
    if (option_delimiter == ' ') {
        // save the option descriptor -- next arg will be the value
        prev_option = &opt;
        offset += opt.str.size();
        if (*(arg.c_str() + offset) != 0)
            return false;
        return true;
    } else {
        offset += opt.str.size();
        if (*(arg.c_str() + offset) != option_delimiter)
            return false;
        offset += 1;
    }
    bool res = opt.do_parse(arg.c_str() + offset);
    return res;
}

void args_parser::print_err(error_t err, string option, string extra) {
    if (!is_flag_set(SILENT))
        switch (err) {
            case NONE: break;
            case NO_REQUIRED_OPTION: 
                sout << "ERROR: The required option missing or can't be parsed: " 
                     << option_starter << option << endl;
                break;
            case NO_REQUIRED_EXTRA_ARG: 
                sout << "ERROR: The required extra argument missing" << endl;
                break;
            case PARSE_ERROR_OPTION: 
                sout << "ERROR: Parse error on option: "
                     << option_starter << option << endl;
                break;
            case PARSE_ERROR_EXTRA_ARGS: 
                sout << "ERROR: Parse error on an extra argument" << endl;
                break;
            case UNKNOWN_EXTRA_ARGS:
                sout << "ERROR: Some extra or unknown arguments or options" << endl;
                break;
            default: throw logic_error("args_parser: print_err: unknown error");
        }
    last_error = err;
    last_error_option = option;
    last_error_extra = extra;
}

#ifdef WIN_IMB
std::string basename(const char *name) {
	std::string file_name(name);
	const size_t i = file_name.find_last_of("\\/");
	if (std::string::npos != i)	{
		file_name.erase(0, i + 1);
	}
	return file_name;
}
#endif

void args_parser::print_help_advice() const {
    sout << "Try \"" <<  basename(argv[0]) << " " << option_starter << "help\" for usage information" << endl;
}

// NOTE: This one is just to loop over expected_args 2-level array in a easier way.
// First call woth FOREACH_FIRST initializes the walk throgh all expected args,
// each next call with FOREACH_NEXT gives a pointer to the next arg from expected_args
// together with the pointer to the group name it belongs
// Two versions are here for ordinary and constant methods, mind the 'const' keyword.
bool args_parser::in_expected_args(enum foreach_t t, const string *&group, smart_ptr<option> *&opt) {
    static map<const string, vector<smart_ptr<option> > >::iterator it;
    static size_t j = 0;
    if (t == FOREACH_FIRST) {
        it = expected_args.begin();
        j = 0;
        return true;
    }
    if (t == FOREACH_NEXT) {
        while (it != expected_args.end()) {
            vector<smart_ptr<option> > &expected_args = it->second;
            if (j >= expected_args.size()) {
               ++it;
               j = 0;
               continue;
            } 
            group = &(it->first);
            opt = &expected_args[j];
            j++;
            return true;
        }
        return false;
    }
    return false;
}

bool args_parser::in_expected_args(enum foreach_t t, const string *&group, const smart_ptr<option> *&opt) const {
    static map<const string, vector<smart_ptr<option> > >::const_iterator cit;
    static size_t j = 0;
    if (t == FOREACH_FIRST) {
        cit = expected_args.begin();
        j = 0;
        return true;
    }
    if (t == FOREACH_NEXT) {
        while (cit != expected_args.end()) {
            const vector<smart_ptr<option> > &expected_args = cit->second;
            if (j >= expected_args.size()) {
               ++cit;
               j = 0;
               continue;
            } 
            group = &(cit->first);
            opt = &expected_args[j];
            j++;
            return true;
        }
        return false;
    }
    return false;
}

void args_parser::print_single_option_usage(const smart_ptr<option> &opt, size_t header_size, 
        bool is_first, bool no_option_name) const {
    string tab(header_size, ' ');
    const char *open_brace = "[";
    const char *close_brace = "]";
    const char *empty = "";
    const char *open = opt->required ? empty : open_brace;
    const char *close = opt->required ? empty : close_brace;
    const string stype = value::get_type_str(opt->type);
    const string cap = (opt->caption.size() == 0 ? stype : opt->caption);
    const string allign = (is_first ? "" : tab);
    if (no_option_name)
        sout << allign << open << cap << close << " ";
    else if (opt->flag)
        sout << allign << open << option_starter << opt->str << close << endl;
    else
        sout << allign << open << option_starter << opt->str << option_delimiter << cap << close << endl;
}

void args_parser::print_help() const {
    if (program_name.size() != 0)
        sout << program_name << endl;
    sout << "Usage: " << basename(argv[0]) << " ";
    string header;
    header +=  "Usage: ";
    header += basename(argv[0]); 
    header += " ";
    size_t size = min(header.size(), (size_t)16);
    string tab(size - 2, ' ');
    bool is_first = true;
    bool is_there_sys_group = false, is_there_empty_group = false;
    // help
    smart_ptr<option> help = new option_scalar(*this, "help", BOOL, value(false)); 
    help->flag = true;
    print_single_option_usage(help, size, is_first);
    // help option
    is_first = false;
    help->flag = false;
    help->set_caption("option");
    print_single_option_usage(help, size, is_first);
    // enumarate all groups which are here
    vector<string> groups;
    map<const string, vector<smart_ptr<option> > >::const_iterator cit;
    for (cit = expected_args.begin(); cit != expected_args.end(); ++cit) {
        groups.push_back(cit->first);
        if (cit->first == "SYS")
            is_there_sys_group = true;
        if (cit->first == "")
            is_there_empty_group = true;
    }
    // "SYS" option go first
    if (is_there_sys_group) {
        const vector<smart_ptr<option> > &args = expected_args.find("SYS")->second;
        for (size_t i = 0; i < args.size(); i++)
            print_single_option_usage(args[i], size, is_first);
    }
    // option from unnamed group go next
    if (is_there_empty_group) {
        const vector<smart_ptr<option> > &args = expected_args.find("")->second;
        for (size_t i = 0; i < args.size(); i++)
            print_single_option_usage(args[i], size, is_first);
    }
    // options from groups in the order they where added
    for (size_t group = 0; group < groups.size(); group++) {
        const vector<smart_ptr<option> > &args = expected_args.find(groups[group])->second;
        if (groups[group] == "EXTRA_ARGS" || groups[group] == "SYS" || groups[group] == "")
            continue;
        sout << tab << groups[group] << ":" << endl;
        for (size_t i = 0; i < args.size(); i++)
            print_single_option_usage(args[i], size, is_first);
    }
    // extra args
    int num_extra_args = 0, num_required_extra_args = 0;
    const std::vector<smart_ptr<option> > &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    for (int j = 0; j < num_extra_args; j++) 
        print_single_option_usage(extra_args[j], size, is_first, true);
    if (num_extra_args)
        sout << endl;
}

void args_parser::print_help(string str) const {
    if (program_name.size() != 0)
        sout << program_name << endl;
    bool was_printed = false;
    const string *pgroup;
    const smart_ptr<option> *popt;
    in_expected_args(FOREACH_FIRST, pgroup, popt);
    while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
        const smart_ptr<option> &opt = *popt;
        if (opt->str == str) {
            sout << "Option: ";
            print_single_option_usage(opt, 0, true);
            if (*pgroup != "SYS" && *pgroup != "")
                sout << "Group: " << *pgroup << endl;
            if (opt->description != "") {
                sout << endl << opt->description << endl;
            } 
            was_printed = true;
        }
    }
    if (!was_printed) {
        sout << "No such option: " << str << endl;
        print_help_advice();
    }
}

void args_parser::print() const {
    const string *pgroup;
    const smart_ptr<option> *popt;
    in_expected_args(FOREACH_FIRST, pgroup, popt);
    while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
        (*popt)->print();
    }
}

const vector<smart_ptr<args_parser::option> > &args_parser::get_extra_args_info(int &num_extra_args, int &num_required_extra_args) const {
    const vector<smart_ptr<option> > &extra_args = expected_args.find("EXTRA_ARGS")->second;
    bool required_args_ended = false;
    for (size_t j = 0; j < extra_args.size(); j++) {
        if (extra_args[j]->required) {
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

vector<smart_ptr<args_parser::option> > &args_parser::get_extra_args_info(int &num_extra_args, int &num_required_extra_args) {
    vector<smart_ptr<option> > &extra_args = expected_args["EXTRA_ARGS"];
    for (size_t j = 0; j < extra_args.size(); j++) {
        if (extra_args[j]->required)
            num_required_extra_args++;
    } 
    num_extra_args = extra_args.size();
    return extra_args;
}

void args_parser::get_command_line(std::string &result) const {
    for (int n = 0; n < argc; n++) {
        result += argv[n];
        if (n < argc-1)
            result += " ";
    }
}

bool args_parser::parse() {
    bool parse_result = true;
    bool help_printed = false;
    unknown_args.resize(0);
    // go through all given args
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        // if there is a pointer to a optioniptor which corresponds to previous argv[i]
        if (prev_option) {
            // the option itself was given as a previous argv[i] 
            // now only parse the option argument
            option &opt = *prev_option;
            if (!opt.required && opt.defaultize_before_parsing) 
                opt.set_default_value();
            opt.defaulted = false;
            if (!opt.do_parse(arg.c_str())) {
                print_err(PARSE_ERROR_OPTION, opt.str, arg);
                parse_result = false;
            }
            prev_option = NULL;
            continue;
        }
        // help is hardcoded as and optional 1st arg
        if (i == 1 && match(arg, string("help")) && !is_flag_set(NOHELP)) {
            if (argc == 3) {
                print_help(string(argv[2]));
            } else {
                print_help();
            }
            parse_result = false;
            help_printed = true;
        }
        // go throwgh all expected_args[] elements to find the option by pattern
        bool found = false;
        const string *pgroup;
        smart_ptr <option> *popt;
        in_expected_args(FOREACH_FIRST, pgroup, popt);
        while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
            if (*pgroup == "EXTRA_ARGS")
                continue;
            if (match(arg, **popt)) {
                if (!(*popt)->required && (*popt)->defaultize_before_parsing)
                    (*popt)->set_default_value();
                (*popt)->defaulted = false;
                if ((*popt)->flag) {
                    (*popt)->do_parse("on");
                    found = true;
                    break;
                }
                if (!get_value(arg, **popt)) {
                    print_err(PARSE_ERROR_OPTION, (*popt)->str, arg);
                    parse_result = false;
                }
                found = true;
                break;
            }
        }
        // all unmatched args are stored in a separate array to handle them later
        if (!found)
            unknown_args.push_back(arg);
    }

    // the case when cmdline args ended too early
    if (prev_option != NULL) {
        print_err(PARSE_ERROR_OPTION, prev_option->str);
        parse_result = false;
    }

    // now parse the expected extra agrs
    int num_extra_args = 0, num_required_extra_args = 0;
    std::vector<smart_ptr<option> > &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (unknown_args.size() < (size_t)num_required_extra_args) {
        print_err(NO_REQUIRED_EXTRA_ARG, "");
        parse_result = false;
    } else {
        int num_processed_extra_args = 0;
        for (size_t j = 0; j < extra_args.size(); j++) {
            if (j >= unknown_args.size())
               break;
            if (match(unknown_args[j], "")) 
                continue;
            if (!extra_args[j]->required && extra_args[j]->defaultize_before_parsing)
                extra_args[j]->set_default_value();
            extra_args[j]->defaulted = false;
            if (!extra_args[j]->do_parse(unknown_args[j].c_str())) {
                print_err(PARSE_ERROR_EXTRA_ARGS, "", unknown_args[j]);
                parse_result = false;
                break;
            }
            num_processed_extra_args++;
        }
        assert((size_t)num_processed_extra_args <= unknown_args.size());
        unknown_args.erase(unknown_args.begin(), unknown_args.begin() + num_processed_extra_args);
    }

    // loop again through all in expected_args[] to find options which were not given in cmdline
    const string *pgroup;
    smart_ptr<option> *popt;
    in_expected_args(FOREACH_FIRST, pgroup, popt);
    while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
        if ((*popt)->is_default_setting_required()) {
            (*popt)->set_default_value();
            continue;
        }
        if ((*popt)->is_required_but_not_set()) {
            print_err(NO_REQUIRED_OPTION, (*popt)->str);
            parse_result = false;
        }
    }
    // if there are too many unexpected args, raise an error
    if (!is_flag_set(ALLOW_UNEXPECTED_ARGS)) {
        if (parse_result && unknown_args.size()) {
            print_err(UNKNOWN_EXTRA_ARGS, "");
            parse_result = false;
        }
    }
    if (!parse_result && !is_flag_set(SILENT) && !help_printed)
        print_help_advice();
    return parse_result;
}

args_parser::option &args_parser::set_caption(int n, const char *cap) {
    int num_extra_args = 0, num_required_extra_args = 0;
    vector<smart_ptr<option> > &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (n >= num_extra_args)
        throw logic_error("args_parser: no such extra argument");
    extra_args[n]->caption.assign(cap);
    return *extra_args[n];
}

vector<args_parser::value> args_parser::get_result_value(const string &s) const {
    const string *pgroup;
    const smart_ptr<option> *popt;
    in_expected_args(FOREACH_FIRST, pgroup, popt);
    while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
        if ((*popt)->str == s) {
            return (*popt)->get_value_as_vector();
        }
    }
    throw logic_error("args_parser: no such option");
}

void args_parser::get_unknown_args(vector<string> &r) const {
    for (size_t j = 0; j < unknown_args.size(); j++) {
        r.push_back(unknown_args[j]);
    }
}

#ifdef WITH_YAML_CPP
bool args_parser::load(istream &stream) {
    try {
        YAML::Parser parser(stream);
        YAML::Node node;
        parser.GetNextDocument(node);
        // loop through all in expected_args[] to find each option in file 
        const string *pgroup;
        smart_ptr<option> *popt;
        in_expected_args(FOREACH_FIRST, pgroup, popt);
        while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
            if (*pgroup == "SYS" || *pgroup == "EXTRA_ARGS")
                continue;
            if(const YAML::Node *pName = node.FindValue((*popt)->str.c_str())) {
                *pName >> **popt;
            }
        }
        int num_extra_args = 0, num_required_extra_args = 0;
        std::vector<smart_ptr<option> > &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
        if(const YAML::Node *pName = node.FindValue("extra_args")) {
            int j = 0;
            for(YAML::Iterator it = pName->begin(); it != pName->end(); ++it) {
                if (j == num_extra_args) 
                    break;
                popt = &extra_args[j];
                    *it >> **popt;
            }
        }
    }
    catch (const YAML::Exception& e) {
        sout << "ERROR: input YAML file parsing error: " << e.what() << endl;
        return false;
    }
    // now do regular parse procedure to complete: 1) cmdline options ovelapping: 
    // what is given in cmdline has a priority; 2) filling in non-required options 
    // with defaults.
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
    out << YAML::BeginDoc;
    if (program_name.size() != 0)
        out << YAML::Comment(program_name.c_str());
    out << YAML::BeginMap;
    out << YAML::Flow;
    out << YAML::Key << "version";
    out << YAML::Value << version;
    const string *pgroup;
    const smart_ptr<option> *popt;
    in_expected_args(FOREACH_FIRST, pgroup, popt);
    while(in_expected_args(FOREACH_NEXT, pgroup, popt)) {
        if (*pgroup == "SYS" || *pgroup == "EXTRA_ARGS")
            continue;
        if ((*popt)->defaulted) {
            YAML::Emitter comment;
            comment << YAML::BeginMap;
            comment << YAML::Flow << YAML::Key << (*popt)->str.c_str();
            comment << YAML::Flow << YAML::Value << **popt;
            comment << YAML::EndMap;
            out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
        } else {
            out << YAML::Key << (*popt)->str.c_str();
            out << YAML::Value << **popt;
        }
    }
    int num_extra_args = 0, num_required_extra_args = 0;
    const std::vector<smart_ptr<option> > &extra_args = get_extra_args_info(num_extra_args, num_required_extra_args);
    if (num_extra_args > 0) {
        out << YAML::Key << "extra_args";
        out << YAML::Value << YAML::BeginSeq << YAML::Newline;
        for (int i = 0; i < num_extra_args; i++) {
            popt = &extra_args[i];
            if ((*popt)->defaulted) {
                YAML::Emitter comment;
                comment << YAML::Flow << **popt;
                out << YAML::Flow << YAML::Newline << YAML::Comment(comment.c_str()) << YAML::Comment("(default)");
            } else {
                out << **popt;
            }
        }
        out << YAML::Newline << YAML::EndSeq;
    }
    out << YAML::EndMap;
    out << YAML::Newline;
    return string(out.c_str());
}
#endif

bool args_parser::is_option(const string &str) const {
    if (strncmp(str.c_str(), option_starter, strlen(option_starter)) == 0) return true;
    return false;
}


ostream &operator<<(ostream &s, const args_parser::option &opt) {
    opt.to_ostream(s);
    return s;
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

template <> args_parser::arg_t get_arg_t<int>() { return args_parser::INT; }
template <> args_parser::arg_t get_arg_t<float>() { return args_parser::FLOAT; }
template <> args_parser::arg_t get_arg_t<std::string>() { return args_parser::STRING; }
template <> args_parser::arg_t get_arg_t<bool>() { return args_parser::BOOL; }

template <> int get_val<int>(const args_parser::value &v) { return v.i; }
template <> float get_val<float>(const args_parser::value &v) { return v.f; }
template <> bool get_val<bool>(const args_parser::value &v) { return v.b; }
template <> std::string get_val<std::string>(const args_parser::value &v) { return v.str; }

