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

ContactGroupItem::ContactGroupItem(UniString title, Elm_Object_Item *indexItem)
	: m_Title(std::move(title)), m_IndexItem(indexItem)
{
}

const UniString &ContactGroupItem::getTitle() const
{
	return m_Title;
}

Elm_Object_Item *ContactGroupItem::getIndexItem() const
{
	return m_IndexItem;
}

char *ContactGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(m_Title.getUtf8Str().c_str());
	}

	return nullptr;
}
