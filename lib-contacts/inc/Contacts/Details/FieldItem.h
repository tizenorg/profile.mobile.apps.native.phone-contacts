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

#ifndef CONTACTS_DETAILS_FIELD_ITEM_H
#define CONTACTS_DETAILS_FIELD_ITEM_H

#include "Ux/SelectItem.h"
#include <contacts.h>

namespace Contacts
{
	namespace Model
	{
		class ContactField;
		class ContactObject;
	}

	namespace Details
	{
		/**
		 * @brief Genlist item representing ContactObject and its first field.
		 */
		class FieldItem : public Ux::SelectItem
		{
		public:
			/**
			 * @brief Create genlist item representing ContactObject.
			 * @param[in]   object  Contact field of TypeObject type
			 */
			FieldItem(Model::ContactObject &object);

			/**
			 * @return ContactField associated with the item.
			 */
			Model::ContactField &getField() const;

			/**
			 * @return ContactObject associated with the item.
			 */
			Model::ContactObject &getObject() const;

		protected:
			friend class DetailsView;

			/**
			 * @see GenItem::getItemClass()
			 */
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

			/**
			 * @see GenItem::getText()
			 */
			virtual char *getText(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see SelectItem::getDefaultResult()
			 */
			virtual Ux::SelectResult getDefaultResult() const override;

			/**
			 * @see ContactField::UpdatedCallback
			 */
			virtual void onFieldUpdated(Model::ContactField &field, contacts_changed_e change);

		private:
			Model::ContactObject &m_Object;
			Model::ContactField &m_Field;
		};
	}
}

#endif /* CONTACTS_DETAILS_FIELD_ITEM_H */
