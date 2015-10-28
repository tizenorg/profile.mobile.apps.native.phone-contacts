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

#include "ContactsAppControl.h"
#include "ContactsLocalization.h"
#include "PhDialerAddToContactsPopup.h"
#include "WDebugBase.h"

#define STYLE_ITEM		"type1"
#define PART_MAIN_TEXT	"elm.text"

PhDialerAddToContactsPopup::PhDialerAddToContactsPopup(std::string number)
	: m_Number(std::move(number))
{}

Evas_Object* PhDialerAddToContactsPopup::onCreate(Evas_Object *parent, void *param)
{
	setTitle(T_("IDS_KPD_BUTTON_ADD_TO_CONTACTS_ABB2"));
	setContent(std::bind(&PhDialerAddToContactsPopup::createGenlist, this, std::placeholders::_1));

	Evas_Object *popup = WPopup::onCreate(parent, param);
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);

	return popup;
}

Evas_Object *PhDialerAddToContactsPopup::createGenlist(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
	elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE );

	Elm_Genlist_Item_Class *itc = createItemClass();

	elm_genlist_item_append(genlist, itc, T_("IDS_LOGS_BUTTON_CREATE_CONTACT_ABB"),
			NULL, ELM_GENLIST_ITEM_NONE, launchContactCreate, this);
	elm_genlist_item_append(genlist, itc, T_("IDS_LOGS_BUTTON_UPDATE_CONTACT_ABB2"),
			NULL, ELM_GENLIST_ITEM_NONE, launchContactUpdate, this);

	elm_genlist_item_class_free(itc);
	return genlist;
}

Elm_Genlist_Item_Class *PhDialerAddToContactsPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = STYLE_ITEM;
	itc->func.text_get = getItemText;
	return itc;
}

char *PhDialerAddToContactsPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	WPRET_VM(!data, NULL, "data = NULL");
	const char *text = (const char *)data;

	if(!strcmp(part, PART_MAIN_TEXT)) {
		return strdup(text);
	}

	return NULL;
}

void PhDialerAddToContactsPopup::launchContactCreate(void *data,
		Evas_Object *obj, void *event_info)
{
	WPRET_M(!data, "data = NULL");
	PhDialerAddToContactsPopup *popup = (PhDialerAddToContactsPopup*)data;

	::launchContactCreate(popup->m_Number.c_str());
	popup->destroy();
}

void PhDialerAddToContactsPopup::launchContactUpdate(void *data,
		Evas_Object *obj, void *event_info)
{
	WPRET_M(!data, "data = NULL");
	PhDialerAddToContactsPopup *popup = (PhDialerAddToContactsPopup*)data;

	::launchContactEdit(0, popup->m_Number.c_str());
	popup->destroy();
}
