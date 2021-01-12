#include "utils-logging.hpp"

Logger& getLogger() {
    static auto logger = new Logger(ModInfo{ID, VERSION}, LoggerOptions(false, true));
    return *logger;
}
