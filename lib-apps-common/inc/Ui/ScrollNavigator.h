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

#ifndef UI_SCROLL_NAVIGATOR_H
#define UI_SCROLL_NAVIGATOR_H

#include "Ui/Navigator.h"
#include "Ui/ScrollPage.h"
#include <vector>

namespace Ui
{
	/**
	 * @brief Scrollable page navigator.
	 */
	class EXPORT_API ScrollNavigator : public Navigator
	{
	public:
		ScrollNavigator();

		/**
		 * @see Navigator::getCurrentPage()
		 */
		virtual ScrollPage *getCurrentPage() const override;

		/**
		 * @brief Set whether navigation is allowed either by user or by calling navigateTo().
		 * @param[in]   isEnabled   Whether navigation is enabled
		 */
		void setNavigationEnabled(bool isEnabled);

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		virtual void onPageAttached(NavigatorPage *page) override;

		virtual ScrollPage *attachView(View *view) override;
		virtual void navigateToPage(NavigatorPage *page) override;
		virtual void navigateFromPage(NavigatorPage *page) override;

	private:
		void onTabSelected(Evas_Object *obj, Elm_Object_Item *selectedItem);
		void onPageChanged(Evas_Object *obj, void *eventInfo);
		void onResize(Evas *e, Evas_Object *obj, void *eventInfo);

		Evas_Object *m_Tabbar;
		Evas_Object *m_Scroller;
		Evas_Object *m_Box;

		Evas_Coord m_Width;
		Evas_Coord m_Height;

		bool m_IsNavigating;
		bool m_IsNavigationEnabled;
		ScrollPage *m_CurrentPage;
		std::vector<ScrollPage *> m_Pages;
	};
}

#endif /* UI_SCROLL_NAVIGATOR_H */
