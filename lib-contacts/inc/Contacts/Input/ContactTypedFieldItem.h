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

#ifndef CONTACTS_INPUT_CONTACT_TYPED_FIELD_ITEM_H
#define CONTACTS_INPUT_CONTACT_TYPED_FIELD_ITEM_H

#include "Contacts/Input/ContactFieldItem.h"

namespace Contacts
{
	namespace Model
	{
		class ContactTypedObject;
		class ContactEnumField;
		class ContactTextField;
	}

	namespace Input
	{
		/**
		 * @brief Genlist item representing ContactTypedObject and its first field,
		 *        type field and label field.
		 */
		class ContactTypedFieldItem : public ContactFieldItem
		{
		public:
			/**
			 * @see ContactFieldItem::ContactFieldItem()
			 */
			ContactTypedFieldItem(Model::ContactObject &object);

		protected:
			/**
			 * @return ContactTypedObject associated with the item.
			 */
			const Model::ContactTypedObject &getTypedObject() const;

			/**
			 * @see GenItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenItem::getState()
			 */
			virtual Eina_Bool getState(Evas_Object *parent, const char *part) override;

		private:
			Model::ContactEnumField &m_TypeField;
			Model::ContactTextField &m_LabelField;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_TYPED_FIELD_ITEM_H */
