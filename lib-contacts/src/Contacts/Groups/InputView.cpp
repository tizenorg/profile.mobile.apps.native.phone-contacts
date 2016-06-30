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

#include "Contacts/Groups/InputView.h"

#include "App/Path.h"
#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Contacts/Groups/AddMembersItem.h"
#include "Contacts/Groups/NameItem.h"
#include "Contacts/Groups/RingtoneItem.h"
#include "GroupItemLayout.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"

#include <app_i18n.h>
#include <notification.h>

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace Common::Database;

InputView::InputView()
	: m_CancelButton(nullptr), m_DoneButton(nullptr), m_Genlist(nullptr),
	  m_AddMembersItem(nullptr), m_NameItem(nullptr), m_RingtoneItem(nullptr)
{
}

Evas_Object *InputView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(GROUP_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	m_NameItem = new NameItem();
	m_NameItem->setFilledCallback(std::bind(&InputView::enableDoneButton, this, std::placeholders::_1));
	m_Genlist->insert(m_NameItem);

	m_AddMembersItem = new AddMembersItem();
	m_Genlist->insert(m_AddMembersItem);

	m_RingtoneItem = new RingtoneItem();
	m_Genlist->insert(m_RingtoneItem);

	return m_Genlist->getEvasObject();
}

void InputView::onPageAttached(Ui::NavigatorPage *page)
{
	page->setTitle("IDS_PB_HEADER_CREATE");

	createPageButtons();
	page->setContent("title_left_btn", m_CancelButton);
	page->setContent("title_right_btn", m_DoneButton);
}

void InputView::createPageButtons()
{
	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, "IDS_TPLATFORM_ACBUTTON_CANCEL_ABB");
	evas_object_smart_callback_add(m_CancelButton, "clicked",
			makeCallback(&InputView::onCancelPressed), this);

	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_TPLATFORM_ACBUTTON_DONE_ABB");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&InputView::onDonePressed), this);
	elm_object_disabled_set(m_DoneButton, EINA_TRUE);
}

void InputView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	Ui::NavigatorPage *page = getPage();
	page->close();
}

void InputView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	if (isAlreadyExist()) {
		notification_status_message_post(_("IDS_PB_TPOP_GROUP_NAME_ALREADY_IN_USE"));
		return;
	}
	save();

	Ui::NavigatorPage *page = getPage();
	page->close();
}

void InputView::enableDoneButton(bool isEnableed)
{
	elm_object_disabled_set(m_DoneButton, !isEnableed);
}

bool InputView::isAlreadyExist()
{
	contacts_list_h groupList = nullptr;
	contacts_db_get_all_records(_contacts_group._uri, 0, 0, &groupList);
	for (auto &&record : makeRange(groupList)) {
		if (Utils::safeCmp(m_NameItem->getName().c_str(),
				getRecordStr(record, _contacts_group.name))) {
			return true;
		}
	}
	return false;
}

void InputView::save()
{
	contacts_record_h record = nullptr;
	contacts_record_create(_contacts_group._uri, &record);
	contacts_record_set_str(record, _contacts_group.name,
			m_NameItem->getName().c_str());
	contacts_record_set_str(record, _contacts_group.ringtone_path,
			m_RingtoneItem->getPath().c_str());

	int id = 0;
	contacts_db_insert_record(record, &id);
	contacts_record_destroy(record, true);
}
