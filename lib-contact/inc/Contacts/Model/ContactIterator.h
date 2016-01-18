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
#include <iterator>

namespace Contacts
{
	namespace Model
	{
		class ContactArray;
		class ContactObject;

		/**
		 * @brief Index-based iterator template for ContactArray and ContactObject.
		 */
		template <typename FieldContainer>
		class ContactIterator :
			public std::iterator<std::input_iterator_tag, ContactField>
		{
		public:
			/**
			 * @brief Create iterator.
			 * @param[in]   container   Container with getField() method
			 * @param[in]   index       Index of the field pointed by iterator
			 */
			ContactIterator(const FieldContainer &container, int index)
				: m_Container(container), m_Index(index) { }

			/**
			 * @brief Increment iterator.
			 */
			ContactIterator &operator++() { ++m_Index; return *this; }

			/**
			 * @brief Get field pointed by iterator.
			 */
			ContactField &operator*() const { return *m_Container.getField(m_Index); }

			/**
			 * @brief Compare iterators for inequality.
			 */
			bool operator!=(const ContactIterator &that) const { return m_Index != that.m_Index;}

		protected:
			const FieldContainer &m_Container;
			size_t m_Index;
		};

		typedef ContactIterator<ContactArray> ContactArrayIterator;
		typedef ContactIterator<ContactObject> ContactObjectIterator;
	}
}

#endif /* CONTACTS_MODEL_CONTACT_ITERATOR_H */
