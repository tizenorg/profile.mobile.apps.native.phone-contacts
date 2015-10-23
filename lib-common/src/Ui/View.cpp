/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
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

#include "Ui/View.h"

using namespace Ui;

View::View()
	: m_Window(nullptr), m_StackNavi(nullptr), m_TabNavi(nullptr),
	  m_NaviPage(nullptr)
{
}

View::~View()
{
	delete m_NaviPage;
}

Window *View::getWindow() const
{
	return m_Window;
}

Navigator *View::getNavigator(NavigatorType type) const
{
	return type == StackNavigator ? m_StackNavi : m_TabNavi;
}

NavigatorPage *View::getPage() const
{
	return m_NaviPage;
}

void View::onViewAttached(Window *window, Navigator *stackNavi, Navigator *tabNavi)
{
	m_Window = window;
	m_StackNavi = stackNavi;
	m_TabNavi = tabNavi;
}

void View::onPageAttached(NavigatorPage *page)
{
	if (page != m_NaviPage) {
		delete m_NaviPage;
		m_NaviPage = page;
	}

	onPageAttached();
}
