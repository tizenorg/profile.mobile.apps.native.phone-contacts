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

#ifndef CONTACTS_INPUT_CONTACT_GROUPS_FIELD_ITEM_H
#define CONTACTS_INPUT_CONTACT_GROUPS_FIELD_ITEM_H

#include "Contacts/Input/ContactFieldItem.h"
#include "Ux/SelectTypes.h"

namespace Contacts
{
	namespace Model
	{
		class ContactArray;
	}

	namespace Input
	{
		/**
		 * @brief Genlist item representing FieldGroups field.
		 */
		class ContactGroupsFieldItem : public ContactFieldItem
		{
		public:
			/**
			 * @see ContactFieldItem::ContactFieldItem
			 */
			ContactGroupsFieldItem(Model::ContactObject &object);

			/**
			 * @see GenItem::isFocusable()
			 */
			virtual bool isFocusable() const override;

			/**
			 * @see ContactFieldSubItem::update()
			 */
			virtual void update() override;

		private:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			virtual void onInserted() override;
			virtual void onSelected() override;
			bool onGroupsSelected(Ux::SelectResults results);

			std::string formatGroups() const;

			Model::ContactArray &m_GroupRels;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_GROUPS_FIELD_ITEM_H */
