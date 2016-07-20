/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Ui/Genlist.h"
#include "Ui/GenGroupItem.h"

using namespace Ui;

Genlist::Genlist()
	: GenContainer(TypeGenlist)
{
}

void Genlist::onCreated()
{
	Evas_Object *genlist = getEvasObject();
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	evas_object_smart_callback_add(genlist, "expanded",
			(Evas_Smart_Cb) &Genlist::onItemExpanded, this);
	evas_object_smart_callback_add(genlist, "contracted",
			(Evas_Smart_Cb) &Genlist::onItemContracted, this);
}

void Genlist::onItemExpanded(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item && item->isGroupItem()) {
		GenGroupItem *groupItem = dynamic_cast<GenGroupItem *>(item);
		if (groupItem) {
			groupItem->onExpanded(true);
		}
	}
}

void Genlist::onItemContracted(void *data, Evas_Object *obj, Elm_Object_Item *objectItem)
{
	GenItem *item = (GenItem *) elm_object_item_data_get(objectItem);
	if (item && item->isGroupItem()) {
		GenGroupItem *groupItem = dynamic_cast<GenGroupItem *>(item);
		if (groupItem) {
			groupItem->onExpanded(false);
		}
	}
}
