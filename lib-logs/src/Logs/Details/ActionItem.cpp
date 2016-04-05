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

#include "Logs/Details/ActionItem.h"
#include "Logs/Model/Log.h"

#include "LogsDetailsItemLayout.h"
#include "LogsDetailsItemLayoutMetrics.h"
#include "DetailsItemLayoutMetrics.h"

#include "App/Path.h"
#include "Ui/Scale.h"
#include "Utils/Callback.h"

#include <app_i18n.h>

using namespace Logs::Details;
using namespace Logs::Model;
using namespace std::placeholders;

namespace
{
	struct {
		const char *icon;
		const char *operation;
		const char *scheme;
	} actions[] = {
		/* ActionCall    = */ { GROUP_ICON_CALL,     APP_CONTROL_OPERATION_CALL, "tel:" },
		/* ActionMessage = */ { GROUP_ICON_MESSAGE,  APP_CONTROL_OPERATION_COMPOSE, "sms:" },
	};

	struct {
		const char *name;
		int type;
	} numberTypes[] = {
		{ "IDS_PB_OPT_MOBILE",        CONTACTS_NUMBER_TYPE_CELL},
		{ "IDS_PB_OPT_HOME_ABB",      CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_HOME },
		{ "IDS_PB_OPT_WORK",          CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_WORK},
		{ "IDS_PB_OPT_MAIN",          CONTACTS_NUMBER_TYPE_MAIN},
		{ "IDS_PB_OPT_FAX_HWORK_ABB", CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_WORK},
		{ "IDS_PB_OPT_FAX_HHOME_ABB", CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_HOME},
		{ "IDS_PB_OPT_PAGER",         CONTACTS_NUMBER_TYPE_PAGER},
		{ "IDS_PB_OPT_OTHER",         CONTACTS_NUMBER_TYPE_OTHER},
		{ "IDS_PB_OPT_CUSTOM",        CONTACTS_NUMBER_TYPE_CUSTOM},
	};

	const std::string layoutPath = App::getResourcePath(LOGS_DETAILS_ITEM_LAYOUT_EDJ);
}

ActionItem::ActionItem(LogGroup *group)
		: m_Group(group)
{
	m_Log = m_Group->getLogList().back();
	m_GroupChangeCbHandle = m_Group->addChangeCallback(std::bind(&ActionItem::onGroupChanged, this, _1));
}

ActionItem::~ActionItem()
{
	if (m_Group) {
		m_Group->removeChangeCallback(m_GroupChangeCbHandle);
	}
}

Elm_Genlist_Item_Class *ActionItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(LOGS_DETAILS_ACTION_ITEM_STYLE);
	return &itc;
}

char *ActionItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_NUMBER_TYPE) == 0) {
		if (isSavedLog()) {
			return getStrNumberType();
		} else {
			return strdup(_("IDS_LOGS_SBODY_UNSAVED_M_STATUS"));
		}
	}

	return nullptr;
}

Evas_Object *ActionItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_ICON_CALL) == 0) {
		return createActionButton(parent, ActionCall);
	} else if (strcmp(part, PART_ICON_MESSAGE) == 0) {
		return createActionButton(parent, ActionMessage);
	} else if (strcmp(part, PART_NUMBER) == 0) {
		return createEntryNumber(parent);
	}
	return nullptr;
}

void ActionItem::onSelected()
{
	executeAction(ActionCall);
}

bool ActionItem::isSavedLog()
{
	return m_Log->getPersonId() > 0;
}

char *ActionItem::getStrNumberType()
{
	int type = m_Log->getNumberType();
	if (type == CONTACTS_NUMBER_TYPE_CUSTOM) {
		return strdup(m_Log->getNumberLabel().c_str());
	}
	for (auto &numberType : numberTypes) {
		if (numberType.type == type) {
			return strdup(_(numberType.name));
		}
	}
	return nullptr;
}

Evas_Object *ActionItem::createEntryNumber(Evas_Object *parent)
{
	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_editable_set(entry, EINA_FALSE);

	char *text = elm_entry_utf8_to_markup(m_Log->getNumber());
	elm_entry_entry_set(entry, text);
	free(text);

	return entry;
}

Evas_Object *ActionItem::createActionButton(Evas_Object *parent, ActionId actionId)
{
	static const int imageSize = Ui::getScaledValue(ACTION_BTN_WH);

	Evas_Object *image = elm_image_add(parent);
	elm_image_file_set(image, layoutPath.c_str(), actions[actionId].icon);
	evas_object_size_hint_min_set(image, imageSize, imageSize);
	evas_object_propagate_events_set(image, EINA_FALSE);
	evas_object_smart_data_set(image, (void *) actionId);
	evas_object_smart_callback_add(image, "clicked",
			makeCallback(&ActionItem::onButtonPressed), this);

	return image;
}

void ActionItem::executeAction(ActionId actionId)
{
	auto action = actions[actionId];

	std::string uri = action.scheme;
	const char *number = m_Log->getNumber();
	if (number) {
		uri.append(number);
	}

	m_AppControl = App::AppControl(action.operation, nullptr, uri.c_str());
	m_AppControl.launch();
}

void ActionItem::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	executeAction((ActionId) (long) evas_object_smart_data_get(button));
}

void ActionItem::onGroupChanged(int type)
{
	if (type & LogGroup::ChangeRemoved) {
		m_Group = nullptr;
	} else {
		m_Log = m_Group->getLogList().back();
		elm_genlist_item_fields_update(getObjectItem(), PART_NUMBER_TYPE, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}
