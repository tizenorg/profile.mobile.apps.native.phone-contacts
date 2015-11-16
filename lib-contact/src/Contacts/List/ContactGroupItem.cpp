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

#include "Contacts/List/ContactGroupItem.h"

using namespace Contacts::List;

namespace
{
	Elm_Genlist_Item_Class groupItemClass = Ui::GenlistItem::createItemClass("group_index");
}

ContactGroupItem::ContactGroupItem(const char *title)
	: GenlistItem(&groupItemClass, ELM_GENLIST_ITEM_GROUP)
{
	if (title) {
		m_Title = title;
	}
}

char *ContactGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(m_Title.c_str());
	}

	return nullptr;
}
