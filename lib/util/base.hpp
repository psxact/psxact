#ifndef UTIL_BASE_HPP_
#define UTIL_BASE_HPP_

#if defined(__GNUC__)
#define attr_printf(s, n) __attribute__((format (printf, s, n)))
#define attr_noreturn __attribute__((noreturn))
#else
#define attr_printf(s, n)
#define attr_noreturn
#endif

#endif // UTIL_BASE_HPP_
