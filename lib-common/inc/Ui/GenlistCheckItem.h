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

#ifndef UI_GENLIST_CHECK_ITEM
#define UI_GENLIST_CHECK_ITEM

#include "Ui/GenlistItem.h"

namespace Ui
{
	/**
	 * @brief Genlist check item
	 */
	class EXPORT_API GenlistCheckItem : public Ui::GenlistItem
	{
	public:
		/**
		 * @brief Item check callback.
		 * @param[in]   Whether item is checked
		 */
		typedef std::function<void(bool)> CheckCallback;

		GenlistCheckItem();

		/**
		 * @return Whether the item is checked.
		 */
		bool isChecked() const;

		/**
		 * @brief Set item check state.
		 * @param[in]   isChecked   Whether item is checked
		 */
		void setChecked(bool isChecked);

		/**
		 * @brief Set item check callback.
		 * @param[in]   callback    Callback to be called when item is checked/unchecked
		 */
		void setCheckCallback(CheckCallback callback);

	protected:
		/**
		 * @see GenlistItem::getContent()
		 * @remark Use it in derived class to create check component
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenlistItem::onSelected()
		 */
		virtual void onSelected() override;

		/**
		 * @brief Called when item is checked or unchecked.
		 * @param[in]   isChecked   Whether item is checked
		 */
		virtual void onChecked(bool isChecked) { }

	private:
		void onChecked();
		void onCheckChanged(Evas_Object *check, void *eventInfo);

		std::string m_CheckPart;
		Eina_Bool m_IsChecked;
		CheckCallback m_OnChecked;
	};
}

#endif /* UI_GENLIST_CHECK_ITEM */
