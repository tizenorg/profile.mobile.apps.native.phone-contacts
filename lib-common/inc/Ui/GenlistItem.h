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
	class Genlist;
	class GenlistGroupItem;

	class EXPORT_API GenlistItem
	{
	public:
		GenlistItem();
		virtual ~GenlistItem();

		/**
		 * @return Whether the item is a group item.
		 */
		virtual bool isGroupItem() const { return false; }

		/**
		 * @return Item type.
		 */
		virtual Elm_Genlist_Item_Type getType() const { return ELM_GENLIST_ITEM_NONE; }

		/**
		 * @return Genlist object item.
		 */
		Elm_Object_Item *getObjectItem() const;

		/**
		 * @return Parent genlist.
		 */
		Genlist *getParent() const;

		/**
		 * @return Parent genlist group item or nullptr if none.
		 */
		GenlistGroupItem *getParentItem() const;

		/**
		 * @return Next item in genlist or nullptr if none.
		 */
		GenlistItem *getNextItem() const;

		/**
		 * @return Previous item in genlist or nullptr if none.
		 */
		GenlistItem *getPrevItem() const;

		/**
		 * @brief Remove item from genlist without destroying it.
		 */
		void pop();

	protected:
		/**
		 * @brief Create statically allocated item class
		 */
		static Elm_Genlist_Item_Class createItemClass(const char *style,
				const char *decorateStyle = nullptr,
				const char *editStyle = nullptr);

		/**
		 * @return Genlist item class of this item.
		 */
		virtual Elm_Genlist_Item_Class *getItemClass() const;

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
		 * @brief Called whenever any item's state part needs to be updated
		 * @details Genlist will emit signal "elm,state,x,active" or
		 *          "elm,state,x,passive" with part's name instead of 'x'.
		 * @param[in]   parent  Parent genlist
		 * @param[in]   part    Name of the part
		 * @return EINA_TRUE for active, EINA_FALSE for passive
		 */
		virtual Eina_Bool getState(Evas_Object *parent, const char *part) { return EINA_FALSE; }

		/**
		 * @brief Called after item was inserted into genlist.
		 */
		virtual void onInserted() { }

		/**
		 * @brief Called before item is popped.
		 */
		virtual void onPop() { }

		/**
		 * @brief Called when item is selected.
		 */
		virtual void onSelected() { }

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
		void onInserted(Elm_Object_Item *item);
		void onDestroy(Evas_Object *genlist);

		Elm_Object_Item *m_Item;
		bool m_Preserve;
	};
}

#endif /* UI_GENLIST_ITEM_H */
