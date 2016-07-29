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

#ifndef UI_HOVERSEL_H
#define UI_HOVERSEL_H

#include "Ui/Selector.h"

namespace Ui
{
	class EXPORT_API Hoversel : public Selector
	{
	public:
		/**
		 * @brief Add item with text.
		 * @param[in]   text        Item text
		 * @param[in]   value       Item data
		 * @return Added item on success, otherwise nullptr.
		 * @see Selector::addItem()
		 */
		Elm_Object_Item *addItem(const char *text, void *data);

		/**
		 * @see Selector::addItem()
		 */
		virtual Elm_Object_Item *addItem(void *data) override;

		/**
		 * @brief Set text of currently selected item.
		 * @param[in]   text        Text to be displayed in hoversel button
		 */
		void setText(const char *text);

		/**
		 * @see Selector::setSelectedItem()
		 */
		virtual void setSelectedItem(Elm_Object_Item *item) override;

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		void onSelected(Evas_Object *hoversel, Elm_Object_Item *item);
		static void onExpanded(void *data, Evas_Object *hoversel, void *eventInfo);
		static void onDismissed(void *data, Evas_Object *hoversel, void *eventInfo);
		static void onBackPressed(void *data, Evas_Object *hoversel, void *eventInfo);
	};
}

#endif /* UI_HOVERSEL_H */
