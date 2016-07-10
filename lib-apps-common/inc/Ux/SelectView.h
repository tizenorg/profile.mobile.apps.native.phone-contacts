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

#ifndef UX_SELECT_VIEW_H
#define UX_SELECT_VIEW_H

#include "Ui/View.h"
#include "Ux/SelectAllItem.h"
#include "Ux/SelectTypes.h"

#include <memory>
#include <vector>

namespace Ui
{
	class GenlistItem;
}

namespace Ux
{
	class SelectItem;

	/**
	 * @brief Base class for a view that support single and multiple selection modes.
	 */
	class EXPORT_API SelectView : public Ui::View
	{
	public:
		/**
		 * @brief Translatable strings table for view elements.
		 */
		struct Strings
		{
			const char *selectAll;      /**< "Select all" item text. */
			const char *buttonDone;     /**< "Done" button text. */
			const char *buttonCancel;   /**< "Cancel" button text. */
			const char *titleDefault;   /**< Title for #SelectNone mode. */
			const char *titleSingle;    /**< Title for #SelectSingle mode */
			const char *titleMulti;     /**< Title for #SelectMulti mode */
			const char *titleWithCount; /**< Title for #SelectMulti mode with selection count.
											 Can contain one integer format specifier. */
			const char *titleWithLimit; /**< Title for #SelectMulti mode with limit.
											 Can contain two integer format specifiers. */
			const char *popupLimit;     /**< Selection limit reached popup text.
											 Can contain one integer format specifiers. */
		};

		/**
		 * @brief Called when item's "checked" state changed in #SelectMulti mode.
		 * @param[in]   item        Changed item
		 * @param[in]   isChecked   Whether item is checked
		 * @param[in]   isSelectAll Whether item is being checked via "Select All"
		 */
		typedef std::function<bool(SelectItem *item, bool isChecked, bool isSelectAll)> CheckCallback;

		SelectView();

		/**
		 * @brief Set translatable strings for the view.
		 * @remark Should be called before create().
		 * @param[in]   strings    Translatable strings table
		 */
		void setStrings(const Strings &strings);

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

		/**
		 * @brief Set cancel callback.
		 * @param[in]   callback    Cancel callback
		 */
		void setCancelCallback(CancelCallback callback);

		/**
		 * @brief Set item check callback.
		 * @param[in]   callback    Check callback
		 */
		void setCheckCallback(CheckCallback callback);

	protected:
		/**
		 * @return View selection mode.
		 */
		SelectMode getSelectMode() const;

		/**
		 * @return Current selection limit.
		 */
		size_t getSelectLimit() const;

		/**
		 * @return Maximum allowed number of selected items.
		 */
		size_t getSelectMax() const;

		/**
		 * @return Current selected items count.
		 */
		size_t getSelectCount() const;

		/**
		 * @brief Add selectable item to be managed by the view.
		 * @param[in]   item    Item to add
		 */
		void addSelectItem(SelectItem *item);

		/**
		 * @brief Remove selectable item.
		 * @param[in]   item    Item to remove
		 */
		void removeSelectItem(SelectItem *item);

		/**
		 * @brief Remove all select items
		 */
		void removeAllSelectItems();
		/**
		 * @brief Creates "Done" and "Cancel" buttons in #SelectMulti mode.
		 * @see View::onPageAttached()
		 */
		virtual void onPageAttached(Ui::NavigatorPage *page) override;

		/**
		 * @brief Calls cancel callback if it exists.
		 * @see View::onBackPressed()
		 */
		virtual bool onBackPressed() override;

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
		friend class SelectItem;

		enum CountChange
		{
			CountIncrement,
			CountDecrement
		};

		void updatePageTitle();
		void updatePageButtons();
		void updateSelectAllItem();

		void updateDoneButtonState();
		void updateSelectAllState();

		void updateSelectCount(CountChange change, SelectItem *item);
		void updateItemCount(CountChange change, SelectItem *item);

		void createPageButtons();
		void destroyPageButtons();

		void onItemExcluded(SelectItem *item, bool isExcluded);
		void onItemSelected(SelectItem *item);
		bool onItemChecked(SelectItem *item, bool isChecked);
		bool onSelectAllChecked(bool isChecked);
		void onSelectAllDestroy();

		void onDonePressed(Evas_Object *button, void *eventInfo);
		void onCancelPressed(Evas_Object *button, void *eventInfo);

		std::unique_ptr<SelectAllItem> m_SelectAllItem;
		std::vector<SelectItem *> m_Items;

		Evas_Object *m_DoneButton;
		Evas_Object *m_CancelButton;

		bool m_IsChecking;
		size_t m_TotalCount;
		size_t m_SelectCount;
		size_t m_SelectLimit;

		SelectMode m_SelectMode;
		SelectCallback m_OnSelected;
		CheckCallback m_OnChecked;
		CancelCallback m_OnCanceled;

		Strings m_Strings;
	};
}

#endif /* UX_SELECT_VIEW_H */
