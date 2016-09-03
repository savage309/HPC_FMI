const int SIZE = 16;

#if 1
    #include <stdio.h>
    #include <stdlib.h>

    float* x = (float*)malloc(sizeof(float));

    typedef const int (& immutable_array_of_16_floats)[SIZE];
    struct InputArray {
        int array[SIZE];
        InputArray() {
            array[0] = 1;
            array[1] = 2;
            array[2] = 3;
            array[3] = 4;
            array[4] = 5;
            array[5] = 6;
            array[6] = 7;
            array[7] = 8;
            array[8] = 9;
            array[9] = 10;
            array[10] = 11;
            array[11] = 12;
            array[12] = 13;
            array[13] = 14;
            array[14] = 15;
            array[15] = 16;
        }
        
        operator immutable_array_of_16_floats () const {
            return array;
        }
    };

    const InputArray input;
    int output[16];
    //#include <iostream>
    //
    int main(int, char**) {
        //std::cout << "test";
        for (size_t i = 0; i < SIZE; ++i)
            output[i] = input[i] + input[i];
        
        for (size_t i = 0; i < SIZE; ++i)
            printf("%i", output[i]);
        return 0;
    }

#else

#include <stdio.h>

    //const
    float input[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    float output[SIZE];

    int main(int, char**) {
        
        for (size_t i = 0; i < SIZE; ++i)
        output[i] = input[i] + input[i];
        
        
        for (size_t i = 0; i < SIZE; ++i)
            printf("%f", output[i]);
        
        return 0;
    }

#endif
