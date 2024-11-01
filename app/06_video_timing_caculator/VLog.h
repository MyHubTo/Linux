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
#define NLOGD(fmt, ...) printf(fmt, __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
// Linux或Unix平台
#ifdef FMT_OUTPUT
#define NLOGD(fmt, ...) print(fg(color::blue),fmt, __VA_ARGS__);
#else
#define NLOGD(fmt, ...) printf(fmt, __VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGV(fmt, ...) printf(fmt, __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGV(fmt, ...) print(fg(color::light_gray),fmt, __VA_ARGS__);
#else
#define NLOGV(fmt, ...) printf(fmt, __VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGI(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGI(fmt, ...) printf(fmt, __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGI(fmt, ...) print(fg(color::green),fmt, __VA_ARGS__);
#else
#define NLOGI(fmt, ...) printf(fmt, __VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGW(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGW(fmt, ...) printf(fmt, __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGW(fmt, ...) print(fg(color::yellow),fmt, __VA_ARGS__);
#else
#define NLOGW(fmt, ...) printf(fmt, __VA_ARGS__);
#endif
#endif

#if defined(ANDROID) || defined(_ANDROID_)
#define NLOGE(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#elif defined(_WIN32) || defined(_WIN64)
#define NLOGE(fmt, ...) printf(fmt, __VA_ARGS__);
#elif defined(__linux__) || defined(__unix__)
#ifdef FMT_OUTPUT
#define NLOGE(fmt, ...) print(fg(color::red),fmt, __VA_ARGS__);
#else
#define NLOGE(fmt, ...) printf(fmt, __VA_ARGS__);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
