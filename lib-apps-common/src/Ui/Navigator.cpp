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

#include "Ui/Navigator.h"
#include "Ui/NavigatorPage.h"

using namespace Ui;

namespace Ui
{
	template <>
	Navigator *Control::findParent<Navigator>(Evas_Object *object)
	{
		return static_cast<Navigator *>(findParent(object, StackNavigator));
	}
}

Navigator::Navigator(NavigatorType type)
	: View(type), m_PageCount(0)
{
}

size_t Navigator::getPageCount() const
{
	return m_PageCount;
}

void Navigator::appendView(View *view)
{
	if (!view) {
		return;
	}

	Navigator *stackNavi = m_StackNavi;
	Navigator *tabNavi = m_TabNavi;
	if (getType() == StackNavigator) {
		stackNavi = this;
	} else {
		tabNavi = this;
	}

	NavigatorPage *page = attachView(view);
	page->onNavigatorAttached(this, view);
	view->onNavigatorAttached(stackNavi, tabNavi, page);

	++m_PageCount;
}

void Navigator::navigateTo(View *view)
{
	if (!view) {
		return;
	}

	NavigatorPage *page = view->getPage();
	if (page && page == getCurrentPage()) {
		return;
	}

	if (view->getNavigator(NavigatorType(getType())) != this) {
		appendView(view);
		page = view->getPage();
	}

	navigateToPage(page);
}

bool Navigator::navigateFrom(View *view)
{
	if (!view || view->getNavigator(NavigatorType(getType())) != this) {
		return false;
	}

	if (getPageCount() == 1) {
		if (!m_OnLastPage || m_OnLastPage()) {
			NavigatorPage *page = getPage();
			if (page) {
				page->close();
				return true;
			}
		}

		return false;
	}

	navigateFromPage(view->getPage());
	return true;
}

void Navigator::setLastPageCallback(LastPageCallback callback)
{
	m_OnLastPage = std::move(callback);
}

void Navigator::unsetLastPageCallback()
{
	m_OnLastPage = nullptr;
}

void Navigator::notifyNavigation(NavigatorPage *page, bool isCurrent)
{
	if (page) {
		page->getView()->onNavigation(isCurrent, getRotation());
	}
}

void Navigator::onNavigation(bool isCurrent)
{
	notifyNavigation(getCurrentPage(), isCurrent);
}

void Navigator::onRotationChanged(int degree)
{
	NavigatorPage *page = getCurrentPage();
	if (page) {
		page->getView()->onRotation(degree);
	}
}

bool Navigator::onBackPressed()
{
	NavigatorPage *page = getCurrentPage();
	return page ? page->getView()->onBackPressed() : true;
}

void Navigator::onMenuPressed()
{
	NavigatorPage *page = getCurrentPage();
	if (page) {
		page->getView()->onMenuPressed();
	}
}

void Navigator::onPageDestroy(NavigatorPage *page)
{
	--m_PageCount;
}
