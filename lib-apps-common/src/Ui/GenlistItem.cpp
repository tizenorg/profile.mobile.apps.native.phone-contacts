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

#include "Ui/GenlistItem.h"
#include "Ui/GenlistGroupItem.h"

using namespace Ui;

GenlistItem::GenlistItem()
	: GenItem(GenContainer::TypeGenlist), m_IsFocusPending(false)
{
}

GenlistGroupItem *GenlistItem::getParentItem() const
{
	Elm_Object_Item *objectItem = elm_genlist_item_parent_get(getObjectItem());
	GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
	return dynamic_cast<GenlistGroupItem *>(item);
}

void GenlistItem::focus(Elm_Genlist_Item_Scrollto_Type position, bool isAnimated)
{
	scrollTo(position, isAnimated);
	if (isRealized()) {
		onFocused();
	} else {
		m_IsFocusPending = true;
	}
}

void GenlistItem::onRealized()
{
	if (m_IsFocusPending) {
		onFocused();
		m_IsFocusPending = false;
	}
}
