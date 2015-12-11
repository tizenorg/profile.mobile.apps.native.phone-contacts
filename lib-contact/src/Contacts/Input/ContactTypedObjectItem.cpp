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

#include "Contacts/Input/ContactTypedObjectItem.h"
#include "Contacts/Input/ContactObjectTypeControl.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactTypedObjectItem::ContactTypedObjectItem(Model::ContactFieldPtr object)
	: ContactObjectItem(std::move(object))
{
	m_TypeField = getTypedObject()->getTypeField();
	m_LabelField = getTypedObject()->getLabelField();
}

ContactTypedObject *ContactTypedObjectItem::getTypedObject() const
{
	return &getObject()->cast<ContactTypedObject>();
}

Evas_Object *ContactTypedObjectItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_LEFT) == 0) {
		Ui::Control *control = new ContactObjectTypeControl(
				&m_TypeField->cast<ContactEnumField>(),
				&m_LabelField->cast<ContactTextField>());
		return control->create(parent);
	} else {
		return ContactObjectItem::getContent(parent, part);
	}
}
