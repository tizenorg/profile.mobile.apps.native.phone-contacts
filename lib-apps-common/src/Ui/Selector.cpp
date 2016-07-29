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

#include "Ui/Selector.h"

using namespace Ui;

void Selector::setSelectEnabled(bool isEnabled)
{
	elm_object_disabled_set(getEvasObject(), !isEnabled);
}

void Selector::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

bool Selector::onSelected(Elm_Object_Item *item)
{
	void *data = elm_object_item_data_get(item);
	return !m_OnSelected || m_OnSelected(data);
}
