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

#ifndef _Ct_REGISTRY_MGR_H_
#define _Ct_REGISTRY_MGR_H_


#include <string>
#include <map>

#include "WDefine.h"

typedef enum {
	REGISTRY_KEY_FAVORITE_UPDATED = 0, /**< Registry key favorite updated */
	REGISTRY_KEY_INSERTED_PERSON_ID, /**< Registry key inserted person Id */
	REGISTRY_KEY_DELETED_PERSON_ID, /**< Registry key deleted person Id */
	REGISTRY_KEY_INSERTED_PERSON, /**< Registry key inserted person */
	REGISTRY_KEY_DELETED_PERSON, /**< Registry key deleted person */
	REGISTRY_KEY_MFC_UPDATED, /**< Registry key MFC updated */
} ctRegistryKey;

typedef enum {
	REGISTRY_SUCCESS = 0, /**< Success registered */
	REGISTRY_NO_DATA, /**< Nothing to register */
	REGISTRY_FAIL, /**< Fail registered */
} ctRegistryErr;

/**
 * @brief Provides methods to manage values by keys.
 */
class WAPP_ASSIST_EXPORT CtRegistryMgr
{
public:

	/**
	 * @brief Gets instance of CtRegistryMgr class
	 * @return instance of CtRegistryMgr class
	 */
	static CtRegistryMgr* getInstance();

	/**
	 * @brief Destroy instance of CtRegistryMgr class
	 */
	void destroy();

	/**
	 * @brief Sets value to the map by key
	 * @param[in]   key     Key by witch value to be registered
	 * @param[in]   value   Value to be registered
	 * @return REGISTRY_SUCCESS on success, otherwise error ctRegistryErr
	 *
	 * @see ctRegistryKey
	 * @see ctRegistryErr
	 */
	ctRegistryErr setInt(ctRegistryKey key, int value);

	/**
	 * @brief Gets value from the map by key
	 * @param[in]   key     Key by witch value to be get
	 * @param[out]   value   Value to be get
	 * @return REGISTRY_SUCCESS on success, otherwise error ctRegistryErr
	 *
	 * @see ctRegistryKey
	 * @see ctRegistryErr
	 */
	ctRegistryErr getInt(ctRegistryKey key, int *value);

private:
	typedef struct {
		int intValue;
		std::string strValue;
	} CtRegistryData;


private:
	CtRegistryMgr();
	virtual ~CtRegistryMgr();

private:
	static CtRegistryMgr* __instance;
	std::map<ctRegistryKey, CtRegistryData*> __registry;

};

#endif /* _Ct_REGISTRY_MGR_H_ */
