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

#ifndef CONTACTS_SETTINGS_NAME_FORMAT_ITEM_H
#define CONTACTS_SETTINGS_NAME_FORMAT_ITEM_H

#include "Ui/GenlistItem.h"
#include <contacts_setting.h>

namespace Contacts
{
	namespace Settings
	{
		/**
		 * @brief Name format item
		 */
		class NameFormatItem : public Ui::GenlistItem
		{
		public:
			/**
			* @brief Create name format item.
			*/
			NameFormatItem();
			virtual ~NameFormatItem() override;

		private:
			virtual void onSelected() override;
			virtual char *getText(Evas_Object *parent, const char *part) override;
			static void onItemStateChanged(int type);
			static void onNameOrderChanged(contacts_name_display_order_e name_sorting_order, void *user_data);
		};
	}
}

#endif /* CONTACTS_SETTINGS_NAME_FORMAT_ITEM_H */
