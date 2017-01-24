#pragma once

template <typename T>
class counted_ptr {
private:
    typedef int counter_t;
public:
    counted_ptr() : pointer(NULL), rc(NULL) { }
    counted_ptr(T * p) : pointer(p), rc(p ? new counter_t(0) : NULL) { increment(); } 
    counted_ptr(const counted_ptr& rhs) : pointer(rhs.pointer), rc(rhs.rc) { increment(); }
    ~counted_ptr() {
        if(rc && decrement() == 0) { delete pointer; delete rc; }
    }
    void swap(counted_ptr& rhs) {
        std::swap(pointer, rhs.pointer);
        std::swap(rc, rhs.rc);
    }
    T *get() { return pointer; }
    const T *get() const { return pointer; }
    counted_ptr& operator=(const counted_ptr& rhs) {
        counted_ptr tmp(rhs);
        this->swap(tmp);
        return *this;
    }
    T& operator*() { return *pointer; }
    const T& operator*() const { return *pointer; }
    T* operator->() { return pointer; }
    const T* operator->() const { return pointer; }
private:
    void increment() { if(rc) ++(*rc); }
    counter_t decrement() { return --(*rc); }

    T *pointer;
    counter_t *rc;
};
