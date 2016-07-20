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

#ifndef UX_SELECT_ALL_ITEM_H
#define UX_SELECT_ALL_ITEM_H

#include "Ui/CheckItem.h"

namespace Ux
{
	/**
	 * @brief "Select all" genlist item
	 */
	class SelectAllItem : public Ui::CheckItem
	{
	public:
		/**
		 * @brief Create "Select all" item.
		 * @param[in]   text    Item text
		 */
		SelectAllItem(const char *text);

	protected:
		/**
		 * @see GenItem::getText()
		 */
		virtual char *getText(Evas_Object *parent, const char *part) override;

		/**
		 * @see GenItem::getContent()
		 */
		virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

	private:
		std::string m_Text;
	};
}

#endif /* UX_SELECT_ALL_ITEM_H */
