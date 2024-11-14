#ifndef _V_LOG_H_
#define _V_LOG_H_
#include <stdio.h>
#define FMT_OUTPUT
#ifdef  FMT_OUTPUT
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
using namespace fmt;
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#include <android/log.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Android平台
#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGD(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
// Windows平台
#define NLOGD(...) printf( __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
// Linux或Unix平台
#ifdef FMT_OUTPUT
#define NLOGD(...) print(fg(color::blue),__VA_ARGS__);
#else
#define NLOGD(...) printf( __VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGV(...) printf( __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGV(...) print(fg(color::light_gray), __VA_ARGS__);
#else
#define NLOGV(...) printf(__VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGI(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGI(...) printf(__VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGI(...) print(fg(color::green),__VA_ARGS__);
#else
#define NLOGI(...) printf(__VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGW(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGW(...) printf(__VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGW(...) print(fg(color::yellow),__VA_ARGS__);
#else
#define NLOGW(...) printf(__VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGE(...) printf(__VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGE(...) print(fg(color::red), __VA_ARGS__);
#else
#define NLOGE(...) printf(__VA_ARGS__);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
