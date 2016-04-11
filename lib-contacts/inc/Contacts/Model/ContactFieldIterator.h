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

#ifndef CONTACTS_MODEL_CONTACT_FIELD_ITERATOR_H
#define CONTACTS_MODEL_CONTACT_FIELD_ITERATOR_H

#include "Utils/Iterator.h"

namespace Contacts
{
	namespace Model
	{
		class ContactField;
		class ContactFieldContainer;

		/**
		 * @brief Index-based iterator for ContactFieldContainer.
		 */
		class ContactFieldIterator :
			public Utils::IndexIterator<ContactFieldIterator, ContactField>
		{
		public:
			/**
			 * @brief Create iterator.
			 * @param[in]   container   Container with getField() method
			 * @param[in]   index       Index of the field pointed by iterator
			 */
			ContactFieldIterator(const ContactFieldContainer &container, int index);

			/**
			 * @brief Get field pointed by iterator.
			 */
			ContactField &operator*() const;

		private:
			const ContactFieldContainer &m_Container;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELD_ITERATOR_H */
