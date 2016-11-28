#include <stdio.h>
#include <stdint.h>
#include "timer.h"

#if __linux__ != 0
	// linux is LP64
	#define PRINT_U64		"%lu"
	#define LITERAL_U64(x)	x##UL
#elif _WIN64 != 0
	// windows is LLP64
	#define PRINT_U64		"%llu"
	#define LITERAL_U64(x)	x##ULL
#elif __APPLE__ != 0
	// osx is LP64
	#define PRINT_U64		"%lu"
	#define LITERAL_U64(x)	x##UL
#else
	#error unsupported target platform
#endif

// target ISA support
#define TARGET_ISA_AMD64	1
#define TARGET_ISA_ARM64	2

#if __x86_64__ != 0 || _M_AMD64 != 0
	#if _M_AMD64 != 0
		#include <intrin.h> // emmintrin.h is also present but it's incomplete under msvc
	#else
		#include <emmintrin.h>
	#endif
	#define TARGET_ISA	TARGET_ISA_AMD64

#elif __ARM_ARCH == 8 || _M_ARM64 != 0
	#include <arm_neon.h>
	#define TARGET_ISA	TARGET_ISA_ARM64

#else
	#error unsupported target ISA

#endif

// compiler support
#if _WIN64 != 0
	#define ALIGN_DECL(alignment, decl) \
	__declspec (align(alignment)) decl

#else
	#define ALIGN_DECL(alignment, decl) \
	decl __attribute__ ((aligned(alignment)))

#endif

#define TOP_ALIGN	32

ALIGN_DECL(TOP_ALIGN, const volatile uint64_t fib[]) = { 1, 1, 2, 3 };

//
// you can add your declarations here, given:
// 1) they do not exceed 32 bytes in total (paddings for alignment non-withstanding)
// 2) all declarations (even const) are volatile
//

// fill this array with the remaining members of the fibonacci seqeunce (started by fib[] above) up to the 93rd member
ALIGN_DECL(TOP_ALIGN, uint64_t res[93 - sizeof(fib) / sizeof(fib[0])]);

int main(int, char**) {
	const uint64_t t0 = timer_nsec();

	const size_t rep = size_t(1e7);
	for (size_t i = 0; i < rep; ++i) {
		// computations follow (your code goes here)
		uint64_t t_1 = fib[sizeof(fib) / sizeof(fib[0]) - 2];
		uint64_t t_0 = fib[sizeof(fib) / sizeof(fib[0]) - 1];

		const size_t trip = 93 - sizeof(fib) / sizeof(fib[0]);
		for (size_t j = 0; j < trip; ++j) {
			const uint64_t t = t_1 + t_0;
			t_1 = t_0;
			t_0 = t;
			res[j] = t;
		}
	}

	const uint64_t dt = timer_nsec() - t0;
	const size_t res_last_i = sizeof(res) / sizeof(res[0]) - 1;

	for (size_t i = 2; i <= res_last_i; ++i) {
		if (res[i - 2] + res[i - 1] != res[i]) {
			fprintf(stderr, "error at " PRINT_U64 " (" PRINT_U64 ", " PRINT_U64 ", " PRINT_U64 ")\n", i, res[i], res[i - 1], res[i - 2]);
			return -1;
		}
	}
	if (res[res_last_i] != LITERAL_U64(12200160415121876738)) {
		fprintf(stderr, "error (" PRINT_U64 ")\n", res[res_last_i]);
		return -1;
	}

	fprintf(stderr, PRINT_U64 ", %f\n", res[res_last_i], 1e-9 * dt);
	return 0;
}

