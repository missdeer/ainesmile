#pragma once


// https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/
// https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
// https://clang.llvm.org/docs/LanguageExtensions.html
#include <intrin.h>

#if defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)
	#define AS_TARGET_ARM		1
	#define AS_TARGET_ARM64	1
	#define AS_TARGET_ARM32	0
	#define AS_USE_SSE2		0
	#define AS_USE_AVX2		0
	// TODO: use ARM Neon
#elif defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
	#define AS_TARGET_ARM		1
	#define AS_TARGET_ARM64	0
	#define AS_TARGET_ARM32	1
	#define AS_USE_SSE2		0
	#define AS_USE_AVX2		0
#else
	#define AS_TARGET_ARM		0
	#define AS_TARGET_ARM64	0
	#define AS_TARGET_ARM32	0
	// SSE2 enabled by default
	#define AS_USE_SSE2		1

	// Clang and GCC use -march=x86-64-v3, https://clang.llvm.org/docs/UsersManual.html#x86
	// or -mavx2 -mpopcnt -mbmi -mbmi2 -mlzcnt -mmovbe
	// MSVC use /arch:AVX2
	#if defined(_WIN64) && defined(__AVX2__)
		#define AS_USE_AVX2	1
        #include <immintrin.h>  // AVX2 intrinsics
	#else
		#define AS_USE_AVX2	0
        #include <emmintrin.h> // SSE2 intrinsics
	#endif

	// TODO: use __isa_enabled/__isa_available in MSVC build to dynamic enable AVX2 code.
	//#if defined(_MSC_VER) || (defined(__has_include) && __has_include(<isa_availability.h>))
	//	#include <isa_availability.h>
	//#else
	//#endif

	// TODO: Function Multiversioning https://gcc.gnu.org/wiki/FunctionMultiVersioning
#endif
