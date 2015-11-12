/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Phone/Dialer/AddNumberPopup.h"
#include "App/AppControlRequest.h"
#include "Ui/Editfield.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

using namespace App;
using namespace Phone::Dialer;

AddNumberPopup::AddNumberPopup(std::string number)
	: m_Number(std::move(number))
{
}

void AddNumberPopup::onCreated()
{
	elm_popup_orient_set(getEvasObject(), ELM_POPUP_ORIENT_CENTER);
	setTitle("IDS_KPD_BUTTON_ADD_TO_CONTACTS_ABB2");
	setContent(createGenlist(getEvasObject()));
}

Evas_Object *AddNumberPopup::createGenlist(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);

	Elm_Genlist_Item_Class *itc = createItemClass();

	elm_genlist_item_append(genlist, itc, "IDS_LOGS_BUTTON_CREATE_CONTACT_ABB",
			NULL, ELM_GENLIST_ITEM_NONE, launchContactCreate, this);
	elm_genlist_item_append(genlist, itc, "IDS_LOGS_BUTTON_UPDATE_CONTACT_ABB2",
			NULL, ELM_GENLIST_ITEM_NONE, launchContactUpdate, this);

	elm_genlist_item_class_free(itc);
	return genlist;
}

Elm_Genlist_Item_Class *AddNumberPopup::createItemClass()
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	RETVM_IF(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = getItemText;
	return itc;
}

char *AddNumberPopup::getItemText(void *data, Evas_Object *obj, const char *part)
{
	RETVM_IF(!data, NULL, "data = NULL");
	const char *text = (const char *)data;

	if (!strcmp(part, "elm.text")) {
		return strdup(_(text));
	}

	return NULL;
}

void AddNumberPopup::launchContactCreate(void *data,
		Evas_Object *obj, void *event_info)
{
	RETM_IF(!data, "data = NULL");
	AddNumberPopup *popup = (AddNumberPopup*)data;

	AppControl request = requestContactCreate(popup->m_Number.c_str());
	request.launch();
	request.detach();

	delete popup;
}

void AddNumberPopup::launchContactUpdate(void *data,
		Evas_Object *obj, void *event_info)
{
	RETM_IF(!data, "data = NULL");
	AddNumberPopup *popup = (AddNumberPopup*)data;

	AppControl request = requestContactEdit(0, popup->m_Number.c_str());
	request.launch();
	request.detach();

	delete popup;
}
