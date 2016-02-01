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

#include "Contacts/List/SelectAllItem.h"
#include <app_i18n.h>

#define PART_TEXT   "elm.text"
#define PART_CHECK  "elm.swallow.end"

using namespace Contacts::List;

char *SelectAllItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_TEXT) == 0) {
		return strdup(_("IDS_PB_MBODY_SELECT_ALL"));
	}

	return nullptr;
}

Evas_Object *SelectAllItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CHECK) == 0) {
		return GenlistCheckItem::getContent(parent, part);
	}

	return nullptr;
}
