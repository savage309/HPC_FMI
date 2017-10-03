
#GPGPU Architectures

>"I saw `cout` being shifted `Hello world` times to the left and stopped right there."
— Steve Gonedes

---

## *disclaimer

* GPGPU Architectures != GPU Architectures (no rops and games)
* Theory != practice
* Lack of full documentation => speculations
* Just a mental model

---

#nVidia

---

### nVidia
#### compute capability 

* 1.X - Tesla - 2XX (32bit) 
* 2.X - Fermi - 4XX, 5XX
* 3.X - Kepler - 6XX, 7XX, Quadro KXXX (not all) + all new Tesla GPUs
* 5.X - Maxwell - 9XX, Quadro MXXXX
* 6.0 - Pascal - GP100
* 6.X (X!=0) - Pascal - Titan X Pascal, 1080, 1070, etc
* 7.X - Volta, GV100

---

## FERMI

---

![](./images/9.PNG)

---

![](./images/10.PNG)

---

### FERMI

* Dynamic scheduler
* True L1 & L2 Cache
* 1 x 32
* Configurable L1 cache / Shared mem
* L2 cache
* 40nm
* Very good for GPGPU

---

## Kepler

---

![](./images/11.PNG)

---

* Static scheduler
* 6 x 32
* Bigger register file
* Non-coherent read only cache
* Configurable shared / L1
* L1 by default used for local mem only
* compiler opt in to cache global reads
* New read only / texture cache
* T __ldg(T*);
* tex1Dfetch
* const T* __restrict__
* 192 float, 4 schedulers (1x48?)
* 28nm

---

## Maxwell

---

4 x 32

![](./images/12.PNG)

---

![](./images/13.PNG)

---

![](./images/14.PNG)

---

### Maxwell

* Separate shared memory
* So, they have unified L1/Texture cache
* This can make the performance worse
* Again __ldg, tex1Dfetch, T* __restrict__
* Compiler opt in to cache global reads
​​* Spills are stored in L2
* Lower occupancy might be good
* No double precision
* 4 x 32 float
* 28nm

---

### Pascal

* Pascal - 16nm + FinFet
* GP100
* GP102, GP104, etc

---

### GP100

* HBM2
* NVLINK + API
* 2 x registers
* Virtual Memory + OS support
* Preemption
* ALUs in memory (atomics)

---

![](./images/gp100.JPG)

---

### GP10X (X != 0)

* Titan X Pascal, 1080, 1070, etc.
* Maxwell on 16nm 
* ~40% better than Maxwell
* Even better in games

---

### GV100 Volta
* Unified L1 & Shared (>7x L1 cache, >4x L1 cache bandwitdh)
* PC for each thread (simplified thread model)
* Kind of ILP, but not ILP (FP ALUs can work at the same as the Int ALUs)
* Faster NVLINK, up to 6 devices
* Faster HBM2
* New instructions (Tensor core)
* 12nm process (improved 14nm), >800m^2

---

# AMD

---

### GCN 

![](./images/15.PNG)

ver 1.0, 1.1, 1.2

---

### GCN

* No ILP ?
* Spill to global mem only
* No max registers param
* 4 x 16 float
* 28nm
* HBM
* Native FP16
* Better FP64

---

![](./images/16.PNG)

---

![](./images/17.PNG)

---

![](./images/18.PNG)

---

![](./images/19.PNG)

bandwidth x4

power /3

price *x

capacity *y

---

![](./images/20.PNG)

---

![](./images/21.PNG)

---

### Polaris

* 14nm + FinFet
* Dynamic scheduler
* Instruction pre-fetch
* Memory compression
* L2 cache

---

### Radeon SSG

* 28nm
* 2 x 512GB Samsung SSD
* ~ x4 better latency
* WIP

---

![](./images/ssg.jpg)

---

##future

* Vega
* 14nm, HBM2
* 2017 ?

---

#Intel HD Graphics

---

![](./images/22.PNG)

---

* Less-wider SIMD
* Bigger register file
* No dedicated memory (eDRAM)
* 1/10 of the compute power
* 2 x 4
* 14nm/finfet
* Bigget market share


---

#Imagination

---

## Power VR
![](./images/23.PNG)

---

![](./images/24.PNG)

---

![](./images/25.PNG)

---

![](./images/26.PNG)

---

## Tools

* nSight

* AMD CodeXL

* Intel Amplifier

---

![](./images/28.png)

---

![](./images/29.jpg)

---

![](./images/30.png)

---

#### Final notes

* Have ILP

* Make sure all caches are used (or not)

* Reduce data dependencies
​
* Reduce register usage (use -v)
​
* no inline where possible
​
* hide that with inline, where possible
​
* code morph/trim
​
* multi kernel
​
* Thread divergence is A KILLER
​
* Compiler params are super important
​
* Function call is turbo expensive (recursion, virtual)
​
* Memory access patterns 
​
* Changing all the time
​
* "Ако първо не успееш, опитай втори път." The Rescuers, 1977
​
* Everything happens for a reason 

---

* Fermi        -   580  -    512 cores - 1581GF   - 244W - 6.4
* Kepler       -  680  -   1536 cores - 3090GF  - 195W - 15.8
* Maxwell    -  980  -   2048 cores - 4612 GF - 165W - 27.95
* Pascal (GP102+) - Maxwell * 1.4 ~ 37
* AMD          - 280x  -   1792 cores - 3290GF - 190W - 17.13
* Intel          - 6200   -     384 cores -  768 GF < 47W - >16.43
​* PowerVR   - Rogue ~     192 cores ~ 115 GF < 5W   - >25

---

![](./images/cpu_gpu_energy.PNG)

---

![](./images/ooo_energy.PNG)

---

![](./images/inorder_energy.PNG)

Azizi PhD, Stanford 2010

---

![](./images/memory_energy.PNG)
