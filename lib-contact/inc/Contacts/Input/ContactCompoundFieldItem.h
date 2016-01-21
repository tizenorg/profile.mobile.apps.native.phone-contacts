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

#ifndef CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_ITEM_H
#define CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_ITEM_H

#include "Contacts/Input/ContactFieldItem.h"

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief Genlist item representing ContactCompoundObject.
		 */
		class ContactCompoundFieldItem : public ContactFieldItem
		{
		public:
			/**
			 * @see ContactFieldItem::ContactFieldItem()
			 */
			ContactCompoundFieldItem(Model::ContactObject &object);

		protected:
			/**
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @brief Disables auto-expansion.
			 * @see ContactFieldItem::onInserted()
			 */
			virtual void onInserted() override { }

			/**
			 * @see GenlistItem::onExpanded()
			 */
			virtual void onExpanded() override;

			/**
			 * @see GenlistItem::onContracted()
			 */
			virtual void onContracted() override;

		private:
			Evas_Object *createExpandButton(Evas_Object *parent);
			void updateExpandButton();
			void onExpandPressed(Evas_Object *button, void *eventInfo);

			Evas_Object *m_ExpandButton;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_ITEM_H */
