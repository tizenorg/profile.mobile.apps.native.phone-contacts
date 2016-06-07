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

#include "Logs/List/LogItem.h"
#include "Logs/Model/Log.h"
#include "Logs/Model/LogGroup.h"
#include "Logs/Common/Utils.h"
#include "Logs/Details/DetailsView.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Navigator.h"
#include "Ui/Scale.h"
#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "LogItemLayout.h"
#include "LogPath.h"

#include <app_i18n.h>
#include <time.h>

#define BUFFER_SIZE             32
#define LOG_TYPE_SIZE           50
#define LOG_INFO_BG_SIZE        100
#define LOG_TIME_TEXT_SIZE      22

#define PART_LOG_NAME           "elm.text"
#define PART_LOG_NUMBER         "elm.text.sub"
#define PART_LOG_COUNT          "elm.text.end"

#define PART_PERSON_THUMBNAIL   "elm.swallow.icon"
#define PART_END                "elm.swallow.end"

using namespace Ui;
using namespace Logs::Details;
using namespace Logs::List;
using namespace Logs::Model;

namespace
{
	const std::string layoutPath = App::getResourcePath(LOG_ITEM_LAYOUT_EDJ);
}

LogItem::LogItem(LogGroup *group)
	: m_Group(group)
{
	setUpdateCallback();
}

void LogItem::setDeleteCallback(DeleteCallback callback)
{
	m_OnDelete = std::move(callback);
}

LogGroup *LogItem::getGroup() const
{
	return m_Group;
}

char *LogItem::getText(Evas_Object *parent, const char *part)
{
	const Log *log = m_Group->getLogList().back();
	const char *name = log->getName();
	const char *number = log->getNumber();

	if (name == nullptr) {
		name = number;
		number = _("IDS_LOGS_SBODY_UNSAVED_M_STATUS");
	} else if (strcmp(name, number) == 0){
		number = _("IDS_LOGS_SBODY_SAVED_M_STATUS");
	}

	if (strcmp(part, PART_LOG_NAME) == 0) {
		return strdup(name);
	} else if (strcmp(part, PART_LOG_NUMBER) == 0) {
		return strdup(number);
	} else if (strcmp(part, PART_LOG_COUNT) == 0) {
		if (m_Group->getLogList().size() == 1) {
			return nullptr;
		}
		char buffer[BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), "(%zu)", m_Group->getLogList().size());
		return strdup(buffer);
	} else if (strcmp(part, PART_LOG_TIME) == 0) {
		char buffer[BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), "<font_size=%d>%s</font_size>", LOG_TIME_TEXT_SIZE, Logs::Common::formatTime(log->getTime()).c_str());
		return strdup(buffer);
	}

	return nullptr;
}

Evas_Object *LogItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_PERSON_THUMBNAIL) == 0) {
		return createThumbnail(parent);
	} else if (strcmp(part, PART_END) == 0) {
		if (getSelectMode() == Ux::SelectNone) {
			Evas_Object *icon = elm_image_add(parent);
			elm_image_file_set(icon, layoutPath.c_str(), ICON_INFO);
			evas_object_size_hint_min_set(icon, getScaledValue(LOG_TYPE_SIZE), getScaledValue(LOG_INFO_BG_SIZE));
			evas_object_propagate_events_set(icon, EINA_FALSE);
			evas_object_smart_callback_add(icon, "clicked",
				(Evas_Smart_Cb) makeCallback(&LogItem::onInfoIconPressed), this);
			return icon;
		} else {
			return SelectItem::getContent(parent, part);
		}
	}

	return nullptr;
}

Ux::SelectResult LogItem::getDefaultResult() const
{
	return { 0, m_Group };
}

void LogItem::onSelected()
{
	if (getSelectMode() != Ux::SelectNone) {
		SelectItem::onSelected();
		return;
	}

	const Log *log = m_Group->getLogList().back();
	App::AppControl appControl = App::requestTelephonyCall(log->getNumber());
	appControl.launch(nullptr, nullptr, false);
	appControl.detach();
}

void LogItem::onInfoIconPressed()
{
	Ui::Navigator *navigator = getParent()->findParent<Ui::Navigator>();
	if (!navigator) {
		return;
	}
	navigator->navigateTo(new DetailsView(getGroup()->getLogList().back()->getNumber()));
}

Evas_Object *LogItem::createThumbnail(Evas_Object *parent)
{
	using Ui::Thumbnail;

	const Log *log = m_Group->getLogList().back();
	Evas_Object *layout = createLayout(parent, LOG_ITEM);

	Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall, log->getImagePath());
	thumbnail->setSizeHint(true);

	elm_layout_content_set(layout, THUMBNAIL_PART, thumbnail->getEvasObject());
	elm_layout_content_set(layout, LOG_TYPE_PART, createIcon(layout, getImagePath(log->getType())));

	return layout;
}

Evas_Object *LogItem::createLayout(Evas_Object *parent, const char *layoutName)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), layoutName);

	return layout;
}

Evas_Object *LogItem::createIcon(Evas_Object *parent, const char *path)
{
	Evas_Object *icon = elm_image_add(parent);
	elm_image_file_set(icon, layoutPath.c_str(), path);

	static int size = getScaledValue(LOG_TYPE_SIZE);
	evas_object_size_hint_min_set(icon, size, size);
	return icon;
}

const char *LogItem::getImagePath(int type)
{
	const char *path = nullptr;
	switch (type) {
		case CONTACTS_PLOG_TYPE_VOICE_OUTGOING:
			path = ICON_OUTGOING;
			break;
		case CONTACTS_PLOG_TYPE_VOICE_INCOMING:
			path = ICON_INCOMING;
			break;
		case CONTACTS_PLOG_TYPE_VOICE_INCOMING_UNSEEN:
		case CONTACTS_PLOG_TYPE_VOICE_INCOMING_SEEN:
			path = ICON_MISSED;
			break;
		case CONTACTS_PLOG_TYPE_VOICE_REJECT:
			path = ICON_REJECTED;
			break;
		case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
			path = ICON_AUTO_REJECTED;
			break;
	}

	return path;
}

void LogItem::updateItem(int type)
{
	if (type & LogGroup::ChangePerson) {
		elm_genlist_item_fields_update(getObjectItem(), PART_LOG_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
		elm_genlist_item_fields_update(getObjectItem(), PART_LOG_NUMBER, ELM_GENLIST_ITEM_FIELD_TEXT);
		elm_genlist_item_fields_update(getObjectItem(), PART_PERSON_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
	if (type & LogGroup::ChangeName) {
		elm_genlist_item_fields_update(getObjectItem(), PART_LOG_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
	if (type & LogGroup::ChangeImage) {
		elm_genlist_item_fields_update(getObjectItem(), PART_PERSON_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
	if (type & LogGroup::ChangeCount) {
		elm_genlist_item_fields_update(getObjectItem(), PART_LOG_COUNT, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
	if (type & LogGroup::ChangeTime) {
		elm_genlist_item_fields_update(getObjectItem(), PART_LOG_TIME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

void LogItem::setUpdateCallback()
{
	m_Group->addChangeCallback([this](int type){
		if (type & LogGroup::ChangeRemoved) {
			if (m_OnDelete) {
				m_OnDelete(this);
			}
		} else {
			updateItem(type);
		}
	});
}
