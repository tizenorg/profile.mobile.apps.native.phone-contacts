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

#ifndef CONTACTS_GROUPS_NAME_ITEM_H
#define CONTACTS_GROUPS_NAME_ITEM_H

#include "Ui/GenItem.h"

#include <contacts.h>

namespace Ui
{
	class Editfield;
}

namespace Contacts
{
	namespace Groups
	{
		class NameItem : public Ui::GenItem
		{
		public:
			/**
			 * @brief Called when text within the entry is changed
			 * @param[in]   true if entry is filled, false - empty
			 */
			typedef std::function<void(bool)> FilledCallback;

			/**
			 * @brief Genlist item representing group's name item
			 * @param[in]   record  Group's record
			 */
			NameItem(contacts_record_h record);

			/**
			 * @brief Set item filled callback.
			 * @param[in]   callback    Callback to be called after entry's text was changed.
			 */
			void setFilledCallback(FilledCallback callback);

			/**
			 * @return group name
			 */
			const char *getName() const;

		private:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual void onInserted() override;

			void onChanged(Evas_Object *entry, void *eventInfo);

			contacts_record_h m_Record;
			FilledCallback m_OnFilled;
		};
	}
}

#endif /* CONTACTS_GROUPS_NAME_ITEM_H */
