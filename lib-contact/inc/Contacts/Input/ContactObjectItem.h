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

#ifndef CONTACTS_INPUT_CONTACT_OBJECT_ITEM_H
#define CONTACTS_INPUT_CONTACT_OBJECT_ITEM_H

#include "Contacts/Input/ContactFieldItem.h"
#include "Contacts/Model/ContactObject.h"
#include <functional>

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief Genlist item representing ContactObject and its first field.
		 */
		class ContactObjectItem : public ContactFieldItem
		{
		public:
			/**
			 * @brief Remove field callback.
			 * @param[in]   Genlist item to be removed
			 * @param[in]   Field to be removed
			 */
			typedef std::function<void(ContactObjectItem *, Model::ContactFieldPtr)> RemoveCallback;

			/**
			 * @brief Create genlist item representing ContactObject.
			 * @param[in]   object      Contact field of TypeObject type
			 */
			ContactObjectItem(Model::ContactFieldPtr object);

			/**
			 * @brief Set remove field callback.
			 * @param[in]   callback    Callback to be called when remove button is pressed
			 */
			void setRemoveCallback(RemoveCallback callback);

			/**
			 * @return ContactObject associated with the item.
			 */
			Model::ContactObject *getObject() const;

		protected:
			/**
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onInserted()
			 */
			virtual void onInserted();

			/**
			 * @brief Insert
			 */
			void insertChildItems();

		private:
			void onRemovePressed(Evas_Object *button, void *eventInfo);

			Model::ContactFieldPtr m_Object;
			RemoveCallback m_OnRemove;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_OBJECT_ITEM_H */
