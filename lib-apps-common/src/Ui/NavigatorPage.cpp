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

#include "Ui/NavigatorPage.h"
#include "Ui/Navigator.h"

using namespace Ui;

NavigatorPage::NavigatorPage()
	: m_Navigator(nullptr), m_View(nullptr)
{
}

NavigatorPage::~NavigatorPage()
{
	if (m_Navigator) {
		m_Navigator->onPageDestroy(this);
	}
}

Navigator *NavigatorPage::getNavigator() const
{
	return m_Navigator;
}

View *NavigatorPage::getView() const
{
	return m_View;
}

void NavigatorPage::setExpanded(bool isExpanded)
{
	setTitleVisibility(isExpanded);
	if (m_Navigator) {
		m_Navigator->getPage()->setTitleVisibility(!isExpanded);
	}
}

void NavigatorPage::close()
{
	m_Navigator->navigateFrom(m_View);
}

void NavigatorPage::onNavigatorAttached(Navigator *navigator, View *view)
{
	m_Navigator = navigator;
	m_View = view;
}
