/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Groups/AddMembersItem.h"

#include "Contacts/Groups/Model/MembersProvider.h"
#include "Contacts/List/ListView.h"
#include "GroupItemLayout.h"

#include "Common/Database/RecordUtils.h"
#include "Ui/Navigator.h"
#include "Ui/ProcessPopup.h"
#include "Utils/Logger.h"
#include "Utils/Thread.h"

#include <app_i18n.h>

#define BUFFER_SIZE 32

using namespace Common::Database;
using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace Contacts::List;

AddMembersItem::AddMembersItem(int groupId)
	: m_GroupId(groupId), m_Count(0)
{
}

const std::vector<int> &AddMembersItem::getMemberIdList() const
{
	return m_ContactIdList;
}

Elm_Genlist_Item_Class *AddMembersItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(GROUP_ADD_MEMBERS_ITEM_STYLE);
	return &itc;
}

char *AddMembersItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_ADD_MEMBERS_LABEL) == 0) {
		return strdup(_("IDS_PB_MBODY_ADD_MEMBERS_ABB"));
	} else if (strcmp(part, PART_GROUP_ADD_MEMBERS_COUNTER) == 0) {
		char counter[BUFFER_SIZE] = { 0, };
		snprintf(counter, sizeof(counter), "(%d)", m_Count);
		return strdup(counter);
	}
	return nullptr;
}

Evas_Object *AddMembersItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_ADD_MEMBERS_BUTTON) == 0) {
		Evas_Object *button = elm_button_add(parent);
		elm_object_style_set(button, "icon_expand_add");
		elm_object_focus_allow_set(button, EINA_FALSE);
		return button;
	}
	return nullptr;
}

void AddMembersItem::onSelected()
{
	Ui::Navigator *navigator = getParent()->findParent<Ui::Navigator>();
	if (!navigator) {
		return;
	}

	MembersProvider *provider = new MembersProvider(m_GroupId, MembersProvider::ModeExclude);
	m_Count = provider->getMembersCount();
	elm_genlist_item_fields_update(getObjectItem(),
			PART_GROUP_ADD_MEMBERS_COUNTER, ELM_GENLIST_ITEM_FIELD_TEXT);

	ListView *view = new ListView(provider);
	view->setSelectMode(Ux::SelectMulti);
	view->setSectionVisibility(ListView::SectionFavorites, false);
	view->setSelectCallback(std::bind(&AddMembersItem::addMembersProcessPopup, this, view,
			std::placeholders::_1));

	navigator->navigateTo(view);
}

bool AddMembersItem::addMembersProcessPopup(ListView *view, Ux::SelectResults results)
{
	auto popup = Ui::ProcessPopup::create(view->getEvasObject(), "IDS_PB_TPOP_PROCESSING_ING");
	new Utils::Thread(std::bind(&AddMembersItem::onAddingMembers, this, std::move(results)),
			std::bind(&AddMembersItem::onAddMembersFinished, this, view, popup));
	return false;
}

void AddMembersItem::onAddingMembers(Ux::SelectResults results)
{
	contacts_connect_on_thread();
	m_ContactIdList.clear();

	for (auto &&result : results) {
		contacts_record_h record = nullptr;
		contacts_db_get_record(_contacts_person._uri, result.value.id, &record);
		m_ContactIdList.push_back(getRecordInt(record, _contacts_person.display_contact_id));
		contacts_record_destroy(record, true);
	}
	m_Count += m_ContactIdList.size();
	contacts_disconnect_on_thread();
}

void AddMembersItem::onAddMembersFinished(ListView *view, Ui::ProcessPopup *popup)
{
	delete popup;
	view->getPage()->close();
	elm_genlist_item_fields_update(getObjectItem(),
			PART_GROUP_ADD_MEMBERS_COUNTER, ELM_GENLIST_ITEM_FIELD_TEXT);
}
