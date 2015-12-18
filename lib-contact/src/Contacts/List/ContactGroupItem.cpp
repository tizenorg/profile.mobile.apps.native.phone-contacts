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
#include "Utils/UniString.h"

using namespace Contacts::List;
using namespace Utils;

ContactGroupItem::ContactGroupItem(const char *title, Elm_Object_Item *indexItem)
	: m_UniTitle(nullptr), m_IndexItem(indexItem)
{
	if (title) {
		m_Title = title;
	}
}

const UniString &ContactGroupItem::getTitle() const
{
	if (!m_UniTitle) {
		m_UniTitle = new UniString(m_Title.c_str());
	}

	return *m_UniTitle;
}

Elm_Object_Item *ContactGroupItem::getIndexItem() const
{
	return m_IndexItem;
}

char *ContactGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(m_Title.c_str());
	}

	return nullptr;
}
