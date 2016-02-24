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

#ifndef UI_LIST_POPUP_H
#define UI_LIST_POPUP_H

#include "Ui/Popup.h"

namespace Ui
{
	class EXPORT_API ListPopup : public Popup
	{
	public:
		/**
		 * @brief Specific item selection callback.
		 */
		typedef std::function<void()> ItemSelectedCallback;

		/**
		 * @brief Any item selection callback.
		 * @param[in]   Item data
		 */
		typedef std::function<void(void *)> SelectedCallback;

		/**
		 * @brief Create list popup
		 * @param[in]   itemStyle   Genlist item style
		 */
		ListPopup(const char *itemStyle = "type1");

		/**
		 * @brief Add ListPopup item
		 * @param[in]   text        Main text of the item
		 * @param[in]   data        Data associated with the item
		 * @param[in]   callback    Callback to be called when item is selected
		 * @return Added item on success, otherwise nullptr
		 */
		Elm_Object_Item *addItem(std::string text, void *data, ItemSelectedCallback callback = nullptr);
		Elm_Object_Item *addItem(std::string text, ItemSelectedCallback callback = nullptr);
		Elm_Object_Item *addItem(void *data, ItemSelectedCallback callback = nullptr);

		/**
		 * @brief Set item selection callback.
		 * @param[in]   callback    Callback to be called when any item is selected
		 */
		void setSelectedCallback(SelectedCallback callback);

	protected:
		/**
		 * @see Control::onCreate()
		 */
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

		/**
		 * @brief Called when item's text part needs to be updated.
		 * @details If "elm.text" part is not handled in this function then it will
		 *          contain the text specified in ListPopup::addItem().
		 * @see Elm_Gen_Item_Text_Get_Cb.
		 */
		virtual char *getItemText(void *data, const char *part) { return nullptr; }

		/**
		 * @brief Called when item's content part needs to be updated.
		 * @see Elm_Gen_Item_Content_Get_Cb.
		 */
		virtual Evas_Object *getItemContent(void *data, const char *part) { return nullptr; }

		/**
		 * @brief Called when any item is selected.
		 * @param[in]   data    Item data
		 */
		virtual void onItemSelected(void *data) { }

	private:
		struct ItemData;

		static char *getText(ItemData *itemData, Evas_Object *genlist, const char *part);
		static Evas_Object *getContent(ItemData *itemData, Evas_Object *genlist, const char *part);
		static void onSelected(ListPopup *popup, Evas_Object *genlist, Elm_Object_Item *item);

		Evas_Object *m_Genlist;
		Elm_Genlist_Item_Class m_ItemClass;
		SelectedCallback m_OnSelected;
	};
}

#endif /* UI_LIST_POPUP_H */
