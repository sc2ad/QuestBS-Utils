#include "utils-logging.hpp"

Logger& getLogger() {
    static Logger logger = Logger(ModInfo{ID, VERSION}, LoggerOptions(false, true));
    return logger;
}
