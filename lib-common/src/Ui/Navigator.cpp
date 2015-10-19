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

void Navigator::appendView(View *view)
{
	Navigator *stackNavi = m_StackNavi;
	Navigator *tabNavi = m_TabNavi;
	if (m_Type == StackNavigator) {
		stackNavi = this;
	} else {
		tabNavi = this;
	}

	view->create(getEvasObject());
	view->onViewAttached(m_Window, stackNavi, tabNavi);
	view->onPageAttached(onPageAttach(view));
}

void Navigator::onNavigateTo()
{
	View *view = getCurrentView();
	if (view) {
		view->onNavigateTo();
	}
}

void Navigator::onNavigateFrom()
{
	View *view = getCurrentView();
	if (view) {
		view->onNavigateFrom();
	}
}

bool Navigator::onBackPressed()
{
	View *view = getCurrentView();
	return view ? view->onBackPressed() : true;
}

Evas_Object *Navigator::onMenuPressed()
{
	View *view = getCurrentView();
	return view ? view->onMenuPressed() : nullptr;
}
