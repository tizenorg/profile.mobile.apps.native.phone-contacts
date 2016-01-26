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

#include "Contacts/List/PersonItem.h"
#include "Ui/Thumbnail.h"
#include <app_i18n.h>

using namespace Contacts::List;
using namespace Contacts::List::Model;

PersonItem::PersonItem(PersonPtr person, Mode mode)
	: m_Person(std::move(person)), m_ItemMode(mode), m_Checked(false)
{ }

void PersonItem::setMode(Mode mode)
{
	m_ItemMode = mode;
	elm_genlist_item_fields_update(getObjectItem(), PART_CHECK, ELM_GENLIST_ITEM_FIELD_CONTENT);
}

const Person &PersonItem::getPerson() const
{
	return *m_Person;
}

bool PersonItem::isChecked() const
{
	return m_Checked;
}

void PersonItem::setPerson(Model::PersonPtr person)
{
	m_Person = std::move(person);
}

void PersonItem::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

void PersonItem::onSelected()
{
	if (m_ItemMode == ModePick) {
		m_Checked = !m_Checked;
		elm_check_state_set(getCheck(), m_Checked);
	}

	if (m_OnSelected) {
		m_OnSelected();
	}
}

Elm_Check *PersonItem::getCheck() const
{
	return elm_object_item_part_content_get(getObjectItem(), PART_CHECK);
}

char *PersonItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_PERSON_NAME) == 0) {
		const char *name = m_Person->getName();
		return strdup(name ? name : _("IDS_LOGS_MBODY_UNKNOWN"));
	}

	return nullptr;
}

Evas_Object *PersonItem::getContent(Evas_Object *parent, const char *part)
{
	using Ui::Thumbnail;

	if (strcmp(part, PART_PERSON_THUMBNAIL) == 0) {
		Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
				m_Person->getImagePath());
		thumbnail->setSizeHint(true);
		return thumbnail->getEvasObject();
	} else if (m_ItemMode == ModePick && strcmp(part, PART_CHECK) == 0) {
		Elm_Check *check = elm_check_add(parent);
		elm_check_state_set(check, m_Checked);
		return check;
	}

	return nullptr;
}
