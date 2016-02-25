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

#ifndef CONTACTS_SETTINGS_EXPORT_CONTROLLER_H
#define CONTACTS_SETTINGS_EXPORT_CONTROLLER_H

#include <string>
#include <vector>

#include "Ui/ProgressController.h"
#include "Contacts/Common/Utils.h"

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief Type of storage where vcard will be created.
		 */
		enum StorageType
		{
			StorageDevice,
			StorageSdCard,
			StorageInternalOther
		};

		/**
		 * @brief Controller for Export ProgressPopup
		 */
		class ExportController : public Ui::ProgressController
		{
		public:
			/**
			 * @brief Create controller for Export ProgressPopup.
			 * @param[in]   parent          Parent object.
			 * @param[in]   title           Progress popup title.
			 * @param[in]   personIdList    Person id list.
			 * @param[in]   vcardStorage    Type of storage where vcard will be created.
			 */
			ExportController(Evas_Object *parent, const char *title,
					std::vector<int> personIdList, StorageType vcardStorage);

			/**
			 * @return Vcard path.
			 */
			const std::string &getVcardPath() const;

		private:
			void createDirectory(const std::string &directoryPath);
			std::string getVcardDirectoryPath();
			const char *getVcardRelativePath() const;
			std::string getVcardFilePath();

			virtual void onStart() override;
			virtual void onFinish() override;
			virtual void onCanceled() override;

		private:
			Evas_Object *m_Parent;
			std::vector<int> m_PersonIdList;
			std::string m_VcardPath;
			StorageType m_VcardStorage;
		};
	}
}

#endif /* CONTACTS_SETTINGS_EXPORT_CONTROLLER_H */
