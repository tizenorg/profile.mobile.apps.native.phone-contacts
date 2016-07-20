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

#ifndef UI_CHECK_ITEM
#define UI_CHECK_ITEM

#include "Ui/GenItem.h"

namespace Ui
{
	/**
	 * @brief GenContainer check item
	 */
	class EXPORT_API CheckItem : public Ui::GenItem
	{
	public:
		/**
		 * @brief Item check callback.
		 * @param[in]   Whether item is checked
		 * @return Whether item's state should be changed.
		 */
		typedef std::function<bool(bool)> CheckCallback;

		CheckItem();
		virtual ~CheckItem() override;

		/**
		 * @return Whether the item is checked.
		 */
		bool isChecked() const;

		/**
		 * @brief Set item check state.
		 * @param[in]   isChecked   Whether item is checked
		 * @return Whether the state was changed successfully.
		 */
		bool setChecked(bool isChecked);

		/**
		 * @brief Set item check callback.
		 * @param[in]   callback    Callback to be called when item is checked/unchecked
		 */
		void setCheckCallback(CheckCallback callback);

		/**
		 * @brief Set item which "checked" state should be synchronized with this item.
		 * @param[in]   item    Item to link with
		 */
		void setLinkedItem(CheckItem *item);

		/**
		 * @brief Unset linked item.
		 */
		void unsetLinkedItem();

	protected:
		/**
		 * @brief Update the part containing check component.
		 */
		void updateCheckPart();

		/**
		 * @see GenItem::getContent()
		 * @remark Use it in derived class to create check component
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenItem::onSelected()
		 */
		virtual void onSelected() override;

		/**
		 * @brief Called when item's "checked" state changes.
		 * @param[in]   isChecked   Whether item is checked
		 * @return Whether item's state should be changed.
		 */
		virtual bool onChecked(bool isChecked) { return true; }

	private:
		void onCheckChanged(Evas_Object *check, void *eventInfo);
		bool notifyCheck();

		std::string m_CheckPart;
		Eina_Bool m_IsChecked;
		bool m_IsChecking;
		CheckCallback m_OnChecked;
		CheckItem *m_LinkedItem;
	};
}

#endif /* UI_CHECK_ITEM */
