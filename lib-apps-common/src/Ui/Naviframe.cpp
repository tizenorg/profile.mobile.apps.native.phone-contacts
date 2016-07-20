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

#include "Ui/Naviframe.h"
#include "Ui/NaviframePage.h"
#include "Ui/View.h"
#include "Utils/Callback.h"

using namespace Ui;

Naviframe::Naviframe()
	: Navigator(StackNavigator)
{
}

NaviframePage *Naviframe::getCurrentPage() const
{
	Elm_Object_Item *item = elm_naviframe_top_item_get(getEvasObject());
	return (NaviframePage *) elm_object_item_data_get(item);
}

Evas_Object *Naviframe::onCreate(Evas_Object *parent)
{
	Evas_Object *naviframe = elm_naviframe_add(parent);
	elm_naviframe_prev_btn_auto_pushed_set(naviframe, EINA_TRUE);

	evas_object_smart_callback_add(naviframe, "transition,finished",
			makeCallback(&Naviframe::onTransition), this);

	return naviframe;
}

NaviframePage *Naviframe::attachView(View *view)
{
	notifyNavigation(getCurrentPage(), false);

	Elm_Object_Item *naviItem = elm_naviframe_item_push(getEvasObject(), nullptr,
			nullptr, nullptr, view->create(getEvasObject()), nullptr);
	elm_naviframe_item_pop_cb_set(naviItem, makeCallback(&Naviframe::onItemPop), this);

	return new NaviframePage(naviItem);
}

void Naviframe::navigateToPage(NavigatorPage *page)
{
	if (page != getCurrentPage()) {
		Elm_Object_Item *naviItem = static_cast<NaviframePage *>(page)->m_NaviItem;
		elm_naviframe_item_pop_to(naviItem);
	}
}
void Naviframe::navigateFromPage(NavigatorPage *page)
{
	Elm_Object_Item *naviItem = static_cast<NaviframePage *>(page)->m_NaviItem;
	if (naviItem == elm_naviframe_top_item_get(getEvasObject())) {
		elm_naviframe_item_pop(getEvasObject());
	} else {
		elm_object_item_del(naviItem);
	}
}

Eina_Bool Naviframe::onItemPop(Elm_Object_Item *item)
{
	NaviframePage *page = static_cast<NaviframePage *>(elm_object_item_data_get(item));
	notifyNavigation(page, false);
	return EINA_TRUE;
}

void Naviframe::onTransition(Evas_Object *obj, void *eventInfo)
{
	notifyNavigation(getCurrentPage(), true);
}

bool Naviframe::onBackPressed()
{
	if (!Navigator::onBackPressed()) {
		return false;
	}

	NaviframePage *page = getCurrentPage();
	if (page && navigateFrom(page->getView())) {
		return false;
	}

	return true;
}
