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

#ifndef CONTACTS_SETTINGS_UTILS_H
#define CONTACTS_SETTINGS_UTILS_H

#include <storage.h>
#include <string>

namespace Contacts
{
	namespace Common
	{
		/**
		 * @brief Type of storage where vcard will be created.
		 */
		enum TargetStorage
		{
			TargetStorageDevice,
			TargetStorageSdCard,
			TargetStorageInternalOther
		};

		/**
		 * @brief Gets root storage directory path
		 * @param[in]   storageType The storage Type
		 *
		 * @return The directory path
		 */
		std::string getRootDirectoryPath(storage_type_e storageType);

		/**
		 * @brief Gets certain storage directory path
		 * @param[in]   storageType The storage Type
		 * @param[in]   dirType The directory type
		 *
		 * @return The directory path
		 */
		std::string getDirectoryPath(storage_type_e storageType, storage_directory_e dirType);
	}
}

#endif /* CONTACTS_SETTINGS_UTILS_H */
