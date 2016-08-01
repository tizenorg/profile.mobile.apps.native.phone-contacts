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

#include "Contacts/Groups/AddMembersItem.h"
#include "Contacts/Groups/NameItem.h"
#include "Contacts/Groups/RingtoneItem.h"
#include "GroupItemLayout.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"

#include <app_i18n.h>
#include <notification.h>

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;

InputView::InputView(int id)
	: m_Id(id), m_CancelButton(nullptr), m_DoneButton(nullptr), m_Genlist(nullptr),
	  m_AddMembersItem(nullptr), m_NameItem(nullptr), m_RingtoneItem(nullptr)
{
	if (m_Id) {
		contacts_db_get_record(_contacts_group._uri, m_Id, &m_Record);
	} else {
		contacts_record_create(_contacts_group._uri, &m_Record);
	}
}
InputView::~InputView()
{
	contacts_record_destroy(m_Record, true);
}

Evas_Object *InputView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(GROUP_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	m_NameItem = new NameItem(m_Record);
	m_NameItem->setFilledCallback(std::bind(&InputView::onNameFilled, this, std::placeholders::_1));
	m_Genlist->insert(m_NameItem);

	m_AddMembersItem = new AddMembersItem(m_Id);
	m_Genlist->insert(m_AddMembersItem);

	m_RingtoneItem = new RingtoneItem(m_Record);
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
	getPage()->close();
}

void InputView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	if (isAlreadyExists()) {
		notification_status_message_post(_("IDS_PB_TPOP_GROUP_NAME_ALREADY_IN_USE"));
		return;
	}
	save();

	getPage()->close();
}

void InputView::onNameFilled(bool isFilled)
{
	elm_object_disabled_set(m_DoneButton, !isFilled);
}

bool InputView::isAlreadyExists()
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_group._uri, &filter);
	contacts_filter_add_str(filter, _contacts_group.name,
			CONTACTS_MATCH_EXACTLY, m_NameItem->getName());

	if (m_Id) {
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_group.id, CONTACTS_MATCH_NOT_EQUAL, m_Id);
	}

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_group._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_filter_destroy(filter);

	int count = 0;
	contacts_db_get_count_with_query(query, &count);
	contacts_query_destroy(query);

	return count != 0;
}

void InputView::save()
{
	if (!m_Id) {
		contacts_db_insert_record(m_Record, &m_Id);
	}
	for (auto &&contactId : m_AddMembersItem->getMemberIdList()) {
		contacts_group_add_contact(m_Id, contactId);
	}

	contacts_db_update_record(m_Record);
}
