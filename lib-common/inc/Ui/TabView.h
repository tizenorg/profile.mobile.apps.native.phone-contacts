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

#ifndef UI_TAB_VIEW_H
#define UI_TAB_VIEW_H

#include "Ui/Navigator.h"
#include <vector>

namespace Ui
{
	/**
	 * @brief Scrollable tab navigator
	 */
	class EXPORT_API TabView : public Navigator
	{
	public:
		TabView();

		/**
		 * @see Navigator::getCurrentView()
		 */
		virtual View *getCurrentView() const override;

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;
		virtual void onPageAttached() override;

		virtual NavigatorPage *attachView(View *view) override;
		virtual void navigateToView(View *view) override;

	private:
		static void onTabDestroy(void *data, Evas_Object *obj, void *tabItem);
		void onTabSelect(Evas_Object *obj, void *tabItem);
		void onPageChanged(Evas_Object *obj, void *eventInfo);
		void onResize(Evas *e, Evas_Object *obj, void *eventInfo);

		Evas_Object *m_Tabbar;
		Evas_Object *m_Scroller;
		Evas_Object *m_Box;

		Evas_Coord m_Width;
		Evas_Coord m_Height;

		bool m_IsNavigating;
		size_t m_CurrentPage;
		std::vector<View *> m_Views;
	};
}

#endif /* UI_TAB_VIEW_H */
