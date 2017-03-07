#pragma once
#include "smart_ptr.h"
#include "scope.h"
#include <iostream>

class Benchmark {
    public:
        Benchmark() : initialized(false) {}
        virtual const std::string get_name() const = 0;
        virtual Benchmark* create_myself() const = 0;
        virtual void allocate_internals() {}
        virtual bool init_description() { return true; }
        virtual void init() { } 
        virtual void run(const std::pair<int, int> &) = 0;
        virtual void finalize() { }
        virtual bool is_default() { return true; }
        smart_ptr<Scope> get_scope() { if (scope.get() == NULL) { scope.assign(new Scope); scope->commit(); } return scope; }
        virtual ~Benchmark() { }
        bool initialized;
    protected:
        smart_ptr<Scope> scope;
    private:
        Benchmark &operator=(const Benchmark &) { return *this; }
        Benchmark(const Benchmark &) {}
};

#define DEFINE_INHERITED(CLASS, SUITE_CLASS) virtual const std::string get_name() const { return name; } \
    virtual Benchmark *create_myself() const { return new CLASS; } \
    CLASS() { SUITE_CLASS::register_elem(this); this->allocate_internals(); }

#define DECLARE_INHERITED(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME;

