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

#include "Contacts/Input/ContactTypedFieldControl.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"
#include "Common/Strings.h"

#include "Ux/EditfieldPopup.h"
#include "Utils/Logger.h"

using namespace Common;
using namespace Contacts::Input;
using namespace Contacts::Model;

ContactTypedFieldControl::ContactTypedFieldControl(ContactEnumField &typeField,
		ContactTextField &labelField)
	: m_TypeField(typeField), m_LabelField(labelField)
{
}

void ContactTypedFieldControl::onCreated()
{
	int currentValue = m_TypeField.getValue();

	auto pairs = getEnumValueNames(EnumType(m_TypeField.getSubType()));
	for (auto &&pair : pairs) {
		addItem(pair.name, pair.value);

		if (pair.value == currentValue) {
			setText(pair.name);
		}
	}

	if (m_TypeField.hasCustomValue()) {
		setText(m_LabelField.getValue());
	}

	setSelectedCallback(std::bind(&ContactTypedFieldControl::onSelected, this,
			std::placeholders::_1));
}

bool ContactTypedFieldControl::onSelected(int value)
{
	if (value == m_TypeField.getCustomValue()) {
		auto popup = new Ux::EditfieldPopup();
		popup->setStrings({
			"IDS_PB_HEADER_ENTER_CUSTOM_LABEL_ABB",
			"IDS_PB_NPBODY_CUSTOM_TYPE_ABB",
			"IDS_PB_BUTTON_CREATE_ABB2",
			"IDS_PB_BUTTON_CANCEL" });
		popup->setResultCallback([this, value](const char *label) {
			m_TypeField.setValue(value);
			m_LabelField.setValue(label);
			setText(label);
		});

		popup->create(getEvasObject());
		return false;
	} else {
		if (m_TypeField.hasCustomValue()) {
			m_LabelField.reset();
		}

		m_TypeField.setValue(value);
		return true;
	}
}
