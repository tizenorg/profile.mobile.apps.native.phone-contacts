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

#include "Contacts/Input/ContactCompoundFieldControl.h"
#include "Contacts/Model/ContactCompoundObject.h"
#include "Contacts/Common/Strings.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactCompoundFieldControl::ContactCompoundFieldControl(Ui::GenlistItem *parent,
		ContactCompoundObject &object)
	: ContactTextFieldControl(parent, *object.getField<ContactTextField>(0)),
	  m_Object(object), m_IsCompoundMode(true)
{
}

void ContactCompoundFieldControl::setCompoundMode(bool isEnabled)
{
	if (isEnabled == m_IsCompoundMode) {
		return;
	}

	save();
	m_IsCompoundMode = isEnabled;
	updateEntryLayout();
	update();
}

void ContactCompoundFieldControl::updateEntryLayout()
{
	if (m_IsCompoundMode) {
		setGuideText(Common::getContactFieldName(ContactFieldId(m_Object.getId())));
		elm_entry_autocapital_type_set(getEntry(), ELM_AUTOCAPITAL_TYPE_WORD);
	} else {
		ContactTextFieldControl::updateEntryLayout();
		elm_entry_autocapital_type_set(getEntry(), ELM_AUTOCAPITAL_TYPE_SENTENCE);
	}
}

std::string ContactCompoundFieldControl::getFieldValue() const
{
	return m_IsCompoundMode ? m_Object.getValue() : ContactTextFieldControl::getFieldValue();
}

void ContactCompoundFieldControl::setFieldValue(std::string value)
{
	if (m_IsCompoundMode) {
		m_Object.setValue(std::move(value));
	} else {
		ContactTextFieldControl::setFieldValue(std::move(value));
	}
}
