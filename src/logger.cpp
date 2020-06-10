#include "utils-logging.hpp"

const Logger& getLogger() {
    static const Logger& logger = Logger("Utils|v" VERSION);
    return logger;
}