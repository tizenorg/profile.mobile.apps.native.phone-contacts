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

#include "WDefine.h"
#include <stdio.h>

int WAppAssist_getMajorVersion() { return WAPP_ASSIST_VERSION_MAJOR; }
int WAppAssist_getMinorVersion() { return WAPP_ASSIST_VERSION_MINOR; }
int WAppAssist_getPatchVersion() { return WAPP_ASSIST_VERSION_PATCH; }
const char* WAppAssist_getVersion()
{
	static char ver[24] = {0,};
	if( ver[0] == '\0' )
		snprintf( ver, sizeof(ver), "%d.%d.%d", WAppAssist_getMajorVersion(), WAppAssist_getMinorVersion(), WAppAssist_getPatchVersion() );

	return ver;
}
