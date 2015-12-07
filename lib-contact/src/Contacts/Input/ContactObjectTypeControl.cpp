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

#include "Contacts/Input/ContactObjectTypeControl.h"
#include "Contacts/Input/ContactObjectCustomTypePopup.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Common/Strings.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactObjectTypeControl::ContactObjectTypeControl(ContactEnumField *typeField,
		ContactTextField *labelField)
	: m_TypeField(typeField), m_LabelField(labelField)
{
}

void ContactObjectTypeControl::onCreated()
{
	ContactEnumType enumType = (ContactEnumType) m_TypeField->getSubType();
	auto names = Common::getContactEnumValueNames(enumType);
	auto values = m_TypeField->getValues();
	int currentValue = m_TypeField->getValue();

	for (size_t i = 0; i < values.count(); ++i) {
		addItem(names[i], values[i]);

		if (values[i] == currentValue) {
			setText(names[i]);
		}
	}

	if (m_TypeField->hasCustomValue()) {
		setText(m_LabelField->getValue());
	}

	setSelectedCallback([this](int value) {
		return onSelected(value);
	});
}

bool ContactObjectTypeControl::onSelected(int value)
{
	if (value == m_TypeField->getCustomValue()) {
		auto popup = new ContactObjectCustomTypePopup();
		popup->setResultCallback([this, value](const char *label) {
			m_TypeField->setValue(value);
			m_LabelField->setValue(label);
			setText(label);
		});

		popup->create(getEvasObject());
		return false;
	} else {
		if (m_TypeField->hasCustomValue()) {
			m_LabelField->reset();
		}

		m_TypeField->setValue(value);
		return true;
	}
}
