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

#ifndef CONTACTS_LIST_CONTACT_ITEM_H
#define CONTACTS_LIST_CONTACT_ITEM_H

#include "Ui/GenlistItem.h"
#include "Contacts/List/Model/Contact.h"
#include <contacts.h>

#define PART_CONTACT_NAME "elm.text"
#define PART_CONTACT_THUMBNAIL "elm.swallow.icon"

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Contacts list item
		 */
		class ContactItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Create contact item
			 * @param[in]   contact     Contact object
			 */
			ContactItem(Model::ContactPtr contact);
			virtual ~ContactItem() override;

			/**
			 * @return Contact object
			 */
			const Model::ContactPtr &getContact() const;

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Model::ContactPtr m_Contact;
		};
	}
}

#endif /* CONTACTS_LIST_CONTACT_ITEM_H */
