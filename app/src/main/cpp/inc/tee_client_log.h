/*
 * Copyright (c) Honor Device Co., Ltd. 2020-2020. All rights reserved.
 * Description: TEEC debug interfaces
 * Create: 2020-03-31
 */

#ifndef TEE_CLIENT_LOG_H
#define TEE_CLIENT_LOG_H

#include <android/log.h>

/**
 * @ingroup TEEC_BASIC_FUNC
 * debug interface, only used when TEEC_DEBUG
 */
#ifdef TEEC_DEBUG
#define TEEC_Debug(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define TEEC_Debug(...)
#endif

/**
 * @ingroup TEEC_BASIC_FUNC
 * debug interface
 */
#define TEEC_Error(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif
