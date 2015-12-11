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

#include "Ui/GenlistItem.h"
#include "Ui/GenlistIterator.h"
#include <vector>

namespace Ui
{
	class EXPORT_API GenlistGroupItem : public GenlistItem
	{
	public:
		/**
		 * @see GenlistItem::GenlistItem()
		 */
		GenlistGroupItem(Elm_Genlist_Item_Class *itemClass = nullptr,
				Elm_Genlist_Item_Type itemType = ELM_GENLIST_ITEM_GROUP);
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
		bool empty();

	protected:
		virtual void onExpanded() override;
		virtual void onContracted() override;

	private:
		friend class GenlistItem;

		void onSubItemInserted(GenlistItem *item);
		void onSubItemDestroy(GenlistItem *item);

		GenlistItem *m_FirstItem;
		GenlistItem *m_LastItem;
		std::vector<GenlistItem *> m_ItemsCache;
	};
}

#endif /* UI_GENLIST_GROUP_ITEM_H */
