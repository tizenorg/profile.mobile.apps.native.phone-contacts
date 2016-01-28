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

#include "Contacts/Details/MultilineFieldItem.h"

using namespace Contacts::Details;

Elm_Genlist_Item_Class *MultilineFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("multiline_main.sub");
	return &itc;
}

char *MultilineFieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text.multiline") == 0) {
		return FieldItem::getText(parent, "elm.text");
	}

	return FieldItem::getText(parent, part);
}
