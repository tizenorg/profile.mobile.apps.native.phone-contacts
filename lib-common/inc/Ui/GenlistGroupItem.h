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

#ifndef UI_GENLIST_GROUP_ITEM_H
#define UI_GENLIST_GROUP_ITEM_H

#include "Ui/Genlist.h"
#include "Ui/GenlistItem.h"
#include "Ui/GenlistIterator.h"
#include <vector>

namespace Ui
{
	class EXPORT_API GenlistGroupItem : virtual public GenlistItem
	{
	public:
		GenlistGroupItem();
		virtual ~GenlistGroupItem() override;

		/**
		 * @return Group begin iterator.
		 */
		GenlistIterator begin();

		/**
		 * @return Group end iterator.
		 */
		GenlistIterator end();

		/**
		 * @return Whether the group has no items (i.e. GroupItem has no subitems).
		 */
		bool empty() const;

		/**
		 * @return Whether subitems are shown.
		 */
		bool isExpanded() const;

		/**
		 * @return Group item of the next group or nullptr if none.
		 */
		GenlistGroupItem *getNextGroupItem() const;

		/**
		 * @return Group item of the previous group or nullptr if none.
		 */
		GenlistGroupItem *getPrevGroupItem() const;

		/**
		 * @brief Insert sub item to the group.
		 * @details If group item is not yet inserted itself, sub item is cached
		 *          and inserted into genlist when it's parent is inserted.
		 * @see Genlist::insert()
		 */
		void insertSubItem(GenlistItem *item,
				GenlistItem *sibling = nullptr,
				Genlist::Position position = Genlist::Before);

	protected:
		/**
		 * @see GenlistItem::isGroupItem()
		 */
		virtual bool isGroupItem() const override { return true; }

		/**
		 * @see GenlistItem::getType()
		 */
		virtual Elm_Genlist_Item_Type getType() const override { return ELM_GENLIST_ITEM_GROUP; }

		/**
		 * @see GenlistItem::getItemClass()
		 */
		virtual Elm_Genlist_Item_Class *getItemClass() const override;

		/**
		 * @see GenlistItem::onInserted()
		 */
		virtual void onInserted() override;

		/**
		 * @see GenlistItem::onPop()
		 */
		virtual void onPop() override;

		/**
		 * @brief Called when tree item is expanded.
		 */
		virtual void onExpanded();

		/**
		 * @brief Called when tree item is contracted.
		 */
		virtual void onContracted();

	private:
		friend class Genlist;
		friend class GenlistItem;

		void insertSubItems();
		void popSubItems();

		void onSubItemInserted(GenlistItem *item);
		void onSubItemDestroy(GenlistItem *item);

		GenlistItem *m_FirstItem;
		GenlistItem *m_LastItem;
		std::vector<GenlistItem *> m_ItemsCache;
	};
}

#endif /* UI_GENLIST_GROUP_ITEM_H */
