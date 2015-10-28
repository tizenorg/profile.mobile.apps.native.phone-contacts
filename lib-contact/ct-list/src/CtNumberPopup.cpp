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

#include "CtNumberPopup.h"
#include "CtListView.h"
#include "CtListDataPerson.h"
#include "ContactsDebug.h"

#include <contacts.h>

CtNumberPopup::CtNumberPopup(CtListDataPerson *personData)
	: m_PersonData(personData), m_CallItem(nullptr), m_MessageItem(nullptr)
{
}

void CtNumberPopup::setOnResult(ResultCb callback)
{
	m_OnResult = std::move(callback);
}

Evas_Object *CtNumberPopup::onCreate(Evas_Object *parent, void *param)
{
	setTitle(m_PersonData->getDataString());
	setContent(std::bind(&CtNumberPopup::createGenlist, this, std::placeholders::_1));
	return WPopup::onCreate(parent, param);
}

Evas_Object *CtNumberPopup::createGenlist(Evas_Object *parent)
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->func.text_get = CtNumberPopup::onItemTextGet;

	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);

	m_CallItem = elm_genlist_item_append(genlist, itc, "IDS_PB_OPT_VOICE_CALL", NULL, ELM_GENLIST_ITEM_NONE,
			&CtNumberPopup::onItemSelect, this);
	m_MessageItem = elm_genlist_item_append(genlist, itc, "IDS_PB_OPT_MESSAGE", NULL, ELM_GENLIST_ITEM_NONE,
			&CtNumberPopup::onItemSelect, this);

	elm_genlist_item_class_free(itc);

	return genlist;
}

char *CtNumberPopup::onItemTextGet(void *data, Evas_Object *obj, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(dgettext(PACKAGE, (char*) data));
	}

	return NULL;
}

void CtNumberPopup::onItemSelect(void *data, Evas_Object *obj, void *event_info)
{
	CtNumberPopup *popup = (CtNumberPopup*) data;
	elm_genlist_item_selected_set((Elm_Object_Item*) event_info, EINA_TRUE);

	if (popup->m_OnResult) {
		CtListResultType type = (event_info == popup->m_CallItem
				? LIST_RESULT_NUMBER_FOR_CALL
				: LIST_RESULT_NUMBER_FOR_MESSAGE);

		popup->m_PersonData->setDataType(type);
		popup->m_OnResult(popup->m_PersonData);
	}

	popup->destroy();
}
