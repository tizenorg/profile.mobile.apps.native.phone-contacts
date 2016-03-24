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

#ifndef CONTACTS_SETTINGS_IMPORT_CONTROLLER_H
#define CONTACTS_SETTINGS_IMPORT_CONTROLLER_H

#include <contacts.h>
#include <string>
#include <vector>

#include "Ui/ProgressController.h"

namespace Ui
{
	class Popup;
}

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief Controller for Import ProgressPopup
		 */
		class EXPORT_API ImportController : public Ui::ProgressController
		{
		public:
			/**
			 * @brief Array of contact records
			 */
			typedef std::vector<contacts_record_h> Records;

			/**
			 * @brief Array of vcard paths
			 */
			typedef std::vector<std::string> Vcards;

			/**
			 * @brief Create controller for Import ProgressPopup.
			 * @param[in]   parent          Parent object.
			 * @param[in]   title           Progress popup title.
			 * @param[in]   totalCount      Total count of contacts that will be imported.
			 * @param[in]   vcards          Vcards from which contacts will be imported.
			 * @param[in]   records         Contact records which will be imported. If it is not filled, contacts will be imported directly from vcard.
			 */
			ImportController(Evas_Object *parent, const char *title,
					size_t totalCount, Vcards vcards, Records records = {});

			/**
			 * @return Total count of imported contacts.
			 */
			size_t getTotalCount() const;

		private:
			/**
			 * @brief Private destructor. Only parent class ProgressController should call it.
			 */
			virtual ~ImportController() override;

			void createCancelPopup(Evas_Object *parent);
			bool onPopupContinue();
			bool onPopupCancel();

			virtual void onStart() override;
			virtual void onCanceled() override;
			virtual bool onCancel() override;

			static bool onVcardParse(contacts_record_h record, void *data);

		private:
			Ui::Popup *m_CancelPopup;
			std::condition_variable m_ContinueCondition;
			std::string m_CurrentVcard;
			std::vector<int> m_ImportedContacts;
			bool m_IsPaused;
			Records m_Records;
			Vcards m_Vcards;
		};
	}
}

#endif /* CONTACTS_SETTINGS_IMPORT_CONTROLLER_H */
