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
#include <functional>

namespace Ui
{
	/**
	 * @brief Genlist check item
	 */
	class EXPORT_API GenlistCheckItem : public Ui::GenlistItem
	{
	public:
		/**
		 * @brief Item select callback
		 */
		typedef std::function<void()> SelectedCallback;

		GenlistCheckItem();

		/**
		 * @return Whether the item is checked
		 */
		bool isChecked() const;

		/**
		 * @brief Set item check state
		 * @param[in]   state   New item state
		 */
		void setChecked(bool state);

		/**
		 * @brief Set item selected callback
		 * @param[in]   callback    Callback, that will be invoked on item selection
		 */
		void setSelectedCallback(SelectedCallback callback);

		/**
		 * @brief Unset item selected callback
		 */
		void unsetSelectedCallback();

	protected:
		/**
		 * @see GenlistItem::getContent()
		 * @remark Use it in derived class to create check component
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenlistItem::onSelected()
		 */
		virtual void onSelected() final;

	private:
		Evas_Object *createCheck(Evas_Object *parent);
		void toggleCheckState();
		Evas_Object *getCheck() const;

		bool m_Checked;
		std::string m_CheckPart;
		SelectedCallback m_OnSelected;
	};
}

#endif /* UI_GENLIST_CHECK_ITEM */
