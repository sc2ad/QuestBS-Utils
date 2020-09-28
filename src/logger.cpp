#include "utils-logging.hpp"

const Logger& getLogger() {
    static const Logger logger = Logger(ModInfo{ID, VERSION}, LoggerOptions(false, true));
    return logger;
}