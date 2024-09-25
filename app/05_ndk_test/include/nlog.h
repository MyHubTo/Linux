
#ifndef __NLOG_H__
#define __NLOG_H__
#include <android/log.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifndef  LOG_TAG
#define  LOG_TAG "Android_"
#endif

#ifndef NLOGV
#define NLOGV(...) do { \
                __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif

#ifndef NLOGI
#define NLOGI(...) do { \
                __android_log_print(ANDROID_LOG_INFO,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif

#ifndef NLOGD
#define NLOGD(...) do { \
                __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif

#ifndef NLOGW
#define NLOGW(...) do { \
                __android_log_print(ANDROID_LOG_WARN,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif

#ifndef NLOGE
#define NLOGE(...) do { \
                __android_log_print(ANDROID_LOG_ERROR,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif

#ifndef NLOGF
#define NLOGF(...) do { \
                __android_log_print(ANDROID_LOG_FATAL,LOG_TAG TAG,__VA_ARGS__); \
        } while (0)
#endif
#ifdef __cplusplus
}
#endif
#endif