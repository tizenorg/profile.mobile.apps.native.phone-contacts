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

#ifndef CONTACTS_INPUT_CONTACT_FIELD_ITEM_H
#define CONTACTS_INPUT_CONTACT_FIELD_ITEM_H

#include "Contacts/Model/ContactField.h"
#include "Ui/GenlistItem.h"

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief Genlist item representing contact field of TypeText or TypeDate type.
		 */
		class ContactFieldItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Create field genlist item.
			 * @param[in]   field   Contact field of TypeText or TypeDate type
			 * @param[in]   type    Genlist item type
			 */
			ContactFieldItem(Model::ContactFieldPtr field,
					Elm_Genlist_Item_Type type = ELM_GENLIST_ITEM_NONE);

		protected:
			/**
			 * @return ContactField associated with the item.
			 */
			template <typename FieldType>
			FieldType *getField() const;

			/**
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		private:
			Model::ContactFieldPtr m_Field;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_FIELD_ITEM_H */
