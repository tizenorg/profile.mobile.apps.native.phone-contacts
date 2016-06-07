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

#ifndef UI_GENLIST_ITEM_H
#define UI_GENLIST_ITEM_H

#include "Ui/GenItem.h"

namespace Ui
{
	class GenlistGroupItem;

	class EXPORT_API GenlistItem : public GenItem
	{
	public:
		GenlistItem();

		/**
		 * @return Whether the item is a group item.
		 */
		virtual bool isGroupItem() const { return false; }

		/**
		 * @return Whether the item can receive focus.
		 */
		virtual bool isFocusable() const { return false; }

		/**
		 * @return Parent genlist group item or nullptr if none.
		 */
		GenlistGroupItem *getParentItem() const;

		/**
		 * @brief Scroll to the item and give it focus.
		 * @see GenlItem::scrollTo()
		 */
		void focus(Elm_Genlist_Item_Scrollto_Type position = ELM_GENLIST_ITEM_SCROLLTO_IN,
				bool isAnimated = false);

	protected:
		/**
		 * @brief Called when item is focused by calling focus().
		 */
		virtual void onFocused() { }

		/**
		 * @brief Provides deferred focus if item was not realized when focus() was called.
		 * @see GenItem::onRealized()
		 */
		virtual void onRealized();

	private:
		friend class Genlist;

		bool m_IsFocusPending;
	};
}

#endif /* UI_GENLIST_ITEM_H */
