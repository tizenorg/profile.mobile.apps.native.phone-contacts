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

MyProfileItem::MyProfileItem(Model::MyProfilePtr myProfile)
	: m_MyProfile(std::move(myProfile))
{ }

const Model::MyProfile& MyProfileItem::getMyProfile() const
{
	return *m_MyProfile;
}

void MyProfileItem::setMyProfile(Model::MyProfilePtr myProfile)
{
	m_MyProfile = std::move(myProfile);
}

void MyProfileItem::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

void MyProfileItem::onSelected()
{
	if (m_OnSelected) {
		m_OnSelected();
	}
}

char *MyProfileItem::getText(Evas_Object *parent, const char *part)
{
	const char *itemText = nullptr;

	if (strcmp(part, PART_MY_PROFILE_NAME) == 0) {
		itemText = m_MyProfile->getName();

		if (!(itemText && *itemText)) {
			itemText = _("IDS_PB_MBODY_SET_MY_PROFILE");
		}
	}

	return itemText ? strdup(itemText) : nullptr;
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
