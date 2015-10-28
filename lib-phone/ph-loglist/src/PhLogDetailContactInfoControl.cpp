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

#include "CommonPath.h"
#include "ContactsAppControl.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "ContactsThumbnail.h"
#include "WUiTimer.h"

#include "PhPath.h"
#include "PhCommon.h"

#include "PhLogUtil.h"
#include "PhLogDetailContactInfoControl.h"

#define DETAILS_BOX_PADDING		16

PhLogDetailContactInfoControl::PhLogDetailContactInfoControl(const PhLogDbMgr::RecordInfo *recordInfo)
	: __baseLayout(NULL)
{
	__personId = recordInfo->personId;

	if (recordInfo->displayName) __name = recordInfo->displayName;
	if (recordInfo->number) __number = recordInfo->number;
	if (recordInfo->imgPath) __imgPath = recordInfo->imgPath;
	if (recordInfo->callDesc) __callDesc = recordInfo->callDesc;
	WDEBUG("name : %s ,number : %s, imgPath : %s, callDesc: %s",recordInfo->displayName, recordInfo->number, recordInfo->imgPath, recordInfo->callDesc );
}

Evas_Object* PhLogDetailContactInfoControl::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();
	elm_theme_extension_add(nullptr, ContactsCommon::getResourcePath(COMMON_BUTTONS_EDJ).c_str());

	if (__personId > 0 || __number.empty()) {
		__baseLayout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/detail/info/contact/contact_info/saved");
		__setDisplayName();
	} else {
		__baseLayout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/detail/info/contact/contact_info/unsaved");
		__setButton();
	}

	__setThumbnail();

	return __baseLayout;
}

void PhLogDetailContactInfoControl::__setThumbnail()
{
	Evas_Object *thumbnail = createThumbnail(__baseLayout, THUMBNAIL_240, __imgPath.c_str());
	evas_object_event_callback_add(thumbnail, EVAS_CALLBACK_MOUSE_UP,
			(Evas_Object_Event_Cb) &PhLogDetailContactInfoControl::onThumbnailPressed, this);
	elm_layout_content_set(__baseLayout, "image.thumbnail", thumbnail);
}

void PhLogDetailContactInfoControl::__setDisplayName()
{
	WHIT();
	elm_object_domain_translatable_part_text_set(__baseLayout, "text.name", TEXT_DOMAIN, PhCommon::convert2MarkupString(PhLogUtil::getDisplayText(__personId, __name.c_str(), __number.c_str(), __callDesc.c_str())).c_str());
}

void PhLogDetailContactInfoControl::__setButton()
{
	Evas_Object *box = elm_box_add(__baseLayout);
	elm_box_padding_set(box, ContactsCommon::getScaledWidth(DETAILS_BOX_PADDING), 0);
	elm_box_horizontal_set(box, EINA_TRUE);
	elm_layout_content_set(__baseLayout, "box.buttons", box);
	Evas_Object * createContactBtn = PhCommon::createButton(box, "custom_default", NULL, [](void *data, Evas_Object *obj, void *event_info){
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogDetailContactInfoControl * p = (PhLogDetailContactInfoControl *)data;
		launchContactCreate(p->__number.c_str());
	}, this);

	WPRET_M( NULL == createContactBtn, "PhCommon::createButton() is failed!");
	evas_object_size_hint_align_set(createContactBtn, EVAS_HINT_FILL, 0.5);
	elm_object_domain_translatable_text_set(createContactBtn, TEXT_DOMAIN, T_("IDS_LOGS_BUTTON_CREATE_CONTACT_ABB"));
	elm_box_pack_end(box, createContactBtn);

	Evas_Object * updateContactBtn = PhCommon::createButton(box, "custom_default", NULL, [](void *data, Evas_Object *obj, void *event_info){
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogDetailContactInfoControl * p = (PhLogDetailContactInfoControl *)data;
		launchContactEdit(0, p->__number.c_str());
	}, this);
	WPRET_M( NULL == updateContactBtn, "PhCommon::createButton() is failed!");
	evas_object_size_hint_align_set(updateContactBtn, EVAS_HINT_FILL, 0.5);
	elm_object_domain_translatable_text_set(updateContactBtn, TEXT_DOMAIN, T_("IDS_LOGS_BUTTON_UPDATE_CONTACT_ABB2"));
	elm_box_pack_end(box, updateContactBtn);
}

void PhLogDetailContactInfoControl::onThumbnailPressed(void *data)
{
	PhLogDetailContactInfoControl *control = (PhLogDetailContactInfoControl*) data;
	if (control->__personId > 0) {
		launchContactDetails(control->__personId);
	}
}
