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

#ifndef CONTACTS_SETTINGS_NAME_FORMAT_POPUP_H
#define CONTACTS_SETTINGS_NAME_FORMAT_POPUP_H

#include "Ui/Popup.h"
#include <contacts_setting.h>

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief Name format popup
		 */
		class NameFormatPopup : public Ui::Popup
		{
		public:
			/**
			* @brief Create name format popup.
			*/
			NameFormatPopup();

		private:
			virtual void onCreated() override;
			Evas_Object *createGenlist(Evas_Object *parent);
			static void onSelect(void *data, Evas_Object *obj, void *event_info);
			Elm_Genlist_Item_Class *createItemClass();
			static Evas_Object *getItemContent(void *data, Evas_Object *obj, const char *part);
			static char *getItemText(void *data, Evas_Object *obj, const char *part);

			Evas_Object *m_RadioGroup;
		};
	}
}

#endif /* CONTACTS_SETTINGS_NAME_FORMAT_POPUP_H */
