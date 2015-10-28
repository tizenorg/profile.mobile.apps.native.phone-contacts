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

#include "CtRegistryMgr.h"

#include "ContactsDebug.h"

CtRegistryMgr* CtRegistryMgr::__instance = NULL;

CtRegistryMgr* CtRegistryMgr::getInstance()
{
	if( __instance == NULL )
		__instance = new CtRegistryMgr();
	return __instance;
}

CtRegistryMgr::CtRegistryMgr()
{
	WHIT();
}

CtRegistryMgr::~CtRegistryMgr()
{
	WHIT();
}

void CtRegistryMgr::destroy()
{
	WHIT();
	delete __instance;
	__instance = NULL;
}

ctRegistryErr CtRegistryMgr::setInt(ctRegistryKey key, int value)
{
	WDEBUG("key %d, value %d", key, value);
	std::map<ctRegistryKey, CtRegistryData*>::iterator iter;
	iter = __registry.find(key);
	if(iter != __registry.end()) {
		WDEBUG("replace as new value");
		delete (*iter).second;
		__registry.erase(iter);
	}

	CtRegistryData *registryData = new CtRegistryData;
	registryData->intValue = value;

	__registry.insert(std::make_pair(key, registryData));

	return REGISTRY_SUCCESS;
}

ctRegistryErr CtRegistryMgr::getInt(ctRegistryKey key, int *value)
{
	WDEBUG("key %d", key);

	std::map<ctRegistryKey, CtRegistryData*>::iterator iter;
	iter = __registry.find(key);
	if(iter != __registry.end()) {
		*value = (*iter).second->intValue;
		delete (*iter).second;
		__registry.erase(iter);

		return REGISTRY_SUCCESS;
	}
	else
		return REGISTRY_NO_DATA;
}
