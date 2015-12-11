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

#ifndef UI_GENLIST_ITERATOR_H
#define UI_GENLIST_ITERATOR_H

#include <iterator>
#include <tizen.h>

namespace Ui
{
	class GenlistItem;

	/**
	 * @brief Genlist items iterator.
	 */
	class EXPORT_API GenlistIterator :
			public std::iterator<std::bidirectional_iterator_tag, GenlistItem *>
	{
	public:
		/**
		 * @brief Create genlist iterator
		 * @param[in]   item    Item pointed by iterator
		 */
		GenlistIterator(GenlistItem *item = nullptr);

		/**
		 * @brief Increment iterator.
		 */
		GenlistIterator &operator++();
		GenlistIterator operator++(int);

		/**
		 * @brief Decrement iterator.
		 */
		GenlistIterator &operator--();
		GenlistIterator operator--(int);

		/**
		 * @brief Dereference iterator.
		 */
		GenlistItem *operator*() const;

		/**
		 * @brief Compare iterators for inequality.
		 */
		bool operator!=(const GenlistIterator &that) const;

	private:
		GenlistItem *m_Item;
	};
}

#endif /* UI_GENLIST_ITERATOR_H */
