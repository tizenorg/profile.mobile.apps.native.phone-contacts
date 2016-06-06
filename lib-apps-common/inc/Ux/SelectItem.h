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

#ifndef UX_SELECT_ITEM_H
#define UX_SELECT_ITEM_H

#include "Ui/GenlistCheckItem.h"
#include "Ux/SelectTypes.h"

namespace Ux
{
	class SelectView;

	/**
	 * @brief Genlist item for SelectView that supports selection mode switching.
	 */
	class EXPORT_API SelectItem : public Ui::GenlistCheckItem
	{
	public:
		SelectItem();

		/**
		 * @return Whether item is excluded from multiple selection.
		 */
		bool isExcluded() const;

		/**
		 * @brief Set item exclusion.
		 * @param[in]   isExcluded  Whether item is excluded from multiple selection
		 */
		void setExcluded(bool isExcluded);

		/**
		 * @return Item selection mode.
		 */
		SelectMode getSelectMode() const;

		/**
		 * @brief Set item selection mode.
		 */
		void setSelectMode(SelectMode selectMode);

		/**
		 * @return Selection result associated with the item.
		 */
		SelectResult getSelectResult() const;

		/**
		 * @return Whether item has custom selection result.
		 */
		bool hasCustomResult() const;

		/**
		 * @brief Set custom selection result to override the default result.
		 */
		void setCustomResult(SelectResult result);

		/**
		 * @brief Unset custom selection result to use default result.
		 */
		void unsetCustomResult();

	protected:
		/**
		 * @return Default selection result associated with the item.
		 */
		virtual SelectResult getDefaultResult() const = 0;

		/**
		 * @see GenlistItem::getContent()
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenlistItem::onSelected()
		 */
		virtual void onSelected() override;

		/**
		 * @see GenlistCheckItem::onChecked()
		 */
		virtual bool onChecked(bool isChecked) override;

		/**
		 * @brief Called when selection mode was changed.
		 * @param[in]   selectMode  New selection mode
		 */
		virtual void onSelectModeChanged(SelectMode selectMode) { }

	private:
		friend class SelectView;

		SelectView *m_SelectView;
		SelectMode m_SelectMode;
		SelectResult m_CustomResult;
		bool m_HasCustomResult;
		bool m_IsExcluded;
	};
}

#endif /* UX_SELECT_ITEM_H */
