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

#include <Elementary.h>
#include <tizen.h>

namespace Ui
{
	class EXPORT_API GenlistItem
	{
	public:
		/**
		 * @brief Create statically allocated item class
		 */
		static Elm_Genlist_Item_Class createItemClass(const char *style,
				const char *decorateStyle = nullptr,
				const char *editStyle = nullptr);

		/**
		 * @return Genlist object item.
		 */
		Elm_Object_Item *getObjectItem() const;

		/**
		 * @return Next item in genlist or NULL if none.
		 */
		GenlistItem *getNextItem() const;

		/**
		 * @return Previous item in genlist or NULL if none.
		 */
		GenlistItem *getPrevItem() const;

	protected:
		/**
		 * @brief Create genlist item.
		 * @param[in]   itemClass   Genlist item class
		 * @param[in]   itemType    Genlist item type
		 */
		GenlistItem(Elm_Genlist_Item_Class *itemClass = nullptr,
					Elm_Genlist_Item_Type itemType = ELM_GENLIST_ITEM_NONE);
		virtual ~GenlistItem() { }

		/**
		 * @brief Called whenever any item's text part needs to be updated.
		 * @param[in]   parent  Parent genlist
		 * @param[in]   part    Name of the part
		 * @return Allocated string with text for a specified part,
		 *         it will be deallocated with free() by genlist.
		 */
		virtual char *getText(Evas_Object *parent, const char *part) { return nullptr; }

		/**
		 * @brief Called whenever any item's content part needs to be updated.
		 * @param[in]   parent  Parent genlist
		 * @param[in]   part    Name of the part
		 * @return Content for a specified part.
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) { return nullptr; }

		/**
		 * @brief Called when item is selected.
		 */
		virtual void onSelected() { }

		/**
		 * @brief Called when tree item is expanded.
		 */
		virtual void onExpanded() { }

		/**
		 * @brief Called when tree item is contracted.
		 */
		virtual void onContracted();

		/**
		 * @brief Called when item is realized (became visible).
		 */
		virtual void onRealized() { }

		/**
		 * @brief Called when item is unrealized (became invisible)
		 *        and its text and content is deleted until it is realized again.
		 */
		virtual void onUnrealized() { }

	private:
		friend class Genlist;

		Elm_Genlist_Item_Type m_ItemType;
		Elm_Genlist_Item_Class *m_ItemClass;
		Elm_Object_Item *m_Item;
	};
}

#endif /* UI_GENLIST_ITEM_H */
