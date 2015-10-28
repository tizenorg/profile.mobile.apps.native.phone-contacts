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


#include "CommonIcons.h"
#include "ContactsCommon.h"
#include "ContactsNumberItemIcons.h"
#include "ContactsAppControl.h"
#include "WDebugBase.h"

namespace
{
	void iconCallCb(void *data, Evas_Object *obj, void *event_info)
	{
		const char *number = (const char*)data;
		int err = launchCall(number);
		if (err != APP_CONTROL_ERROR_NONE) {
			WWARNING("launchMessageComposer() failed(%d)", err);
		}
	}

	void iconMessageCb(void *data, Evas_Object *obj, void *event_info)
	{
		const char *number = (const char*)data;
		int err = launchMessageComposer("sms:", number);
		if (err != APP_CONTROL_ERROR_NONE) {
			WWARNING("launchMessageComposer() failed(%d)", err);
		}
	}

	void createIconLayout(Evas_Object *box, std::string groupName, Evas_Smart_Cb cb, const char* number)
	{
		Evas_Object *layout = elm_image_add(box);
		static const std::string edjPath = ContactsCommon::getResourcePath(COMMON_ICONS_EDJ);
		elm_image_file_set(layout, edjPath.c_str(), groupName.c_str());
		evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.5);
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_min_set(layout, ContactsCommon::getScaledWidth(ICON_SIZE), ContactsCommon::getScaledHeight(ICON_SIZE));
		if (number) {
			evas_object_smart_callback_del(layout, "clicked", cb);
			evas_object_smart_callback_add(layout, "clicked", cb, number);
		}
		evas_object_propagate_events_set(layout, EINA_FALSE);
		elm_box_pack_end(box, layout);
		evas_object_show(layout);
	}
}

Evas_Object *createNumberItemIconsLayout(Evas_Object *parent, const char *number)
{
	Evas_Object *box = elm_box_add(parent);
	elm_box_horizontal_set(box, EINA_TRUE);
	WDEBUG("number = %s", number);
	elm_box_padding_set(box, ContactsCommon::getScaledWidth(SPACE_SIZE), 0);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	createIconLayout(box, GROUP_NUMBER_IC_CALL, iconCallCb, number);
	createIconLayout(box, GROUP_NUMBER_IC_MESSAGE, iconMessageCb, number);
	return box;
}
