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

#include "Ui/Navigator.h"

using namespace Ui;

Navigator::Navigator(NavigatorType type)
	: m_Type(type)
{
}

void Navigator::navigateTo(View *view)
{
	if (!view || view == getCurrentView()) {
		return;
	}

	notifyNavigation(getCurrentView(), false);

	if (view->getNavigator(m_Type) != this) {
		Navigator *stackNavi = m_StackNavi;
		Navigator *tabNavi = m_TabNavi;
		if (m_Type == StackNavigator) {
			stackNavi = this;
		} else {
			tabNavi = this;
		}

		view->create(getEvasObject());
		view->onViewAttached(stackNavi, tabNavi);
		view->onPageAttached(attachView(view));
	}

	navigateToView(view);
	notifyNavigation(view, true);
}

void Navigator::notifyNavigation(View *view, bool isCurrentView)
{
	if (view) {
		view->onNavigation(isCurrentView);
	}
}

void Navigator::onNavigation(bool isCurrentView)
{
	notifyNavigation(getCurrentView(), isCurrentView);
}

bool Navigator::onBackPressed()
{
	View *view = getCurrentView();
	return view ? view->onBackPressed() : true;
}

void Navigator::onMenuPressed()
{
	View *view = getCurrentView();
	if (view) {
		view->onMenuPressed();
	}
}
