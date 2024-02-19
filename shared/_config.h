#pragma once

#define BS_UTILS_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define BS_UTILS_EXPORT_FUNC extern "C" BS_UTILS_EXPORT
#else
#define BS_UTILS_EXPORT_FUNC BS_UTILS_EXPORT
#endif
