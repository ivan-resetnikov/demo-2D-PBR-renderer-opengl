#include "logging.h"

#ifdef LOG_DISABLE_DEBUG
void log_debug(std::string message) { };
#else
void log_debug(std::string message) {
    std::cout << "[DEBUG]    " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_INFO
void log_info(std::string message) { };
#else
void log_info(std::string message) {
    std::cout << "[INFO]     " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_WARNING
void log_warning(std::string message) { };
#else
void log_warning(std::string message) {
    std::cout << "[WARNING]  " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_ERROR
void log_error(std::string message) { };
#else
void log_error(std::string message) {
    std::cout << "[WARNING]  " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_CRITICAL
void log_critical(std::string message) { };
#else
void log_critical(std::string message) {
    std::cout << "[CRITICAL] " << message << std::endl;
}
#endif
