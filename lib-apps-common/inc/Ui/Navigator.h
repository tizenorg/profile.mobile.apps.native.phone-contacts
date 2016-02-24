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

#ifndef UI_NAVIGATOR_H
#define UI_NAVIGATOR_H

#include "Ui/View.h"

namespace Ui
{
	class NavigatorPage;
	class EXPORT_API Navigator : public View
	{
	public:
		/**
		 * @return Currently displayed view
		 */
		virtual View *getCurrentView() const = 0;

		/**
		 * @brief Navigate to existing or new View
		 * @param[in]   view    View to navigate to
		 */
		void navigateTo(View *view);

	protected:
		Navigator(NavigatorType type);

		/**
		 * @brief Notify specified View about navigation
		 * @param[in]   view            View to notify
		 * @param[in]   isCurrentView   Specifies the new state of the @a view
		 */
		void notifyNavigation(View *view, bool isCurrentView);

		/**
		 * @brief Called after navigateTo() is called for a new view
		 * @param[in]   view    View to attach
		 * @return NavigatorPage for the view
		 */
		virtual NavigatorPage *attachView(View *view) = 0;

		/**
		 * @brief Called after navigateTo() is called
		 * @param[in]   view    View to navigate to
		 */
		virtual void navigateToView(View *view) = 0;

		/**
		 * @see View::onNavigation()
		 */
		virtual void onNavigation(bool isCurrentView) override;

		/**
		 * @see View::onBackPressed()
		 */
		virtual bool onBackPressed() override;

		/**
		 * @see View::onMenuPressed()
		 */
		virtual void onMenuPressed() override;

	private:
		NavigatorType m_Type;
	};
}

#endif /* UI_NAVIGATOR_H */
