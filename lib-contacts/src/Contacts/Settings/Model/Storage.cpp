/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Settings/Model/Storage.h"
#include "Utils/Logger.h"
#include <contacts.h>

using namespace Contacts;
using namespace Contacts::Settings;

namespace
{
	struct StorageData
	{
		storage_type_e m_Type;
		int m_StorageId;
	};

	bool storageCb(int storageId, storage_type_e type, storage_state_e state, const char *path, void *userData)
	{
		RETVM_IF(!userData, false, "Empty user data");
		StorageData *storageData = static_cast<StorageData *>(userData);

		if (type == storageData->m_Type && STORAGE_STATE_MOUNTED == state) {
			storageData->m_StorageId = storageId;
			return false;
		}
		return true;
	}

	int getStorageId(storage_type_e storageType)
	{
		StorageData storageData {storageType, -1};
		int error = storage_foreach_device_supported(storageCb, &storageData);
		RETVM_IF_ERR(error, -1, "storage_foreach_device_supported() failed");

		return storageData.m_StorageId;
	}
}

std::string Model::getDirectoryPath(storage_type_e storageType, storage_directory_e dirType)
{
	char *dirPath = nullptr;
	int error = STORAGE_ERROR_NONE;

	if (dirType == STORAGE_DIRECTORY_MAX) {
		error = storage_get_root_directory(getStorageId(storageType), &dirPath);
	} else {
		error = storage_get_directory(getStorageId(storageType), dirType, &dirPath);
	}
	RETVM_IF_ERR(error, std::string(), "failed to get directory path");

	std::string res = dirPath;
	free(dirPath);

	return res;
}

bool Model::isAccessGranted(storage_type_e storageType, StorageAccessType accessType)
{
	storage_state_e state = STORAGE_STATE_REMOVED;
	storage_get_state(getStorageId(storageType), &state);

	if (accessType == StorageAccessRead) {
		return state == STORAGE_STATE_MOUNTED_READ_ONLY || state == STORAGE_STATE_MOUNTED;
	}

	return state == STORAGE_STATE_MOUNTED;
}

std::vector<std::string> Model::getImportResult(app_control_h reply, size_t &contactsCount)
{
	int err = 0;
	int count = 0;
	char **array = nullptr;
	std::vector<std::string> paths;

	app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &array, &count);
	paths.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		int count = 0;
		err = contacts_vcard_get_entity_count(array[i], &count);
		WARN_IF_ERR(err, "contacts_vcard_get_entity_count() failed.");

		contactsCount += count;
		paths.push_back(array[i]);
		free(array[i]);
	}
	free(array);

	return paths;
}
