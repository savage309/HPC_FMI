#pragma once

#include <stdio.h>
#include <stdint.h>
#include <functional>


namespace Virtual {
    
    //add some dummy defines for having semantic in some places
#define CLASS(X) X
#define INTERFACE(X) X
#define NAME(X) X
#define RETURN(X) X
#define PARAM0(X) X
#define PARAM1(X) X
    
    //our virtual functions will be static functions with "this" as 1st argument
#define VIRTUAL static
    
    //declare (virtual) interface method; syntax:
    //VTABLE_METHODN_DEFINE(0, InterfaceName, MethodName, MethodReturnType, Param0Type, ..., ParamN-1Type)
    //first int goes from 0 to n (every next method has idx the idx of the previous+1)
    //*this should be used to declare the interface methods in the interface class*
#define VTABLE_METHOD0_DEFINE(IDX, CLASS, NAME, RETURNTYPE) \
RETURNTYPE NAME() {  typedef RETURNTYPE (*name_t)(CLASS&); return ((name_t)(vtbl[this->tag*CLASS_LAST + IDX]))(*this); }
    
    
#define VTABLE_METHOD1_DEFINE(IDX, CLASS, NAME, RETURNTYPE, PARAM0) \
RETURNTYPE NAME(PARAM0 p0) { typedef RETURNTYPE (*name_t)(CLASS&, PARAM0); return ((name_t)(vtbl[this->tag*CLASS_LAST + IDX]))(*this, p0); }
    
    //fills in vtable; syntax:
    ///REGISTER_METHODN(TheMethodIdx, Interface, ClassThatImplementsTheInterface, MethodReturnType, Param0Type, ..., ParamN-1Type)
    ///*to be used in the "init" proc*
#define REGISTER_METHOD0(METHOD, BASE, DERIVE, RETURNTYPE, NAME) { RETURNTYPE (*ptr)(BASE&); ptr = DERIVE::NAME; BASE::vtbl[DERIVE::ID*CLASS_LAST + METHOD] = (BASE::FP)ptr; }
#define REGISTER_METHOD1(METHOD, BASE, DERIVE, RETURNTYPE, NAME, PARAM0) { RETURNTYPE (*ptr)(BASE&, PARAM0); ptr = DERIVE::NAME; BASE::vtbl[DERIVE::ID*CLASS_LAST + METHOD] = (BASE::FP)ptr; }
    
#define TAG(X) tag = X;
    
    //every class hierarchy with all of its classes has separate enum
    //we use this and the next enum to calculate the method position in the vtable
    enum {
        CLASS_BASE = 0,
        CLASS_DERIVED,
        CLASS_DERIVED_2,
        CLASS_LAST=CLASS_DERIVED_2,//because of some funny ptr arithmetic
    };
    //lets give names to the method indexes
    enum {
        METHOD_GET=0,
        METHOD_SET=1,
        METHOD_LAST,
    };

    
    class Base {
    protected:
        //specifies the object type
        uint8_t tag;
    public:
        //specifies the class type
        static const int ID = CLASS_BASE;
        //**
        //vtable
        typedef void (*FP)();
        static FP vtbl[(CLASS_LAST+1) * METHOD_LAST];
        //**
        Base() {
            //set the obj type
            TAG(ID);
        }
        
        //define the interface methods
        VTABLE_METHOD0_DEFINE( METHOD_GET,
                              INTERFACE(Base),
                              NAME(get),
                              RETURN(int)
                              );
        
        VTABLE_METHOD1_DEFINE( METHOD_SET,
                              INTERFACE(Base),
                              NAME(set),
                              RETURN(void),
                              PARAM0(int)
                              );
        
        //implement the interface methods
        VIRTUAL int get(Base& self) {
            throw std::bad_function_call();
        }
        
        VIRTUAL void set(Base& self, int) {
            throw std::bad_function_call();
        }
    };
    
    //
    
    class Derived : public Base {
        int i;
    public:
        static const int ID = CLASS_DERIVED;
        
        Derived() {
            TAG(ID);
        }
        
        VIRTUAL void set(Base& self, int j) {
            Derived& myself = static_cast<Derived&>(self);
            myself.i = j;
        }
        
        VIRTUAL int get(Base& self) {
            Derived& myself = static_cast<Derived&>(self);
            
            int res = myself.i;
            return res;
        }
    };
    
    //
    
    class Derived2 : public Derived {
        float f;
    public:
        static const int ID = CLASS_DERIVED_2;
        
        Derived2() {
            TAG(ID);
        }
        
        VIRTUAL void set(Base& self, int j) {
            Derived2& myself = static_cast<Derived2&>(self);
            
            myself.f = j * 42.196f;
        }
        
        VIRTUAL int get(Base& self) {
            Derived2& myself = static_cast<Derived2&>(self);
            float res = myself.f;
            return res;
        }
    };
    void init/*vtable*/();
}