#include "utils-logging.hpp"

Logger& getLogger() {
    static auto logger = new Logger({MOD_ID, VERSION, 0}, LoggerOptions(false, true));
    return *logger;
}
