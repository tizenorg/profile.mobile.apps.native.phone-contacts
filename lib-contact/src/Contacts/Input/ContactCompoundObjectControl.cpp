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

#include "Contacts/Input/ContactCompoundObjectControl.h"
#include "Contacts/Model/ContactCompoundObject.h"
#include "Contacts/Common/Strings.h"
#include "Utils/Callback.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactCompoundObjectControl::ContactCompoundObjectControl(ContactCompoundObject &object)
	: m_Object(object), m_IsEmpty(true)
{
}

void ContactCompoundObjectControl::save()
{
	char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(getEntry()));
	m_Object.setValue(text);
	free(text);
}

void ContactCompoundObjectControl::update()
{
	char *text = elm_entry_utf8_to_markup(m_Object.getValue().c_str());
	elm_entry_entry_set(getEntry(), text);
	free(text);
}

void ContactCompoundObjectControl::onCreated()
{
	setGuideText(Common::getContactFieldName(ContactFieldId(m_Object.getId())));

	Evas_Object *entry = getEntry();
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);
	evas_object_smart_callback_add(entry, "changed",
			makeCallback(&ContactCompoundObjectControl::onChanged), this);
	evas_object_smart_callback_add(entry, "unfocused",
			makeCallback(&ContactCompoundObjectControl::onUnfocused), this);

	update();
}

void ContactCompoundObjectControl::onChanged(Evas_Object *entry, void *eventInfo)
{
	if (m_IsEmpty != elm_entry_is_empty(entry)) {
		m_IsEmpty = !m_IsEmpty;
		save();
	}
}

void ContactCompoundObjectControl::onUnfocused(Evas_Object *entry, void *eventInfo)
{
	save();
}
