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

#include "Logs/Details/LogDetailItem.h"
#include "Logs/Model/Log.h"
#include "Logs/Common/Utils.h"
#include "LogItemLayout.h"
#include "LogPath.h"

#include "App/Path.h"
#include "Ui/Scale.h"

#include <app_i18n.h>
#define BUFFER_SIZE             64
#define LOG_TYPE_SIZE           45

#define PART_LOG_TIME           "elm.text"
#define PART_LOG_TYPE           "elm.text.sub"

#define PART_LOG_TYPE_ICON      "elm.swallow.icon"
#define PART_CHECK              "elm.swallow.end"

using namespace Ui;
using namespace Logs::Details;
using namespace Logs::Model;
using namespace Logs::Common;
using namespace Contacts;

namespace
{
	const std::string layoutPath = App::getResourcePath(LOG_ITEM_LAYOUT_EDJ);
}

LogDetailItem::LogDetailItem(Log *log)
	: m_Log(log)
{
}

char *LogDetailItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_LOG_TYPE) == 0) {
		return getTypeString(m_Log->getType(), m_Log->getDuration());
	} else if (strcmp(part, PART_LOG_TIME) == 0) {
		return strdup(formatTime(m_Log->getTime()).c_str());
	}

	return nullptr;
}

Evas_Object *LogDetailItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_LOG_TYPE_ICON) == 0) {
		return createIcon(parent, getImagePath(m_Log->getType()));
	} else if (strcmp(part, PART_CHECK) == 0) {
		return SelectItem::getContent(parent, part);
	}

	return nullptr;
}

void LogDetailItem::onSelectModeChanged(SelectMode selectMode)
{
	if (getSelectMode() == SelectNone) {
		elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	} else {
		elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_DEFAULT);
	}
}

SelectResult LogDetailItem::getSelectResult() const
{
	return { 0, m_Log };
}

Evas_Object *LogDetailItem::createIcon(Evas_Object *parent, const char *path)
{
	Evas_Object *icon = elm_image_add(parent);
	elm_image_file_set(icon, layoutPath.c_str(), path);

	static int size = getScaledValue(LOG_TYPE_SIZE);
	evas_object_size_hint_min_set(icon, size, size);
	return icon;
}

const char *LogDetailItem::getImagePath(int type)
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

char *LogDetailItem::getTypeString(int type, time_t duration)
{
	const char *str = nullptr;

	if (duration > 0) {
		switch (type) {
			case CONTACTS_PLOG_TYPE_VOICE_OUTGOING:
				str = "IDS_ST_BODY_OUTGOING_CALL";
				break;
			case CONTACTS_PLOG_TYPE_VOICE_INCOMING:
				str = "IDS_CALL_BODY_INCOMING_CALL";
				break;
		}

		if (str) {
			return appendDuration(str, duration);
		}
	}

	switch (type) {
		case CONTACTS_PLOG_TYPE_VOICE_INCOMING_UNSEEN:
		case CONTACTS_PLOG_TYPE_VOICE_INCOMING_SEEN:
			str = "IDS_LOGS_SBODY_MISSEDM_CALL_STATUS";
			break;
		case CONTACTS_PLOG_TYPE_VOICE_REJECT:
		case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
		default:
			str = "IDS_LOGS_SBODY_CANCELLED_M_STATUS";
			break;
	}

	return strdup(_(str));
}

char *LogDetailItem::appendDuration(const char *typeStr, time_t duration)
{
	char buffer[BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%s, %s", _(typeStr), formatDuration(*gmtime(&duration)).c_str());
	return strdup(buffer);
}
