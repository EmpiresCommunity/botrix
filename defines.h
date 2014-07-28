#ifndef __BOTRIX_DEFINES_H__
#define __BOTRIX_DEFINES_H__


#include <good/log.h>

#include "source_engine.h"


// Define for logging.
#define BLOG(user, level, ...)\
    do {\
        extern int iLogBufferSize;\
        extern char* szLogBuffer;\
        good::log::format(level, szLogBuffer, iLogBufferSize, __VA_ARGS__);\
        good::log::print(level, szLogBuffer);\
        CUtil::Message(user, szLogBuffer);\
    } while (false)

// Botrix log with level. Log to stdout/stderr and server.
#define BLOG_T(...)        BLOG(NULL, good::ELogLevelTrace, __VA_ARGS__)
#define BLOG_D(...)        BLOG(NULL, good::ELogLevelDebug, __VA_ARGS__)
#define BLOG_I(...)        BLOG(NULL, good::ELogLevelInfo, __VA_ARGS__)
#define BLOG_W(...)        BLOG(NULL, good::ELogLevelWarning, __VA_ARGS__)
#define BLOG_E(...)        BLOG(NULL, good::ELogLevelError, __VA_ARGS__)

// Log to stdout and user.
#define BULOG_T(user, ...) BLOG(user, good::ELogLevelTrace, __VA_ARGS__)
#define BULOG_D(user, ...) BLOG(user, good::ELogLevelDebug, __VA_ARGS__)
#define BULOG_I(user, ...) BLOG(user, good::ELogLevelInfo, __VA_ARGS__)
#define BULOG_W(user, ...) BLOG(user, good::ELogLevelWarning, __VA_ARGS__)
#define BULOG_E(user, ...) BLOG(user, good::ELogLevelError, __VA_ARGS__)

// Assertion.
#define BASSERT(exp, ...)\
    do {\
        if ( !(exp) )\
        {\
            BLOG_E("Assert failed: (%s); in %s(), file %s, line %d\n", #exp, __FUNCTION__, __FILE__, __LINE__);\
            BreakDebugger();\
            __VA_ARGS__;\
        }\
    } while (false)


#endif // __BOTRIX_DEFINES_H__
