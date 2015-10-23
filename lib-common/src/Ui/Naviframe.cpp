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

#include "Ui/Naviframe.h"
#include "Ui/NaviframePage.h"
#include "Ui/View.h"
#include "Utils/Callback.h"

using namespace Ui;

Naviframe::Naviframe()
	: Navigator(StackNavigator)
{
}

View *Naviframe::getCurrentView() const
{
	Elm_Object_Item *item = elm_naviframe_top_item_get(getEvasObject());
	if (!item) {
		return nullptr;
	}

	return static_cast<View*>(elm_object_item_data_get(item));
}

Evas_Object *Naviframe::onCreate(Evas_Object *parent)
{
	Evas_Object *naviframe = elm_naviframe_add(parent);
	elm_naviframe_prev_btn_auto_pushed_set(naviframe, EINA_TRUE);

	return naviframe;
}

NavigatorPage *Naviframe::attachView(View *view)
{
	Elm_Object_Item *naviItem = elm_naviframe_item_push(getEvasObject(), nullptr,
			nullptr, nullptr, view->getEvasObject(), nullptr);
	elm_object_item_data_set(naviItem, view);
	elm_naviframe_item_pop_cb_set(naviItem, makeCallback(&Naviframe::onItemPop), this);
	return new NaviframePage(naviItem);
}

void Naviframe::navigateToView(View *view)
{
	if (view != getCurrentView()) {
		NaviframePage *page = static_cast<NaviframePage*>(view->getPage());
		elm_naviframe_item_promote(page->m_NaviItem);
	}
}

Eina_Bool Naviframe::onItemPop(Elm_Object_Item *item)
{
	evas_object_smart_callback_add(getEvasObject(), "transition,finished",
			makeCallback(&Naviframe::onTransition), this);

	View *view = static_cast<View*>(elm_object_item_data_get(item));
	notifyNavigation(view, false);
	return EINA_TRUE;
}

void Naviframe::onTransition(Evas_Object *obj, void *eventInfo)
{
	notifyNavigation(getCurrentView(), true);

	evas_object_smart_callback_del(getEvasObject(), "transition,finished",
			makeCallback(&Naviframe::onTransition));
}

bool Naviframe::onBackPressed()
{
	if (!Navigator::onBackPressed()) {
		return false;
	}

	if (elm_naviframe_top_item_get(getEvasObject())
			!= elm_naviframe_bottom_item_get(getEvasObject())) {
		elm_naviframe_item_pop(getEvasObject());
		return false;
	}

	return true;
}
