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

#include "Contacts/Details/FieldItem.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Common/Strings.h"

#include <app_i18n.h>

using namespace Contacts;
using namespace Contacts::Details;
using namespace Contacts::Model;
using namespace std::placeholders;

#define DATE_BUFFER_SIZE 32

FieldItem::FieldItem(ContactObject &object)
	: m_Object(object), m_Field(*object.getField(0)),
	  m_SelectMode(SelectNone), m_ResultType(ResultNone)
{
	m_Object.setUpdateCallback(std::bind(&FieldItem::onFieldUpdated, this, _1, _2));
}

void FieldItem::setSelectMode(SelectMode mode, ResultType type)
{
	m_SelectMode = mode;
	m_ResultType = type;
	elm_genlist_item_fields_update(getObjectItem(), "*", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void FieldItem::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

void FieldItem::setRemoveCallback(RemoveCallback callback)
{
	m_OnRemove = std::move(callback);
}

ContactObject &FieldItem::getObject() const
{
	return m_Object;
}

ContactField &FieldItem::getField() const
{
	return m_Field;
}

SelectMode FieldItem::getSelectMode() const
{
	return m_SelectMode;
}

ResultType FieldItem::getResultType() const
{
	return m_ResultType;
}

Elm_Genlist_Item_Class *FieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
	return &itc;
}

char *FieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		switch (m_Field.getType()) {
			case TypeText:
			{
				const char *value = m_Field.cast<ContactTextField>().getValue();
				return value ? strdup(value) : nullptr;
			}
			case TypeDate:
			{
				tm date = m_Field.cast<ContactDateField>().getValue();
				char buffer[DATE_BUFFER_SIZE];
				strftime(buffer, sizeof(buffer), "%x", &date);
				return strdup(buffer);
			}
			default:
				return nullptr;
		}
	} else if (strcmp(part, "elm.text.sub") == 0) {
		const char *name = Common::getContactFieldName(ContactFieldId(m_Object.getId()));
		return name ? strdup(_(name)) : nullptr;
	}

	return nullptr;
}

void FieldItem::onSelected()
{
	if (m_SelectMode == SelectSingle) {
		onSelected(getObject().getSubType());
	}
}

void FieldItem::onSelected(unsigned resultType)
{
	if (m_OnSelected) {
		m_OnSelected({ resultType, getObject().getRecordId()});
	}
}

void FieldItem::onFieldUpdated(ContactField &field, contacts_changed_e change)
{
	if (&field == &m_Field) {
		elm_genlist_item_fields_update(getObjectItem(), "elm.text", ELM_GENLIST_ITEM_FIELD_TEXT);
	} else if (&field == &m_Object) {
		if (change == CONTACTS_CHANGE_DELETED || field.isEmpty()) {
			if (m_OnRemove) {
				m_OnRemove(this);
			}
		}
	}
}
