#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#ifdef __cplusplus
#include <cstdint>      // For integer types
#include <cstddef>      // For size_t and ptrdiff_t
#include <type_traits>  // For static_assert
#else
#include <stdbool.h>
#include <stdint.h>     // For integer types in C99
#include <stddef.h>     // For size_t and ptrdiff_t in C99
#endif

// Define all types --------------------------------------------------------

#ifdef __cplusplus // C++
using uni = void;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u24 = std::uint32_t; // u24: Simulated 24-bit type (uses uint32_t for compatibility and simplicity)
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using c8 = char;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i24 = std::int32_t; // u24: Simulated 24-bit type (uses uint32_t for compatibility and simplicity)
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;
using f128 = long double; // Platform-dependent type

using b = bool;

// Platform-dependent register types --------------------------------------

using reg = std::size_t;
using sreg = std::ptrdiff_t;

#else // C99

typedef void uni;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u24; // Assuming no native 24-bit type, use uint32_t
typedef uint32_t u32;
typedef uint64_t u64;

typedef char c8;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i24; // Assuming no native 24-bit type, use int32_t
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
typedef long double f128; // Platform-dependent type

typedef _Bool b;

// Platform-dependent register types --------------------------------------

typedef size_t reg;
typedef ptrdiff_t sreg;

#endif // __cplusplus

// User types checks ------------------------------------------------------

// STATIC_ASSERT for C99 and C++
#if defined(__cplusplus) || (__STDC_VERSION__ >= 201112L) // If C version equal or more than C11 or C++
#include <assert.h>

static_assert(sizeof(u8) == 1, "size of uint8 type must be equal to 1, change typedef u8");
static_assert(sizeof(u16) == 2, "size of uint16 type must be equal to 2, change typedef u16");
static_assert(sizeof(u24) == 4, "size of uint24 type must be equal to 4, change typedef u24");
static_assert(sizeof(u32) == 4, "size of uint32 type must be equal to 4, change typedef u32");
static_assert(sizeof(u64) == 8, "size of uint64 type must be equal to 8, change typedef u64");

static_assert(sizeof(c8) == 1, "size of char type must be equal to 1, change typedef c8");
static_assert(sizeof(i8) == 1, "size of int8 type must be equal to 1, change typedef i8");
static_assert(sizeof(i16) == 2, "size of int16 type must be equal to 2, change typedef i16");
static_assert(sizeof(i24) == 4, "size of int24 type must be equal to 4, change typedef i24");
static_assert(sizeof(i32) == 4, "size of int32 type must be equal to 4, change typedef i32");
static_assert(sizeof(i64) == 8, "size of int64 type must be equal to 8, change typedef i64");

static_assert(sizeof(f32) == 4, "size of float type must be equal to 4, change typedef f32");
static_assert(sizeof(f64) == 8, "size of double type must be equal to 8, change typedef f64");
//static_assert(sizeof(f128) == 16, "size of long double type must be equal to 16, change typedef f128"); // Platform-dependent
//#ifdef __SIZEOF_LONG_DOUBLE__
//static_assert(__SIZEOF_LONG_DOUBLE__ >= 8, "long double must be at least 16 bytes for f128");
//#endif /* __SIZEOF_LONG_DOUBLE__ */

static_assert(sizeof(float) == sizeof(u32), "Unexpected float format");
static_assert(sizeof(double) == sizeof(u64), "Unexpected double format");

static_assert(sizeof(b) == 1, "size of bool type must be equal to 1, change typedef b");

static_assert(sizeof(reg) == sizeof(uni*), "size of reg type must be equal to pointer size, change typedef reg");
static_assert(sizeof(sreg) == sizeof(uni*), "size of sreg type must be equal to pointer size, change typedef sreg");

#else // If older C version
#define STATIC_ASSERT(COND, MSG) typedef int static_assertion_##MSG[(COND) ? 1 : -1]

// unsigned
STATIC_ASSERT(sizeof(u8) == 1, size_of_uint8_type_must_be_equal_1);
STATIC_ASSERT(sizeof(u16) == 2, size_of_uint16_type_must_be_equal_2);
STATIC_ASSERT(sizeof(u24) == 4, size_of_uint24_type_must_be_equal_4);
STATIC_ASSERT(sizeof(u32) == 4, size_of_uint32_type_must_be_equal_4);
STATIC_ASSERT(sizeof(u64) == 8, size_of_uint64_type_must_be_equal_8);

// signed
STATIC_ASSERT(sizeof(c8) == 1, size_of_char_type_must_be_equal_1);
STATIC_ASSERT(sizeof(i8) == 1, size_of_int8_type_must_be_equal_1);
STATIC_ASSERT(sizeof(i16) == 2, size_of_int16_type_must_be_equal_2);
STATIC_ASSERT(sizeof(i24) == 4, size_of_int24_type_must_be_equal_4);
STATIC_ASSERT(sizeof(i32) == 4, size_of_int32_type_must_be_equal_4);
STATIC_ASSERT(sizeof(i64) == 8, size_of_int64_type_must_be_equal_8);

// floating point
STATIC_ASSERT(sizeof(f32) == 4, size_of_float_type_must_be_equal_4);
STATIC_ASSERT(sizeof(f64) == 8, size_of_double_type_must_be_equal_8);
// STATIC_ASSERT(sizeof(f128) == 16, size_of_long_double_type_must_be_equal_16); // Platform-dependent

STATIC_ASSERT(sizeof(float) == sizeof(u32), Unexpected_float_format);
STATIC_ASSERT(sizeof(double) == sizeof(u64), Unexpected_double_format);

// bool type
STATIC_ASSERT(sizeof(b) == 1, size_of_bool_type_must_be_equal_1);

// reg types
STATIC_ASSERT(sizeof(reg) == sizeof(uni*), size_of_reg_type_must_be_equal_pointer_size);
STATIC_ASSERT(sizeof(sreg) == sizeof(uni*), size_of_sreg_type_must_be_equal_pointer_size);

#undef STATIC_ASSERT
#endif /* If older C version */

#endif /* BASIC_TYPES_H_ */
