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

#ifndef CONTACTS_SETTINGS_MODEL_STORAGE_H
#define CONTACTS_SETTINGS_MODEL_STORAGE_H

#include <app_control.h>
#include <storage.h>
#include <string>
#include <vector>

namespace Contacts
{
	namespace Settings
	{
		namespace Model
		{
			enum StorageAccessType
			{
				StorageAccessRead,
				StorageAccessWrite
			};

			/**
			 * @brief Gets root or certain storage directory path.
			 * @details If you want to get root storage directory path use STORAGE_DIRECTORY_MAX for @a dirType.
			 *
			 * @param[in]   storageType The storage Type
			 * @param[in]   dirType The directory type
			 *
			 * @return The directory path
			 */
			std::string getDirectoryPath(storage_type_e storageType, storage_directory_e dirType);

			/**
			 * @brief Determines whether access is granted or not
			 * @param[in]   storageType Storage type
			 * @param[in]   accessType  Access type
			 * @return true if granted, false denied
			 */
			bool isAccessGranted(storage_type_e storageType, StorageAccessType accessType);

			/**
			 * @brief Get import result
			 * @param[in]   reply           App control reply
			 * @param[out]  contactsCount   Contacts count
			 * @return Vcard path list
			 */
			std::vector<std::string> getImportResult(app_control_h reply, size_t &contactsCount);
		}
	}
}

#endif /* CONTACTS_SETTINGS_MODEL_STORAGE_H */
