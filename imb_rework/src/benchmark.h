#pragma once

class Benchmark {
    public:
        Benchmark() : initialized(false) {}
        virtual const std::string get_name() const = 0;
        virtual Benchmark* create_myself() const = 0;
        virtual bool init_description() = 0;
        virtual void init() = 0;
        virtual void run() = 0;
        virtual bool is_default() = 0;
        virtual ~Benchmark() { }
        bool initialized;
    private:
        Benchmark &operator=(const Benchmark &) { return *this; }
        Benchmark(const Benchmark &) {}
};

