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

#ifndef UI_GEN_ITEM_H
#define UI_GEN_ITEM_H

#include "Ui/GenContainer.h"
#include "Utils/String.h"

#include <functional>
#include <memory>

namespace Ui
{
	class GenItem;
	class GenGroupItem;
	typedef std::weak_ptr<GenItem> GenItemPtr;

	class EXPORT_API GenItem
	{
	public:
		/**
		 * @brief Called when item is selected.
		 */
		typedef std::function<void()> SelectCallback;

		/**
		 * @brief Called before item is destroyed.
		 */
		typedef std::function<void()> DestroyCallback;

		/**
		 * @brief Create generic container item.
		 * @param[in]   type    Parent container type
		 */
		GenItem(GenContainer::Type type = GenContainer::TypeGenlist);
		virtual ~GenItem();

		/**
		 * @return Whether the item is realized.
		 */
		bool isRealized() const;

		/**
		 * @return Whether the item is inserted into container.
		 */
		bool isInserted() const;

		/**
		 * @return Whether the item is a group item.
		 */
		virtual bool isGroupItem() const { return false; }

		/**
		 * @return Whether the item can receive focus.
		 */
		virtual bool isFocusable() const { return false; }

		/**
		 * @return Item type.
		 */
		virtual Elm_Genlist_Item_Type getType() const { return ELM_GENLIST_ITEM_NONE; }

		/**
		 * @return Container object item.
		 */
		Elm_Object_Item *getObjectItem() const;

		/**
		 * @return Parent container.
		 */
		GenContainer *getParent() const;

		/**
		 * @return Parent group item or nullptr if none.
		 */
		GenGroupItem *getParentItem() const;

		/**
		 * @return Next item in container or nullptr if none.
		 */
		GenItem *getNextItem() const;

		/**
		 * @return Previous item in container or nullptr if none.
		 */
		GenItem *getPrevItem() const;

		/**
		 * @return GenItem weak pointer for safe referencing.
		 */
		GenItemPtr getWeakPtr();

		/**
		 * @brief Set item selection callback.
		 * @param[in]   callback    Callback to be called when item is selected
		 */
		void setSelectCallback(SelectCallback callback);

		/**
		 * @brief Set item destruction callback.
		 * @param[in]   callback    Callback to be called before item is destroyed
		 */
		void setDestroyCallback(SelectCallback callback);

		/**
		 * @brief Scroll to the item.
		 * @param[in]   position    Item position on screen
		 * @param[in]   isAnimated  Whether scrolling is animated or immediate
		 * @see Elm_Genlist_Item_Scrollto_Type or Elm_Gengrid_Item_Scrollto_Type
		 */
		void scrollTo(int position = ELM_GENLIST_ITEM_SCROLLTO_IN,
				bool isAnimated = false);

		/**
		 * @brief Scroll to the item and give it focus.
		 * @see GenItem::scrollTo()
		 */
		void focus(Elm_Genlist_Item_Scrollto_Type position = ELM_GENLIST_ITEM_SCROLLTO_IN,
				bool isAnimated = false);

		/**
		* @brief Update item part.
		* @see elm_gelist_item_fields_update or elm_gengrid_item_fields_update
		*/
		void update(const char *parts, int partType);

		/**
		 * @brief Remove item from genlist without destroying it.
		 */
		void pop();

	protected:
		/**
		 * @brief Create statically allocated item class
		 */
		static Elm_Gen_Item_Class createItemClass(const char *style,
				const char *decorateStyle = nullptr,
				const char *editStyle = nullptr);

		/**
		 * @return Generic item class of this item.
		 */
		virtual Elm_Gen_Item_Class *getItemClass() const;

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
		 * @brief Called when genlist filter is set using elm_genlist_filter_set().
		 * @param[in]   parent  Parent genlist
		 * @param[in]   filter  Filter to compare with
		 * @return Whether the item satisfies the filter.
		 */
		virtual Eina_Bool compare(Evas_Object *parent, void *filter) { return EINA_TRUE; }

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
		 * @brief Called when item is longpressed.
		 * @return Whether event is handled and onSelected() shouldn't be called.
		 */
		virtual bool onLongpressed() { return false; }

		/**
		 * @brief Called when item is realized (became visible).
		 */
		virtual void onRealized() { }

		/**
		 * @brief Called when item is unrealized (became invisible)
		 *        and its text and content is deleted until it is realized again.
		 */
		virtual void onUnrealized() { }

		/**
		 * @brief Called when item is focused by calling focus().
		 */
		virtual void onFocused() { }

	private:
		friend class GenContainer;
		void onInserted(Elm_Object_Item *item);
		void onDestroy(Evas_Object *genlist);

		void onSelected(Elm_Object_Item *item);
		void onLongpressed(Elm_Object_Item *item);
		void onRealized(Elm_Object_Item *item);
		void onUnrealized(Elm_Object_Item *item);

		Elm_Object_Item *m_Item;
		bool m_Preserve;
		bool m_IsRealized;
		bool m_IsFocusPending;
		bool m_IsLongpressed;

		std::shared_ptr<GenItem> m_SelfPtr;
		SelectCallback m_OnSelected;
		DestroyCallback m_OnDestroy;
		const struct GenItemApi *m_Api;
	};
}

#endif /* UI_GEN_ITEM_H */
