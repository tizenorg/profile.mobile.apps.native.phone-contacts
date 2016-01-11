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

#include "Contacts/Input/ContactImageFieldItem.h"
#include "Contacts/Input/ContactImageFieldControl.h"
#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

Elm_Genlist_Item_Class *ContactImageFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(INPUT_IMAGE_ITEM_STYLE);
	return &itc;
}

Evas_Object *ContactImageFieldItem::getContent(Evas_Object *parent, const char *part)
{
	auto control = new ContactImageFieldControl(&getField().cast<ContactTextField>());
	return control->create(parent);
}
