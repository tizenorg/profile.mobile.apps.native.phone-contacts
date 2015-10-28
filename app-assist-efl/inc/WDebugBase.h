/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _WDEBUG_H_
#define _WDEBUG_H_

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "PHONE"

#include <dlog.h>
#include <string.h>
#include <assert.h>

#define __MODULE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// normal log
#define WLOG(prio, fmt, arg...) dlog_print(prio, LOG_TAG, "%s: %s(%d) > " fmt, __MODULE__, __func__, __LINE__, ##arg)
#define WDEBUG(fmt, args...) WLOG(DLOG_DEBUG, fmt, ##args)
#define WVERBOSE(fmt, args...) WLOG(DLOG_VERBOSE, fmt, ##args)
#define WINFO(fmt, args...) WLOG(DLOG_INFO, fmt, ##args)
#define WWARNING(fmt, args...) WLOG(DLOG_WARN, fmt, ##args)
#define WERROR(fmt, args...) WLOG(DLOG_ERROR, fmt, ##args)

#define WHIT() WDEBUG(">>>HIT<<<")
#define WHITM(fmt, args... ) WDEBUG(">>>HIT<<<" fmt, ##args)

#define WENTER() WDEBUG("BEGIN >>>>")
#define WLEAVE() WDEBUG("END <<<<")

#define WPRET_VM(expr, val, fmt, arg...) \
	if (expr) { \
		WERROR(fmt, ##arg); \
		return (val); \
	}

#define WPRET_M(expr, fmt, arg...) \
	if (expr) { \
		WERROR(fmt, ##arg); \
		return; \
	}

#define WASSERT(expr) \
	if(!(expr)) { \
		WERROR("WASSERT!"); \
		assert((expr)); \
	}

#define WASSERT_EX(expr, fmt, args... ) \
	if(!(expr)) { \
		WERROR("WASSERT! " fmt, ##args); \
		assert((expr)); \
	}

#endif /* _WDEBUG_H_ */
