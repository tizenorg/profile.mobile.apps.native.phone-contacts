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

#include "Contacts/List/PersonGroupItem.h"
#include "Utils/UniString.h"

using namespace Contacts::List;
using namespace Utils;

PersonGroupItem::PersonGroupItem(UniString title)
	: m_Title(std::move(title)), m_IndexItem(nullptr)
{
}

PersonGroupItem::~PersonGroupItem()
{
	elm_object_item_del(m_IndexItem);
}

const UniString &PersonGroupItem::getTitle() const
{
	return m_Title;
}

Elm_Object_Item *PersonGroupItem::getIndexItem() const
{
	return m_IndexItem;
}

void PersonGroupItem::setIndexItem(Elm_Object_Item *item)
{
	m_IndexItem = item;
	elm_object_item_data_set(m_IndexItem, this);
}

char *PersonGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(m_Title.getUtf8Str().c_str());
	}

	return nullptr;
}
