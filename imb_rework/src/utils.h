#pragma once 

namespace set_operations {
    template <typename T1, typename T2>
    void combine(T1 &to, T2 &from) {
        copy(from.begin(), from.end(), inserter(to, to.end()));
    }
    template <typename T1, typename T2>
    void exclude(T1 &to, T2 &from) {
        for (typename T2::iterator from_it = from.begin();
             from_it != from.end(); ++from_it) {
            typename T1::iterator it = to.find(*from_it);
            if (it != to.end()) {
                to.erase(*it);
            }
        }
    }
    template <typename T1, typename T2>
    void diff(T1 &one, T2 &two, T2 &result) {
        T2 tmp;
        copy(two.begin(), two.end(), inserter(tmp, tmp.end()));
        set_difference(one.begin(), one.end(), tmp.begin(), tmp.end(), inserter(result, result.end()));
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
}

