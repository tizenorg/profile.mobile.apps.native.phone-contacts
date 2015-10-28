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

#ifndef _WDEFINE_H_
#define _WDEFINE_H_

/**
* @file	WDefine.h
* @brief	This file defines version number MACRO and functions.
*
*/

#define WAPP_ASSIST_VERSION_MAJOR	1
#define WAPP_ASSIST_VERSION_MINOR	3
#define WAPP_ASSIST_VERSION_PATCH	2

#define WAPP_ASSIST_EXPORT __attribute__((visibility("default")))

WAPP_ASSIST_EXPORT int WAppAssist_getMajorVersion();
WAPP_ASSIST_EXPORT int WAppAssist_getMinorVersion();
WAPP_ASSIST_EXPORT int WAppAssist_getPatchVersion();
WAPP_ASSIST_EXPORT const char* WAppAssist_getVersion();

#define WDISABLE_COPY_AND_ASSIGN( A ) \
		A( const A& ) = delete; \
		const A& operator=( const A& ) = delete

#define WDECLARE_PRIVATE_IMPL( A ) \
	class __##A##Impl* __pv; \
	friend class __##A##Impl


#endif /* _WDEFINE_H_ */
