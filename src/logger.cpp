#include "utils-logging.hpp"

const Logger& getLogger() {
    static const Logger& logger = Logger(ID "|v" VERSION);
    return logger;
}