//
//  tests.c
//  
//
//  Created by savage309 on 22.09.15 г..
//
//

#include <stdio.h>

//*******************************************************************

namespace BitHacks {
//actually slower
//why ?
void swapXOR(int& a, int& b) {
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
}
//(x ^ y) ^ y = x
//--------------------------------------------
//|input    | 0        | 1        | 2        |
//|------------------------------------------|
//|10111101 | 10010011 | 10010011 | 00101110 |
//|00101110 | 00101110 | 10111101 | 10111101 |
//|-------------------------------------------

//*******************************************************************

//unpredictable branch?
int min0(int x, int y) {
    if (x < y)
        return x;
    return y;
}

//if x < y, -(x<y)=-1, which is all ones in two's complement represent => y^(x^y)=x
//4 cycles = L1 cache access
int min1(int x, int y) {
    return y ^ ((x^y) & - (x < y));
}

//*******************************************************************

int mod0(int x, int y, int n) {
    return (x + y) % n;
}

// (x + y) mod n, 0 <= x < n, 0 <= y< n
int mod1(int x, int y, int n) {
    int z = x + y;
    //unpredictable branch
    return r = (z < n) ? z : z - n;
}

int mod2(int x, int y, int n) {
    int z = x + y;
    //same as minus
    return z - (n & - (z >= n));
}
    
//*******************************************************************
//***mind f stuff
//*******************************************************************
    
int roundUpToPowerOf2(int64_t n) {
                 // 10010
    //in case if power of 2, decrement
    --n;         // 10001
    n |= n >> 1; // 11001
    n |= n >> 2; // 11101
    n |= n >> 4; // 11111
    n |= n >> 8; //
    n |= n >> 16;//
    n |= n >> 32;//
    ++n;         //100000
    return n;
}

int leastSignifcant1(int x) {
    return x & (-x);
} //+https://en.wikipedia.org/wiki/De_Bruijn_sequence
 

int numberOf1s(int x) {
    
    for (int r = 0; x != 0; ++r)
        x &= (x - 1);
    return x;
}
    
//*******************************************************************
//context
//how to find if a variable is allocated on the heap or on the stack
//
//how much is the overhead of the RAII pattern ?
}
//

//adding additional information
//reference counting
//memoize
//lazy

