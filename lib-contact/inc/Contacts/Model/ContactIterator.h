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
#ifndef CONTACTS_MODEL_CONTACT_ITERATOR_H
#define CONTACTS_MODEL_CONTACT_ITERATOR_H

#include "Contacts/Model/ContactField.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Index-based iterator.
		 */
		class ContactIterator
		{
		public:
			/**
			 * @brief Increment iterator.
			 */
			ContactIterator &operator++();

			/**
			 * @brief Compare iterators for inequality.
			 */
			bool operator!=(const ContactIterator &that) const;

		protected:
			ContactIterator(const ContactField &field, int index);

			int m_Index;
			const ContactField &m_Field;
		};

		/**
		 * @brief Iterator for ContactArray.
		 */
		class ContactArrayIterator : public ContactIterator
		{
		public:
			friend class ContactArray;
			using ContactIterator::ContactIterator;

			/**
			 * @brief Get field pointed by iterator.
			 */
			ContactFieldPtr operator*() const;
		};

		/**
		 * @brief Iterator for ContactObject.
		 */
		class ContactObjectIterator : public ContactIterator
		{
		public:
			friend class ContactObject;
			using ContactIterator::ContactIterator;

			/**
			 * @brief Get field pointed by iterator.
			 */
			ContactFieldPtr operator*() const;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_ITERATOR_H */
