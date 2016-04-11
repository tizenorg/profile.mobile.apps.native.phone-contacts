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

#ifndef CONTACTS_LIST_CONTACT_SEARCH_ITEM_H
#define CONTACTS_LIST_CONTACT_SEARCH_ITEM_H

#include "Ui/GenlistItem.h"
#include "Ui/Editfield.h"

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Contact search item
		 */
		class ContactSearchItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Item changed/done callback
			 */
			typedef std::function<void()> ChangedCallback;

			/**
			 * @brief Contact search item
			 */
			explicit ContactSearchItem();

			/**
			 * @brief Set entry changed callback
			 */
			void setEntryChangedCb(ChangedCallback callback);

			/**
			 * @brief Set entry done button callback
			 */
			void setDonePressedCb(ChangedCallback callback);

		protected:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

		private:
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			void onDonePressed(Evas_Object *entry, void *eventInfo);
			void onChangedPressed(Evas_Object *entry, void *eventInfo);

			Ui::Editfield *m_SearchField;

			ChangedCallback m_onChanged;
			ChangedCallback m_onDone;
		};
	}
}

#endif /* CONTACTS_LIST_CONTACT_SEARCH_ITEM_H */
