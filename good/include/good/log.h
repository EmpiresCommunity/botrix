//----------------------------------------------------------------------------------------------------------------
// Log implementation. Log is printf() based.
// Copyright (c) 2014 Borzh.
//----------------------------------------------------------------------------------------------------------------

#ifndef __GOOD_LOG_H__
#define __GOOD_LOG_H__


// Force documenting defines.
/** @file */


#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#ifdef GOOD_MULTI_THREAD
    #include "good/mutex.h"
#endif


/// Log trace message to default log.
#define GLOG_T(...)            good::log::printf( good::ELogLevelTrace,   __VA_ARGS__ )
/// Log debug message to default log.
#define GLOG_D(...)            good::log::printf( good::ELogLevelDebug,   __VA_ARGS__ )
/// Log info message to default log.
#define GLOG_I(...)            good::log::printf( good::ELogLevelInfo,    __VA_ARGS__ )
/// Log warning message to default log.
#define GLOG_W(...)            good::log::printf( good::ELogLevelWarning, __VA_ARGS__ )
/// Log error message to default log.
#define GLOG_E(...)            good::log::printf( good::ELogLevelError,   __VA_ARGS__ )


#ifndef GOOD_LOG_MAX_MSG_SIZE
    /// Maximum size of one message is 4k.
    #define GOOD_LOG_MAX_MSG_SIZE  4*1024
#endif


namespace good
{


    /// Enum for log levels.
    enum TLogLevelId
    {
        ELogLevelOff = 0,              ///< No logging.
        ELogLevelTrace,                ///< Trace log level.
        ELogLevelDebug,                ///< Debug log level.
        ELogLevelInfo,                 ///< Info log level.
        ELogLevelWarning,              ///< Warning log level.
        ELogLevelError,                ///< Error log level.
        ELogLevelTotal                 ///< Amount of log levels.
    };

    typedef unsigned int TLogLevel;    ///< Typedef for log level.


    //************************************************************************************************************
    /**
     * @brief Class used for logging to stdout, stderr and/or files.
     *
     *
     */
    //************************************************************************************************************
    class log
    {
    public:

        static bool bLogToStdOut;       ///< Log all but error messages to stdout (true by default).
        static bool bLogToStdErr;       ///< Log error messages to stderr (true by default).

        static TLogLevel iLogLevel;     ///< Log level.
        static TLogLevel iFileLogLevel; ///< File log level.
        static TLogLevel iStdErrLevel;  ///< Minimum level to log to stderr (ELogLevelError by default).

        /**
         * @brief Set log level.
         * @param iLevel level to log.
         */
        static void set_level( TLogLevel iLevel );

        /**
         * @brief Set log level.
         * @param iLevel level to log.
         */
        static TLogLevel get_level() { return m_iLogLevel; }

        /**
         * @brief Set log prefix.
         *
         * If log prefix is set, then each log message will first print that prefix.
         *
         * You can use next predefined strings in there:
         * @li %L log level.
         * @li %l log level letter.
         * @li %D date.
         * @li %T time.
         * @li %t thread id.
         * @li %p process id.
         *
         * By default log prefix is empty.
         *
         * @param szPrefix string to print before each log message.
         * @return false size of @p szPrefix is greater than GOOD_LOG_MAX_MSG_SIZE.
         */
        static bool set_prefix( const char* szPrefix );

        /**
         * @brief Set log file.
         * @param szFile file where save each log message.
         * @param bAppend if true, the file's contents will be preserved.
         * @return false if file can't be created.
         */
        static bool start_log_to_file( const char* szFile, bool bAppend = false );

        /**
         * @brief Stop logging to file.
         */
        static void stop_log_to_file();

        /**
         * @brief Copy message to @p szOutput.
         * @param iLevel level of message.
         * @param bLog if false, will only print to ouput buffer.
         * @param szOutput buffer where to save output. Can be NULL.
         * @param iOutputSize size of output buffer.
         * @param szFmt format string.
         * @return size of characters written.
         */
        static int format( TLogLevel iLevel, char* szOutput, int iOutputSize, const char* szFmt, ... )
            FORMAT_FUNCTION(4, 5);

        /**
         * @brief Log with given level.
         * @param iLevel level of message.
         * @param szFmt format string.
         */
        static int printf( TLogLevel iLevel, const char* szFmt, ... )
            FORMAT_FUNCTION(2, 3);

        /**
         * @brief Log aready formatted string with given level.
         *
         * Prefix is not used in output. To use after call format().
         * @param iLevel level of message.
         * @param szFinal final string to log.
         */
        static void print( TLogLevel iLevel, const char* szFinal );


    protected:
        static int format_va_list( char* szOutput, int iOutputSize, const char* szFmt, va_list argptr );

        static TLogLevel m_iLogLevel; ///< Log level.
        static FILE* m_fLog; ///< Opened file descriptor where to do the logging.

        enum TLogFormatId
        {
            ELogFormatDate = 0,
            ELogFormatTotal
        };

        static bool m_aLogFields[ELogFormatTotal]; ///< Log fields.

#ifdef GOOD_MULTI_THREAD
        static mutex m_cMutex; ///< Mutex to synchronize access.
#endif
    };


} // namespace good


#endif // __GOOD_LOG_H__
