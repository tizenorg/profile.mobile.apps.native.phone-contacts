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

#ifndef UI_MENU_H
#define UI_MENU_H

#include "Ui/Control.h"
#include <functional>

namespace Ui
{
	class EXPORT_API Menu : public Control
	{
	public:
		/**
		 * @brief Item selection callback
		 */
		typedef std::function<void()> ItemCallback;

		/**
		 * @brief Add menu item
		 * @param[in]   text        Item text
		 * @param[in]   callback    Item selection callback
		 * @return Added item on success, otherwise nullptr
		 */
		Elm_Object_Item *addItem(const char *text, ItemCallback callback);

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		static void onItemSelect(void *data, Evas_Object *obj, void *item);
		static void onItemDestroy(void *data, Evas_Object *obj, void *item);
		void onDismissed(Evas_Object *obj, void *eventInfo);
	};
}

#endif /* UI_MENU_H */
