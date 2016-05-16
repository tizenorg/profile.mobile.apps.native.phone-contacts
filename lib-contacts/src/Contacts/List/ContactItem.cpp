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
#include "Contacts/Model/ContactData.h"

#include "Ui/Thumbnail.h"
#include <app_i18n.h>

using namespace Contacts::List;
using namespace Contacts::Model;

ContactItem::ContactItem(ContactData &contactData)
	: m_ContactData(contactData)
{
}

ContactData &ContactItem::getContactData() const
{
	return m_ContactData;
}

void ContactItem::update(int changes)
{
	if (changes & ContactData::ChangedName) {
		elm_genlist_item_fields_update(getObjectItem(),
			PART_CONTACT_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}

	if (changes & ContactData::ChangedImage) {
		elm_genlist_item_fields_update(getObjectItem(),
			PART_CONTACT_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
}

ContactData &ContactItem::getContactData()
{
	return m_ContactData;
}

char *ContactItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CONTACT_NAME) == 0) {
		const char *name = m_ContactData.getName();
		return strdup(name ? name : _("IDS_LOGS_MBODY_UNKNOWN"));
	}

	return nullptr;
}

Evas_Object *ContactItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_CONTACT_THUMBNAIL) == 0) {
		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
				m_ContactData.getImagePath());
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	} else if (strcmp(part, PART_CONTACT_CHECK) == 0) {
		return SelectItem::getContent(parent, part);
	}

	return nullptr;
}

Ux::SelectResult ContactItem::getDefaultResult() const
{
	return { 0, &m_ContactData };
}
