#pragma once
#include "smart_ptr.h"
#include "scope.h"

class Benchmark {
    public:
        Benchmark() : initialized(false) {}
        virtual const std::string get_name() const = 0;
        virtual Benchmark* create_myself() const = 0;
        virtual bool init_description() = 0;
        virtual void init() = 0;
        virtual void run(const std::pair<int, int> &) = 0;
        virtual bool is_default() = 0;
        smart_ptr<Scope> get_scope() { if (scope.get() == NULL) { scope.assign(new Scope); scope->commit(); } return scope; }
        virtual ~Benchmark() { }
        bool initialized;
    protected:
        smart_ptr<Scope> scope;
    private:
        Benchmark &operator=(const Benchmark &) { return *this; }
        Benchmark(const Benchmark &) {}
};

