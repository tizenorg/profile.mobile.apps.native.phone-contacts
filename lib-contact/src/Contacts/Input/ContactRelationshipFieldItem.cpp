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

#include "Contacts/Input/ContactRelationshipFieldItem.h"
#include "Contacts/Model/ContactTextField.h"

#include "App/Path.h"
#include "App/AppControlRequest.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	const std::string layoutPath = App::getResourcePath(INPUT_ITEM_LAYOUT_EDJ);
}

Elm_Genlist_Item_Class *ContactRelationshipFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(INPUT_REL_ITEM_STYLE);
	return &itc;
}

Evas_Object *ContactRelationshipFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_REL_ICON) == 0) {
		Evas_Object *button = elm_image_add(parent);
		elm_image_file_set(button, layoutPath.c_str(), GROUP_ICON_RELATIONSHIP);
		evas_object_smart_callback_add(button, "clicked",
				makeCallback(&ContactRelationshipFieldItem::onPickPressed), this);
		return button;
	}

	return ContactTypedFieldItem::getContent(parent, part);
}

void ContactRelationshipFieldItem::onPickPressed(Evas_Object *button, void *eventInfo)
{
	m_AppControl = App::requestContactPick(APP_CONTROL_SELECT_SINGLE, APP_CONTROL_RESULT_ID);
	m_AppControl.launch(makeCallbackWithLastParam(&ContactRelationshipFieldItem::onPickResult), this);
}

void ContactRelationshipFieldItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	int personId = atoi(App::getSingleExtraData(reply, APP_CONTROL_DATA_SELECTED).c_str());
	RETM_IF(personId < 1, "Invalid person ID.");

	char *name = nullptr;
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_person._uri, personId, &record);
	contacts_record_get_str_p(record, _contacts_person.display_name, &name);

	getField().cast<ContactTextField>().setValue(name);
	elm_genlist_item_fields_update(getObjectItem(), PART_MIDDLE, ELM_GENLIST_ITEM_FIELD_CONTENT);

	contacts_record_destroy(record, true);
}
