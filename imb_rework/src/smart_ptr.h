#pragma once

template <typename T>
class smart_ptr {
private:
    typedef int counter_t;
public:
    smart_ptr() : pointer(NULL), rc(NULL) { }
    smart_ptr(T * p) : pointer(p), rc(p ? new counter_t(0) : NULL) { increment(); } 
    smart_ptr(const smart_ptr& rhs) : pointer(rhs.pointer), rc(rhs.rc) { increment(); }
    ~smart_ptr() {
        if(rc && decrement() == 0) { delete pointer; delete rc; }
    }
    void swap(smart_ptr& rhs) {
        std::swap(pointer, rhs.pointer);
        std::swap(rc, rhs.rc);
    }
    T *get() { return pointer; }
    const T *get() const { return pointer; }
    smart_ptr& operator=(const smart_ptr& rhs) {
        smart_ptr tmp(rhs);
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
