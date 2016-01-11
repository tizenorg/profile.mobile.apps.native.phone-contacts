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

#include "Contacts/Input/ContactImageFieldControl.h"
#include "Contacts/Model/ContactTextField.h"
#include "Utils/Callback.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactImageFieldControl::ContactImageFieldControl(ContactTextField *field)
	: Thumbnail(Thumbnail::SizeMedium), m_Field(field)
{
}

void ContactImageFieldControl::onCreated()
{
	setImagePath(m_Field->getValue());
	evas_object_smart_callback_add(getImage(), "clicked",
			makeCallback(&ContactImageFieldControl::onImagePressed), this);
}

void ContactImageFieldControl::onImagePressed(Evas_Object *image, void *eventInfo)
{
	//TODO: Show popup
}
