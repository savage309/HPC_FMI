#pragma once

#include <stdio.h>
namespace Virtual {
    class NBase {
    public:
        virtual int get() const = 0;
        virtual void set(int val) = 0;
        virtual ~NBase() {};
    };
    
    class NDerived : public NBase {
        int i;
    public:
        int get() const override {
            return i;
        }
        void set(int val) override {
            i = val;
        }
    };
    
    class NDerived2 : public NBase {
        float f;
    public:
        int get() const override {
            return f;
        }
        void set(int val) override {
            f = val * 42.196f;
        }
    };
}
