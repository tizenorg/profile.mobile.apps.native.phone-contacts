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

#include "ContactsCommon.h"

#include "CtPath.h"
#include "CtType.h"
#include "ContactsDebug.h"
#include "CtProgressBarPopup.h"
#include "CtThemeMgr.h"
#include "CtString.h"

CtProgressBarPopup::CtProgressBarPopup(int totalCount, const char* description, bool timerUpdate)
{
	__totalCount = totalCount;
	__description = description;
	__progressbar = NULL;
	__contentLayout = NULL;
	__timerUpdate = timerUpdate;
	__remainCount = 0;
	__statusCount = 0;

	__timer = ecore_timer_add(0.1, [](void* data)->Eina_Bool{
		CtProgressBarPopup* p = static_cast<CtProgressBarPopup*>(data);

		char percent[CT_TEXT_SHORT_LEN] = {0,};
		char percentTotal[CT_TEXT_SHORT_LEN] = {0, };
		double value = 0.0;

		if(p->__remainCount > 0 ){
			value = (double)(p->__totalCount - p->__remainCount) / (double)(p->__totalCount);
			snprintf(percent, sizeof(percent), "%d%%", (int)(100.0 * (double)(p->__totalCount- p->__remainCount)/(double)(p->__totalCount)));
			snprintf(percentTotal, sizeof(percentTotal), "%d/%d", p->__totalCount- p->__remainCount, p->__totalCount);
		}
		else if(p->__statusCount >0){
			value = (double)(p->__statusCount) / (double)(p->__totalCount);
			snprintf(percent, sizeof(percent), "%d%%", (int)(100.0 * (double)(p->__statusCount)/(double)(p->__totalCount)));
			snprintf(percentTotal, sizeof(percentTotal), "%d/%d", p->__statusCount, p->__totalCount);
		}
		else
			return ECORE_CALLBACK_RENEW;

		elm_progressbar_value_set(p->__progressbar, value);
		elm_object_part_text_set(p->__progressbar, "elm.text.bottom.left", percent);
		elm_object_part_text_set(p->__progressbar, "elm.text.bottom.right", percentTotal);

		if (value == 1)
			return ECORE_CALLBACK_CANCEL;

		return ECORE_CALLBACK_RENEW;
		}, this);

}

Evas_Object* CtProgressBarPopup::__createContent(Evas_Object *parent)
{
	WENTER();

	char percentTotal[CT_TEXT_SHORT_LEN] = {0, };
	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	/* layout */
	__contentLayout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->setObjectTheme(__contentLayout);
	elm_layout_file_set(__contentLayout, contactsEdjePath.c_str(), "popup_text_progressbar_view_layout");
	evas_object_size_hint_weight_set(__contentLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* progressbar */
	__progressbar = elm_progressbar_add(__contentLayout);
	evas_object_size_hint_align_set(__progressbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(__progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	snprintf(percentTotal, sizeof(percentTotal), "%d/%d", 0, __totalCount);
	elm_object_part_text_set(__progressbar, "elm.text.bottom.left", "0%");
	elm_object_part_text_set(__progressbar, "elm.text.bottom.right", percentTotal);

	elm_progressbar_pulse(__progressbar, EINA_TRUE);
	elm_object_signal_emit(__progressbar, "elm,units,show", "elm");
	elm_object_part_content_set(__contentLayout, "progressbar", __progressbar);
	elm_object_content_set(parent, __contentLayout);

	return __contentLayout;
}

Evas_Object* CtProgressBarPopup::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	setContent([this](Evas_Object* parent)->Evas_Object* {
		return __createContent(parent);
	});

	setTitle(__description.c_str());
	return WPopup::onCreate(parent, viewParam);
}
void CtProgressBarPopup::setProgressRemainCount(int count)
{
	char percent[CT_TEXT_SHORT_LEN] = {0,};
	char percentTotal[CT_TEXT_SHORT_LEN] = {0, };
	double value = 0.0;

	if(__timerUpdate){
		__remainCount = count;
	}
	else {
		value = (double)(__totalCount-count) / (double)__totalCount;
		elm_progressbar_value_set(__progressbar, value);
		snprintf(percent, sizeof(percent), "%d%%", (int)(100.0 * (double)(__totalCount-count)/(double)__totalCount));
		snprintf(percentTotal, sizeof(percentTotal), "%d/%d", count, __totalCount);
		elm_object_part_text_set(__progressbar, "elm.text.bottom.left", percent);
		elm_object_part_text_set(__progressbar, "elm.text.bottom.right", percentTotal);
	}
}

void CtProgressBarPopup::setProgressStatusCount(int count)
{
	char percent[CT_TEXT_SHORT_LEN] = {0,};
	char percentTotal[CT_TEXT_SHORT_LEN] = {0, };
	double value = 0.0;

	if(__timerUpdate)
		__statusCount = count;
	else {
		value = (double)count / (double)__totalCount;
		elm_progressbar_value_set(__progressbar, value);
		snprintf(percent, sizeof(percent), "%d%%", (int)(100.0 * (double)count/(double)__totalCount));
		snprintf(percentTotal, sizeof(percentTotal), "%d/%d", count, __totalCount);
		elm_object_part_text_set(__progressbar, "elm.text.bottom.left", percent);
		elm_object_part_text_set(__progressbar, "elm.text.bottom.right", percentTotal);
	}
}


void CtProgressBarPopup::onDestroy()
{
	WENTER();

	if (__timer){
		ecore_timer_del(__timer);
		__timer = NULL;
	}
}

CtProgressBarPopup::~CtProgressBarPopup()
{

}

