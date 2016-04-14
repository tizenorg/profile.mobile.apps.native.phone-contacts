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

#include "Contacts/List/MyProfileItem.h"
#include "Ui/Thumbnail.h"
#include <app_i18n.h>

using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace std::placeholders;

MyProfileItem::MyProfileItem(MyProfilePtr myProfile)
	: m_MyProfile(std::move(myProfile))
{
	m_MyProfile->setUpdateCallback(std::bind(&MyProfileItem::onUpdated, this, _1));
}

MyProfile &MyProfileItem::getMyProfile() const
{
	return *m_MyProfile;
}

char *MyProfileItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_MY_PROFILE_NAME) == 0) {
		const char *name = m_MyProfile->getName();
		if (!(name && *name)) {
			name = _("IDS_PB_MBODY_SET_MY_PROFILE");
		}

		return strdup(name);
	}

	return nullptr;
}

Evas_Object *MyProfileItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_MY_PROFILE_THUMBNAIL) == 0) {
		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
				m_MyProfile->getImagePath());
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	}

	return nullptr;
}

void MyProfileItem::onUpdated(int changes)
{
	if (changes & MyProfile::ChangedName) {
		elm_genlist_item_fields_update(getObjectItem(),
			PART_MY_PROFILE_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}

	if (changes & MyProfile::ChangedImage) {
		elm_genlist_item_fields_update(getObjectItem(),
			PART_MY_PROFILE_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
}
