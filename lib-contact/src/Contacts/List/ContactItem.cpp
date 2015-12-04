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

#include "Contacts/List/ContactItem.h"
#include "Ui/Thumbnail.h"

using namespace Contacts::List;
using namespace Contacts::List::Model;

ContactItem::ContactItem(ContactPtr contact)
	: m_Contact(std::move(contact))
{ }

ContactItem::~ContactItem() { }

const ContactPtr &ContactItem::getContact() const
{
	return m_Contact;
}

char *ContactItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CONTACT_NAME) == 0) {
		return strdup(m_Contact->getName());
	}

	return nullptr;
}

Evas_Object *ContactItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_CONTACT_THUMBNAIL) == 0) {
		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
				m_Contact->getPicturePath());
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	}

	return nullptr;
}
