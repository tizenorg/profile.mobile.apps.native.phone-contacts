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

#ifndef _CONTACTS_DEBUG_H_
#define _CONTACTS_DEBUG_H_

#include <Elementary.h>
#include <assert.h>
#include <glib.h>


#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "PHONE"

#include "WDebugBase.h"

#if !defined(PACKAGE)
#  define PACKAGE "contacts"
#endif


#define PHONE_PKG		"org.tizen.phone"
#define CONTACTS_PKG	"org.tizen.contacts"

// Add additional your own debug MACRO

#define WPRDT_VM(expr, val, fmt, arg...) do { \
	if (expr) { \
		WDEBUG(fmt, ##arg); \
		return (val); \
	} \
} while (0)

#define WPRDT_M(expr, fmt, arg...) do { \
	if (expr) { \
		WDEBUG(fmt, ##arg); \
		return; \
	} \
} while (0)

#define WPWARN(expr, fmt, arg...) do { \
	if (expr) { \
		WWARNING(fmt, ##arg); \
	} \
} while (0)

#define SAFE_STRDUP(src) (src)?strdup(src):NULL
#define SAFE_STRLEN(src) ((src)?strlen(src):0)
#define SAFE_STRCMP(src1, str2) ((src1)?strcmp(src1, str2):-1)


#define FREEandSTRDUP(dest, src) \
	do{ \
		if(dest) {free(dest); dest=NULL;}\
		if (src) dest = strdup(src); \
	} while (false)

#define SAFE_FREE(src) \
	do{ \
		if(src) { free(src); src=NULL; }\
	}while(false)


// Usage Logging log
//phone (dialer/logs)
#define LOGGING_LOG_MAKE_A_CALL "0001" 	//Phone/Call Button
#define LOGGING_LOG_MAKE_A_VIDEO_CALL "0002"	//Phone/VideoCall Button
#define LOGGING_LOG_ADD_TO_CONTACTS "0005"	//Phone/"+ Add to contacts" Button
#define LOGGING_LOG_SPEED_DIAL_SETTINGS "0006"	//Phone/Option/Speed dial setting
#define LOGGING_LOG_CALL_SETTINGS "0007"	//Phone/Option/Call settings
#define LOGGING_LOG_VIEW_LOGS "0008"	//Phone/Logs
#define LOGGING_LOG_VIEW_LOGS_BY "0011"	//Phone/Logs/Option/View
#define LOGGING_LOG_DETAIL_LOG "0014"	//Phone/Logs/select a item

//contacts
#define LOGGING_LOG_CONTACT_APP "0016"
#define LOGGING_LOG_ADD_TO_FAVOR "0018"
#define LOGGING_LOG_REMOVE_FROM_FAVOR "0019"
#define LOGGING_LOG_LINK "0026"
#define LOGGING_LOG_PHONETIC_NAME "0041"

#endif /* _CT_DEBUG_H_ */
