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

#ifndef CONTACTS_SELECT_VIEW_H
#define CONTACTS_SELECT_VIEW_H

#include "Contacts/Common/SelectMode.h"
#include "Ui/View.h"

#include <vector>

namespace Ui
{
	class GenlistItem;
}

namespace Contacts
{
	class SelectItem;
	class SelectAllItem;

	/**
	 * @brief Base class for a view that support single and multiple selection modes.
	 */
	class SelectView : public Ui::View
	{
	public:
		SelectView();
		virtual ~SelectView() override;

		/**
		 * @brief Set selection mode.
		 * @param[in]   selectMode  Selection mode
		 */
		void setSelectMode(SelectMode selectMode);

		/**
		 * @brief Set item selection limit.
		 * @param[in]   count   Maximal selectable items count
		 */
		void setSelectLimit(size_t selectLimit);

		/**
		 * @brief Set selection callback.
		 * @param[in]   callback    Selection callback
		 */
		void setSelectCallback(SelectCallback callback);

	protected:
		/**
		 * @return View selection mode.
		 */
		SelectMode getSelectMode() const;

		/**
		 * @return Current seleciton limit.
		 */
		size_t getSelectLimit() const;

		/**
		 * @return Current selected items count.
		 */
		size_t getSelectCount() const;

		/**
		 * @return Page title format for the current selection mode.
		 */
		virtual const char *getPageTitle() = 0;

		/**
		 * @brief Creates "Done" and "Cancel" buttons in #SelectMulti mode.
		 * @see View::onPageAttached()
		 */
		virtual void onPageAttached() override;

		/**
		 * @brief Should be called when selectable item is inserted.
		 * @param[in]   item    Inserted item
		 */
		void onItemInserted(SelectItem *item);

		/**
		 * @brief Should be called when selectable item is about to be removed.
		 * @param[in]   item    Item being removed
		 */
		void onItemRemove(SelectItem *item);

		/**
		 * @brief Called when selectable item is pressed it #SelectNone mode.
		 * @param[in]   item    Pressed item
		 */
		virtual void onItemPressed(SelectItem *item) { }

		/**
		 * @brief Called when "Select All" item should be inserted.
		 * @param[in]   item    "Select All" genlist item
		 */
		virtual void onSelectAllInsert(Ui::GenlistItem *item) { }

		/**
		 * @brief Called when "Select All" item is about to be removed.
		 */
		virtual void onSelectAllRemove() { }

		/**
		 * @brief Called when selection mode was changed.
		 * @param[in]   selectMode  New selection mode
		 */
		virtual void onSelectModeChanged(SelectMode selectMode) { }

		/**
		 * @brief Called when selection limit was changed.
		 * @param[in]   selectLimit New selection limit
		 */
		virtual void onSelectLimitChanged(size_t selectLimit) { }

	private:
		void updatePageTitle();
		void updatePageButtons();
		void updateSelectAllItem();

		void createPageButtons();
		void destroyPageButtons();

		void onItemSelected(SelectItem *item);
		void onItemChecked(SelectItem *item, bool isChecked);
		void onSelectAllChecked(bool isChecked);

		void onDonePressed(Evas_Object *button, void *eventInfo);
		void onCancelPressed(Evas_Object *button, void *eventInfo);

		SelectAllItem *m_SelectAllItem;
		std::vector<SelectItem *> m_Items;

		Evas_Object *m_DoneButton;
		Evas_Object *m_CancelButton;

		size_t m_SelectCount;
		size_t m_SelectLimit;

		SelectMode m_SelectMode;
		SelectCallback m_OnSelected;
	};
}

#endif /* CONTACTS_SELECT_VIEW_H */
