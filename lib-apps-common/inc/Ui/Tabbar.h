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

#ifndef UI_TABBAR_H
#define UI_TABBAR_H

#include "Ui/Selector.h"

namespace Ui
{
	/**
	 * @brief Selector control for switching between tabs.
	 */
	class EXPORT_API Tabbar : public Selector
	{
	public:
		/**
		 * @see Selector::addItem()
		 */
		virtual Elm_Object_Item *addItem(void *data) override;

		/**
		 * @see Selector::setSelectEnabled()
		 */
		virtual void setSelectEnabled(bool isEnabled) override;

		/**
		 * @see Selector::setSelectedItem()
		 */
		virtual void setSelectedItem(Elm_Object_Item *item) override;

	protected:
		/**
		 * @see Control::onCreate()
		 */
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		void onSelected(Evas_Object *tabbar, Elm_Object_Item *item);
	};
}

#endif /* UI_TABBAR_H */
