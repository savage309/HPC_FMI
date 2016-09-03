__kernel void square(
   __global const float* restrict input,
   __global float* restrict output,
   const unsigned int count)
{
   int i = get_global_id(0);
   if(i < count)
       output[i] = input[i] * input[i];
}
