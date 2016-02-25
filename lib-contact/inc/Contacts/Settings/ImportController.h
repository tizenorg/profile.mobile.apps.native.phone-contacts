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
		class ImportController : public Ui::ProgressController
		{
		public:
			/**
			 * @brief Create controller for Import ProgressPopup.
			 * @param[in]   parent          Parent object.
			 * @param[in]   title           Progress popup title.
			 * @param[in]   totalCount      Total count of contacts that will be imported.
			 * @param[in]   vcards          Vcards which will be imported.
			 */
			ImportController(Evas_Object *parent, const char *title,
					size_t totalCount, std::vector<std::string> vcards);

		private:
			/**
			 * @brief Private destructor. Only parent class ProgressController should call it.
			 */
			virtual ~ImportController() override;

			void createCancelPopup(Evas_Object *parent);
			bool onPopupContinue();
			bool onPopupCancel();

			virtual void onStart() override;
			virtual void onFinish() override;
			virtual void onCanceled() override;
			virtual bool onCancel() override;

			static bool onVcardParse(contacts_record_h record, void *data);

		private:
			Ui::Popup *m_CancelPopup;
			std::condition_variable m_ContinueCondition;
			std::string m_CurrentVcard;
			std::vector<int> m_ImportedContacts;
			bool m_IsPaused;
			Ecore_Thread *m_Thread;
			std::vector<std::string> m_Vcards;
		};
	}
}

#endif /* CONTACTS_SETTINGS_IMPORT_CONTROLLER_H */
