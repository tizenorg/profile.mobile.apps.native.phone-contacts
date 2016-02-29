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

#include "Contacts/Common/SelectAllItem.h"
#include <app_i18n.h>

using namespace Contacts;

char *SelectAllItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_MBODY_SELECT_ALL"));
	}

	return nullptr;
}

Evas_Object *SelectAllItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.swallow.end") == 0) {
		return GenlistCheckItem::getContent(parent, part);
	}

	return nullptr;
}
