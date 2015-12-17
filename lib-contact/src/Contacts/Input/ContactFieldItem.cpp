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

#include "Contacts/Input/ContactFieldItem.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Input/ContactDateFieldControl.h"
#include "Contacts/Input/ContactTextFieldControl.h"
#include "Ui/Editfield.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	Elm_Genlist_Item_Class itc = Ui::GenlistItem::createItemClass(INPUT_ITEM_STYLE);
}

ContactFieldItem::ContactFieldItem(ContactFieldPtr field)
	: GenlistItem(&itc), m_Field(std::move(field))
{
}

const ContactField &ContactFieldItem::getField() const
{
	return *m_Field;
}

Evas_Object *ContactFieldItem::getContent(Evas_Object *parent, const char *part)
{
	Ui::Control *control = nullptr;
	if (m_Field && strcmp(part, PART_MIDDLE) == 0) {
		switch (m_Field->getType()) {
			case TypeText:
				control = new ContactTextFieldControl(&m_Field->cast<ContactTextField>());
				break;
			case TypeDate:
				control = new ContactDateFieldControl(&m_Field->cast<ContactDateField>());
				break;
			default:
				break;
		}
	}

	return control ? control->create(parent) : nullptr;
}
