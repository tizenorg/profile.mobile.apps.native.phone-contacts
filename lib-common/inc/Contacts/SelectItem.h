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

#ifndef CONTACTS_SELECT_ITEM_H
#define CONTACTS_SELECT_ITEM_H

#include "Contacts/SelectTypes.h"
#include "Ui/GenlistCheckItem.h"

namespace Contacts
{
	/**
	 * @brief Genlist item for SelectView that supports selection mode switching.
	 */
	class EXPORT_API SelectItem : public Ui::GenlistCheckItem
	{
	public:
		SelectItem();

		/**
		 * @return Item selection mode.
		 */
		SelectMode getSelectMode() const;

		/**
		 * @brief Set item selection mode.
		 */
		void setSelectMode(SelectMode selectMode);

	protected:
		/**
		 * @return Selection result associated with the item.
		 */
		virtual SelectResult getSelectResult() const = 0;

		/**
		 * @see GenlistItem::getContent()
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenlistItem::onSelected()
		 */
		virtual void onSelected() override;

		/**
		 * @brief Called when selection mode was changed.
		 * @param[in]   selectMode  New selection mode
		 */
		virtual void onSelectModeChanged(SelectMode selectMode) { }

	private:
		friend class SelectView;

		SelectMode m_SelectMode;
	};
}

#endif /* CONTACTS_SELECT_ITEM_H */
