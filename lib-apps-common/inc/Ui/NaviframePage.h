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

#ifndef UI_NAVIFRAME_PAGE_H
#define UI_NAVIFRAME_PAGE_H

#include "Ui/NavigatorPage.h"

namespace Ui
{
	class EXPORT_API NaviframePage : public NavigatorPage
	{
	public:
		NaviframePage(Elm_Object_Item *naviItem);

		/**
		 * @see NavigatorPage::setTitle()
		 */
		virtual void setTitle(const char *title) override;

		/**
		 * @see NavigatorPage::setStyle()
		 */
		virtual void setStyle(const char *style) override;

		/**
		 * @see NavigatorPage::setContent()
		 */
		virtual void setContent(const char *part, Evas_Object *content) override;

	private:
		friend class Naviframe;

		void onDestroy(Evas_Object *obj, void *eventInfo);

		Elm_Object_Item *m_NaviItem;
	};
}

#endif /* UI_NAVIFRAME_PAGE_H */
