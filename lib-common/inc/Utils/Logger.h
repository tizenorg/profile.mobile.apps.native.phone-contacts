/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#define LOG_TAG "contacts"
#include <dlog.h>

#define __MODULE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#include "Utils/Tracer.h"
#define TRACE Utils::Tracer tracer(LOG_TAG, __MODULE__, __func__, __LINE__)

#define DLOG(prio, fmt, arg...) dlog_print(prio, LOG_TAG, "%s: %s(%d) > " fmt, __MODULE__, __func__, __LINE__, ##arg)
#define DBG(fmt, arg...) DLOG(DLOG_DEBUG, fmt, ##arg)
#define ERR(fmt, arg...) DLOG(DLOG_ERROR, fmt, ##arg)

#define WARN_IF(expr, fmt, arg...) \
if (expr) { \
	ERR(fmt, ##arg); \
} \

#define RETM_IF(expr, fmt, arg...) \
if (expr) { \
	ERR(fmt, ##arg); \
	return; \
} \

#define RETVM_IF(expr, val, fmt, arg...) \
if (expr) { \
	ERR(fmt, ##arg); \
	return (val); \
} \

#endif /* UTILS_LOGGER_H */
