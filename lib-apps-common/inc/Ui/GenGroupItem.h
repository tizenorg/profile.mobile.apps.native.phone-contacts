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

#ifndef UI_GEN_GROUP_ITEM_H
#define UI_GEN_GROUP_ITEM_H

#include "Ui/GenContainer.h"
#include "Ui/GenItem.h"

#include <string>
#include <vector>

namespace Ui
{
	class EXPORT_API GenGroupItem : virtual public GenItem
	{
	public:
		/**
		 * @brief Create group item.
		 * @param[in]   title   Item title (can be translatable)
		 */
		explicit GenGroupItem(const char *title = nullptr);
		virtual ~GenGroupItem() override;

		/**
		 * @return Group first child Item.
		 */
		GenItem *getFirstItem() const;

		/**
		 * @return Group last child Item.
		 */
		GenItem *getLastItem() const;

		/**
		 * @return Group begin iterator.
		 */
		GenIterator begin();

		/**
		 * @return Group end iterator.
		 */
		GenIterator end();

		/**
		 * @return Child items count.
		 */
		size_t getItemCount() const;

		/**
		 * @return Whether the group has no items (i.e. GroupItem has no subitems).
		 */
		bool isEmpty() const;

		/**
		 * @return Whether subitems are shown.
		 */
		bool isExpanded() const;

		/**
		 * @return Group item of the next group or nullptr if none.
		 */
		GenGroupItem *getNextGroupItem() const;

		/**
		 * @return Group item of the previous group or nullptr if none.
		 */
		GenGroupItem *getPrevGroupItem() const;

		/**
		 * @brief Insert sub item to the group.
		 * @details If group item is not yet inserted itself, sub item is cached
		 *          and inserted into container when it's parent is inserted.
		 * @see GenContainer::insert()
		 */
		void insertSubItem(GenItem *item, GenItem *sibling = nullptr,
				GenContainer::Position position = GenContainer::Before);

	protected:
		/**
		 * @see GenItem::isGroupItem()
		 */
		virtual bool isGroupItem() const override { return true; }

		/**
		 * @see GenItem::getType()
		 */
		virtual Elm_Genlist_Item_Type getType() const override { return ELM_GENLIST_ITEM_GROUP; }

		/**
		 * @see GenItem::getItemClass()
		 */
		virtual Elm_Gen_Item_Class *getItemClass() const override;

		/**
		 * @brief Sets item title into "elm.text" part.
		 * @see GenItem::getText()
		 */
		virtual char *getText(Evas_Object *parent, const char *part) override;

		/**
		 * @brief Inserts all subitems.
		 * @see GenItem::onInserted()
		 */
		virtual void onInserted() override;

		/**
		 * @brief Pops all subitems.
		 * @see GenItem::onPop()
		 */
		virtual void onPop() override;

		/**
		 * @brief Called when tree item is expanded or contracted.
		 */
		virtual void onExpanded(bool isExpanded);

	private:
		friend class Genlist;

		void insertSubItems();
		void popSubItems();

		std::string m_Title;
		std::vector<GenItemPtr> m_ItemsCache;
	};
}

#endif /* UI_GEN_GROUP_ITEM_H */
