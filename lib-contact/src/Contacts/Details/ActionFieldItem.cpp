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

#include "Contacts/Details/ActionFieldItem.h"
#include "Contacts/Model/ContactTextField.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/ListPopup.h"
#include "Ui/Scale.h"
#include "Utils/Callback.h"

#include "ContactsDetailsItemLayout.h"
#include "ContactsDetailsItemLayoutMetrics.h"

#define PART_ACTION_BUTTON1 "elm.swallow.icon.2"
#define PART_ACTION_BUTTON2 "elm.swallow.end"

using namespace Common;
using namespace Contacts::Details;
using namespace Contacts::Model;

namespace
{
	const std::string layoutPath = App::getResourcePath(CONTACTS_DETAILS_ITEM_LAYOUT_EDJ);
}

ActionFieldItem::ActionFieldItem(Model::ContactObject &object, ActionType actionType)
	: TypedFieldItem(object), m_ActionType(actionType)
{
}

Evas_Object *ActionFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (getSelectMode() == Ux::SelectNone) {
		if (strcmp(part, PART_ACTION_BUTTON1) == 0) {
			return createActionButton(parent, m_ActionType);
		} else if (m_ActionType == ActionCall && strcmp(part, PART_ACTION_BUTTON2) == 0) {
			return createActionButton(parent, ActionMessage);
		}
	}

	return TypedFieldItem::getContent(parent, part);
}

void ActionFieldItem::onSelected()
{
	if (getSelectMode() == Ux::SelectNone) {
		executeAction(m_ActionType);
	} else {
		TypedFieldItem::onSelected();
	}
}

void ActionFieldItem::onSelectModeChanged(Ux::SelectMode selectMode)
{
	Elm_Object_Item *item = getObjectItem();
	if (item) {
		elm_genlist_item_fields_update(item, PART_ACTION_BUTTON1, ELM_GENLIST_ITEM_FIELD_CONTENT);
		elm_genlist_item_fields_update(item, PART_ACTION_BUTTON2, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
}

Evas_Object *ActionFieldItem::createActionButton(Evas_Object *parent, ActionType actionType)
{
	static const int imageSize = Ui::getScaledValue(BTN_WH);
	static const char *icons[] = {
		/* ActionCall    = */ GROUP_ICON_CALL,
		/* ActionMessage = */ GROUP_ICON_MESSAGE,
		/* ActionEmail   = */ GROUP_ICON_EMAIL,
		/* ActionUrl     = */ GROUP_ICON_INTERNET
	};

	Evas_Object *image = elm_image_add(parent);
	elm_image_file_set(image, layoutPath.c_str(), icons[actionType]);
	evas_object_size_hint_min_set(image, imageSize, imageSize);
	evas_object_propagate_events_set(image, EINA_FALSE);
	evas_object_smart_data_set(image, (void *) actionType);
	evas_object_smart_callback_add(image, "clicked",
			makeCallback(&ActionFieldItem::onButtonPressed), this);

	return image;
}

void ActionFieldItem::executeAction(ActionType actionType)
{
	const char *value = getField().cast<ContactTextField>().getValue();
	m_AppControl = requestAction(actionType, value);
	m_AppControl.launch();
}

void ActionFieldItem::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	executeAction((ActionType) (long) evas_object_smart_data_get(button));
}
