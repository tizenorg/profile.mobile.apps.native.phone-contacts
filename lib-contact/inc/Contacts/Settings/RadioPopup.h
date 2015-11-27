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

#ifndef CONTACTS_SETTINGS_RADIO_POPUP_H
#define CONTACTS_SETTINGS_RADIO_POPUP_H

#include "Ui/Popup.h"
#include <string>

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief Radio popup
		 */
		class RadioPopup : public Ui::Popup
		{
		public:

			/**
			 * @brief Item changed state callback
			 * @param[in]   Changed state result
			 */
			typedef std::function<void(int)> SelectedCallback;

			/**
			 * @brief Create Popup
			 * @param[in]   callback      Selected callback
			 */
			RadioPopup(SelectedCallback callback = nullptr);

			virtual ~RadioPopup() override;

			/**
			 * @brief Add new item
			 * @param[in]   text      Item name
			 * @param[in]   value     Item type
			 */
			void addItem(std::string text, int value);

			/**
			 * @brief Set selected item
			 * @param[in]   value     Selected item type
			 */
			void setSelectedItem(int value);

		private:

			virtual void onCreated() override;
			Evas_Object *createGenlist(Evas_Object *parent);
			Elm_Genlist_Item_Class *createItemClass();

			static void onSelect(void *data, Evas_Object *obj, void *event_info);
			static char *getItemText(void *data, Evas_Object *obj, const char *part);
			static Evas_Object *getItemContent(void *data, Evas_Object *obj, const char *part);
			static void delItem(void *data, Evas_Object *obj);

			SelectedCallback m_Callback;
			Evas_Object *m_Genlist;
			Elm_Genlist_Item_Class *m_ItemClass;
			Evas_Object *m_RadioGroup;
		};
	}
}
#endif /* CONTACTS_SETTINGS_RADIO_POPUP_H */
