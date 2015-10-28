/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "CtView.h"
#include "ContactsDebug.h"


CtView::CtView()
{
	WHIT();
	__layout = NULL;
	__emergencyMode = 0;
	__easyMode = 0;


	//evas_object_smart_callback_add(layout, "language,changed", __languageChangedCb , this);
	//evas_object_smart_callback_add(getWindow()->getEvasObj(), "wm,rotation,changed", __windowChangedCb , this);
}

CtView::~CtView()
{
	WHIT();

	evas_object_smart_callback_del_full(__layout, "language,changed", __languageChangedCb , this);
/*
	evas_object_smart_callback_del_full(__layout, "language,changed", __languageChangedCb , this);
	evas_object_smart_callback_del_full(getWindow()->getEvasObj(), "wm,rotation,changed", __windowRotationCb , this);
*/
}


void CtView::__languageChangedCb(void* data, Evas_Object* obj, void* event_info)
{
	WHIT();
	CtView *self = (CtView*)data;
	self->onLanguageChanged();
}

Evas_Object* CtView::createBaseLayout(Evas_Object* parent)
{
	WHIT();
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	evas_object_smart_callback_add(layout, "language,changed", __languageChangedCb , this);

	return layout;

}


