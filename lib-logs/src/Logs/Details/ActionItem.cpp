/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "DetailsItemLayoutMetrics.h"

#include "App/Path.h"
#include "Ui/Menu.h"
#include "Ui/Scale.h"
#include "Utils/Callback.h"
#include "Common/Strings.h"

#include <app_i18n.h>

using namespace Logs::Details;
using namespace Logs::Model;
using namespace Common;
using namespace std::placeholders;

#define PART_NUMBER_TYPE     "elm.text.sub"
#define PART_NUMBER          "elm.text"
#define PART_ICON_CALL       "elm.swallow.icon.2"
#define PART_ICON_MESSAGE    "elm.swallow.end"
#define TEXT_BUFFER_SIZE 128
#define TAG_BACKING "<backing=on><backing_color=#00ddff99>"


namespace
{
	const std::string layoutPath = App::getResourcePath(LOGS_DETAILS_ITEM_LAYOUT_EDJ);
}

ActionItem::ActionItem(LogGroup *group)
	: m_Group(group)
	, m_IsSelecting(false)
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

void ActionItem::updateGroup(LogGroup *group)
{
	m_Group = group;
	if (m_Group) {
		m_Log = m_Group->getLogList().back();
		m_GroupChangeCbHandle = m_Group->addChangeCallback(std::bind(&ActionItem::onGroupChanged, this, _1));
		elm_genlist_item_fields_update(getObjectItem(), PART_NUMBER_TYPE, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

Elm_Genlist_Item_Class *ActionItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
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
	} else if (strcmp(part, PART_NUMBER) == 0) {
		if (m_IsSelecting) {
			char buffer[TEXT_BUFFER_SIZE];
			snprintf(buffer, sizeof(buffer), TAG_BACKING "%s", m_Log->getNumber());
			return strdup(buffer);
		}
		return Utils::safeDup(m_Log->getNumber());
	}

	return nullptr;
}

Evas_Object *ActionItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_ICON_CALL) == 0) {
		return createActionButton(parent, ActionCall);
	} else if (strcmp(part, PART_ICON_MESSAGE) == 0) {
		return createActionButton(parent, ActionMessage);
	}

	return nullptr;
}

void ActionItem::onSelected()
{
	executeAction(ActionCall);
}

bool ActionItem::onLongpressed()
{
	updateSelecting(true);

	auto menu = new Ui::Menu();
	menu->create(getParent()->getEvasObject());
	menu->addItem("IDS_TPLATFORM_OPT_COPY", [this] {
		const char *text = m_Log->getNumber();
		if (text) {
			elm_cnp_selection_set(getParent()->getEvasObject(), ELM_SEL_TYPE_CLIPBOARD,
					ELM_SEL_FORMAT_TEXT, text, strlen(text));
		}
	});

	int x, y, w, h;
	Evas_Object *rect = elm_object_item_track(getObjectItem());
	evas_object_geometry_get(rect, &x, &y, &w, &h);
	elm_object_item_untrack(getObjectItem());

	Evas_Object *obj = menu->getEvasObject();
	evas_object_smart_callback_add(obj, "dismissed",
			makeCallback(&ActionItem::onMenuDismissed), this);
	elm_ctxpopup_horizontal_set(obj, EINA_TRUE);
	elm_object_style_set(obj, "default");
	evas_object_move(obj, x, y + h / 2);
	evas_object_show(obj);

	return true;
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
	const char *name = _(getEnumValueName(EnumNumberType, type));
	return Utils::safeDup(name);
}

Evas_Object *ActionItem::createActionButton(Evas_Object *parent, ActionType actionType)
{
	static const int imageSize = Ui::getScaledValue(ACTION_BTN_WH);
	static const char *actionIcons[] = {
		/* ActionCall = */ GROUP_ICON_CALL,
		/* ActionMessage = */ GROUP_ICON_MESSAGE
	};

	Evas_Object *image = elm_image_add(parent);
	elm_image_file_set(image, layoutPath.c_str(), actionIcons[actionType]);
	evas_object_size_hint_min_set(image, imageSize, imageSize);
	evas_object_propagate_events_set(image, EINA_FALSE);
	evas_object_smart_data_set(image, (void *) actionType);
	evas_object_smart_callback_add(image, "clicked",
			makeCallback(&ActionItem::onButtonPressed), this);

	return image;
}

void ActionItem::executeAction(ActionType actionType)
{
	m_AppControl = requestAction(actionType, m_Log->getNumber());
	m_AppControl.launch();
}

void ActionItem::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	executeAction((ActionType) (long) evas_object_smart_data_get(button));
}

void ActionItem::onGroupChanged(int type)
{
	if (!(type & LogGroup::ChangeRemoved)) {
		m_Log = m_Group->getLogList().back();
		elm_genlist_item_fields_update(getObjectItem(), PART_NUMBER_TYPE, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

void ActionItem::onMenuDismissed(Evas_Object *obj, void *eventInfo)
{
	updateSelecting(false);
}

void ActionItem::updateSelecting(bool isSelecting)
{
	if (m_IsSelecting != isSelecting) {
		m_IsSelecting = isSelecting;
		elm_genlist_item_fields_update(getObjectItem(), "elm.text", ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}
