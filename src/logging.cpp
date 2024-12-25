#include "logging.h"

#ifdef LOG_DISABLE_DEBUG
void logDebug(std::string message) { };
#else
void logDebug(std::string message) {
    std::cout << "[DEBUG]   " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_INFO
void logInfo(std::string message) { };
#else
void logInfo(std::string message) {
    std::cout << "[INFO]    " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_CRITICAL
void logCritical(std::string message) { };
#else
void logCritical(std::string message) {
    std::cout << "[CRITICAL] " << message << std::endl;
}
#endif

#ifdef LOG_DISABLE_WARNING
void logWarning(std::string message) { };
#else
void logWarning(std::string message) {
    std::cout << "[WARNING]  " << message << std::endl;
}
#endif