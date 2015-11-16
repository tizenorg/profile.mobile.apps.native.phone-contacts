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

ContactItem::ContactItem(contacts_record_h record)
	: m_Record(record)
{
}

ContactItem::~ContactItem()
{
	contacts_record_destroy(m_Record, true);
}

char *ContactItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CONTACT_NAME) == 0) {
		char *name = nullptr;
		contacts_record_get_str(m_Record, _contacts_person.display_name, &name);
		return name;
	}

	return nullptr;
}

Evas_Object *ContactItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_CONTACT_THUMBNAIL) == 0) {
		char *path = nullptr;
		contacts_record_get_str_p(m_Record, _contacts_person.image_thumbnail_path, &path);

		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall, path);
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	}

	return nullptr;
}
