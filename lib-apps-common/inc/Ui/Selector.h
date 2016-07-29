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

#ifndef UI_SELECTOR_H
#define UI_SELECTOR_H

#include "Ui/Control.h"
#include <functional>

namespace Ui
{
	/**
	 * @brief Interface for controls that provide item selection functionality.
	 */
	class EXPORT_API Selector : public Control
	{
	public:
		/**
		 * @brief Item selection callback.
		 * @param[in]   Item data
		 * @return Whether to perform the default action if any.
		 */
		typedef std::function<bool(void *)> SelectCallback;

		/**
		 * @brief Add item.
		 * @param[in]   value   Item data which will be passed to SelectedCallback
		 * @return Added item on success, otherwise nullptr.
		 */
		virtual Elm_Object_Item *addItem(void *data) = 0;

		/**
		 * @brief Set whether selection is enabled.
		 * @param[in]   isEnabled   Whether selection is enabled
		 */
		virtual void setSelectEnabled(bool isEnabled);

		/**
		 * @brief Set which item is currently selected.
		 * @param[in]   item    Item to be selected
		 */
		virtual void setSelectedItem(Elm_Object_Item *item) = 0;

		/**
		 * @brief Set item selection callback.
		 * @param[in]   callback    Callback to be called after item was selected
		 */
		void setSelectCallback(SelectCallback callback);

	protected:
		/**
		 * @brief Item selection callback.
		 * @param[in]   item    Selected item
		 */
		bool onSelected(Elm_Object_Item *item);

		SelectCallback m_OnSelected;
	};
}

#endif /* UI_SELECTOR_H */
