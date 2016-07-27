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

#ifndef UI_NAVIGATOR_H
#define UI_NAVIGATOR_H

#include "Ui/View.h"
#include <functional>

namespace Ui
{
	class NavigatorPage;

	/**
	 * @brief Navigator base class.
	 */
	class EXPORT_API Navigator : public View
	{
	public:
		/**
		 * @brief Called when the navigator is about to navigate away
		 *        from it's last remaining page.
		 * @return Whether to navigate from the last page.
		 */
		typedef std::function<bool()> LastPageCallback;

		/**
		 * @return Currently displayed page.
		 */
		virtual NavigatorPage *getCurrentPage() const = 0;

		/**
		 * @return Page count.
		 */
		size_t getPageCount() const;

		/**
		 * @brief Add view to navigator.
		 * @details Adding a view may also result in navigating to it
		 *          if it's a first view or this is a #StackNavigator.
		 * @param[in]   view    View to add
		 */
		void appendView(View *view);

		/**
		 * @brief Navigate to existing or new View.
		 * @param[in]   view    View to navigate to
		 */
		void navigateTo(View *view);

		/**
		 * @brief Navigate away from the view and destroy it.
		 * @param[in]   view    View to navigate from
		 * @return Whether navigation occurred.
		 */
		bool navigateFrom(View *view);

		/**
		 * @brief Set last page callback.
		 * @param[in]   callback    Last page navigation callback.
		 */
		void setLastPageCallback(LastPageCallback callback);

		/**
		 * @brief Unset last page callback.
		 */
		void unsetLastPageCallback();

	protected:
		Navigator(NavigatorType type);

		/**
		 * @brief Notify specified Page about navigation.
		 * @param[in]   page        Page to notify
		 * @param[in]   isCurrent   The new state of the @a page
		 */
		void notifyNavigation(NavigatorPage *page, bool isCurrent);

		/**
		 * @brief Called after navigateTo() is called for a new view
		 * @param[in]   view    View to attach
		 * @return NavigatorPage for the view
		 */
		virtual NavigatorPage *attachView(View *view) = 0;

		/**
		 * @brief Called after navigateTo() is called
		 * @param[in]   page    Page to navigate to
		 */
		virtual void navigateToPage(NavigatorPage *page) = 0;

		/**
		 * @brief Called after navigateFrom() is called
		 * @param[in]   page    Page to navigate from
		 */
		virtual void navigateFromPage(NavigatorPage *page) = 0;

		/**
		 * @see View::onNavigation()
		 */
		virtual void onNavigation(bool isCurrent) override;

		/**
		 * @brief Called when device orientation is changed
		 */
		virtual void onRotationChanged(int degree) override;

		/**
		 * @see View::onBackPressed()
		 */
		virtual bool onBackPressed() override;

		/**
		 * @see View::onMenuPressed()
		 */
		virtual void onMenuPressed() override;

	private:
		friend class NavigatorPage;
		void onPageDestroy(NavigatorPage *page);

		size_t m_PageCount;
		LastPageCallback m_OnLastPage;
	};
}

#endif /* UI_NAVIGATOR_H */
