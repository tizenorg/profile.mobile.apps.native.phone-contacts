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

#ifndef UI_TAB_PAGE_H
#define UI_TAB_PAGE_H

#include "Ui/Control.h"
#include "Ui/NavigatorPage.h"

namespace Ui
{
	class EXPORT_API TabPage : public Control, public NavigatorPage
	{
	public:
		TabPage(size_t index);
		virtual ~TabPage() override;

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
		friend class TabView;
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		void onTabAttached(Elm_Object_Item *tabItem);

		size_t m_Index;
		Elm_Object_Item *m_TabItem;

		Evas_Object *m_Sizer;
		Evas_Object *m_Page;
	};
}

#endif /* UI_TAB_PAGE_H */
