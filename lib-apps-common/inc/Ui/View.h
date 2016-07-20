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

#ifndef UI_VIEW_H
#define UI_VIEW_H

#include "Ui/Control.h"
#include "Ui/NavigatorPage.h"

namespace Ui
{
	class Navigator;

	/**
	 * @brief Navigation type supported by Navigator
	 */
	enum NavigatorType
	{
		StackNavigator = TypeMax, /**< Allows navigation to the added View or the previous View */
		TabNavigator              /**< Allows navigation to any added View */
	};

	/**
	 * @brief Represents content of a Window or Navigator page
	 */
	class EXPORT_API View : public Control
	{
	public:
		View(int type = TypeView);

		/**
		 * @brief Get parent Navigator
		 * @param[in]   type    Navigator type
		 * @return Innermost parent Navigator of the specified type if exists,
		 *         otherwise nullptr
		 */
		Navigator *getNavigator(NavigatorType type = StackNavigator) const;

		/**
		 * @return NavigatorPage of this View
		 */
		NavigatorPage *getPage() const;

	protected:
		/**
		 * @brief Called after Navigator has navigated to or from this View
		 * @param[in]   isCurrent   Specifies the new state of this View
		 */
		virtual void onNavigation(bool isCurrent) { }

		/**
		 * @brief Called when NavigatorPage is attached to this View
		 */
		virtual void onPageAttached(NavigatorPage *page) { }

		/**
		 * @brief Called when "back" button is pressed
		 * @return true to navigate to the previous view, otherwise false
		 */
		virtual bool onBackPressed() { return true; }

		/**
		 * @brief Called when "menu" button is pressed
		 */
		virtual void onMenuPressed() { }

	private:
		friend class Window;
		friend class Navigator;
		void onNavigatorAttached(Navigator *stackNavi, Navigator *tabNavi, NavigatorPage *page);

		Navigator     *m_StackNavi;
		Navigator     *m_TabNavi;
		NavigatorPage *m_Page;
	};
}

#endif /* UI_VIEW_H */
