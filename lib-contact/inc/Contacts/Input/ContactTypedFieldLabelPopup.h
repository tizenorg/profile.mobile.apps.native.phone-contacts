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

#ifndef CONTACTS_INPUT_CONTACT_TYPED_FIELD_LABEL_POPUP_H
#define CONTACTS_INPUT_CONTACT_TYPED_FIELD_LABEL_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	class Editfield;
}

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief Popup for editing object's custom type label.
		 *
		 * @see ContactTypedObject
		 */
		class ContactTypedFieldLabelPopup : public Ui::Popup
		{
		public:
			/**
			 * @brief User input result callback.
			 * @param[in]   Inputed custom type label
			 */
			typedef std::function<void(const char *)> ResultCallback;

			ContactTypedFieldLabelPopup();

			/**
			 * @brief Set user input result callback.
			 * @param[in]   callback    Callback to be called when user confirmed inputed label
			 */
			void setResultCallback(ResultCallback callback);

		private:
			virtual void onCreated() override;
			void onCreatePressed();
			void onDonePressed(Evas_Object *entry, void *eventInfo);
			static void onEntryChanged(Evas_Object *button, Evas_Object *entry, void *eventInfo);

			ResultCallback m_OnResult;
			Ui::Editfield *m_Editfield;
		};

	}
}

#endif /* CONTACTS_INPUT_CONTACT_TYPED_FIELD_LABEL_POPUP_H */
