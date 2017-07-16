#ifndef OS_LABS_ERROR_H_
#define OS_LABS_ERROR_H_

void log_error_errno(const char* what,
                     const char* file,
                     int line,
                     const char* func);
void log_error_msg(const char* file,
                   int line,
                   const char* func,
                   const char* msg,
                   int exit_code);

enum {
    ERRNO_NOT_SET = 512
};

#define LOG_ERROR_ERRNO(msg) \
    (log_error_errno((msg), __FILE__, __LINE__, __func__))
#define LOG_ERROR_MSG(msg, exit_code) \
    (log_error_msg(__FILE__, __LINE__, __func__, (msg), (exit_code)))

#endif /* OS_LABS_ERROR_H_ */
