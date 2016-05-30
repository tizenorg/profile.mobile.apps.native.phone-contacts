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

#include "Contacts/List/VcardView.h"
#include "Contacts/List/ContactItem.h"
#include "Contacts/Model/ContactRecordData.h"
#include "Contacts/Settings/ImportController.h"
#include "Common/Strings.h"

#include "Ui/Genlist.h"
#include "Utils/Logger.h"

#include <app_i18n.h>
#include <notification.h>

#define NOTI_BUFFER_SIZE 1024

using namespace Contacts::Model;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace std::placeholders;

VcardView::VcardView(std::string vcardPath)
	: m_Genlist(nullptr), m_VcardPath(std::move(vcardPath)), m_Provider(m_VcardPath)
{
	auto strings = Common::getSelectViewStrings();

	setStrings(strings);
	setSelectMode(Ux::SelectMulti);
	setSelectCallback(std::bind(&VcardView::onSelectResult, this, _1));
}

Evas_Object *VcardView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	for (auto &&contact : m_Provider.getDataList()) {
		ContactItem *item = new ContactItem(static_cast<ContactRecordData &>(*contact));
		m_Genlist->insert(item);
		onItemInserted(item);
	}

	return m_Genlist->getEvasObject();
}

void VcardView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

bool VcardView::onSelectResult(Ux::SelectResults results)
{
	std::vector<contacts_record_h> records;
	for (auto &&result : results) {
		ContactRecordData *contact = (ContactRecordData *) result.value.data;
		records.push_back(contact->getRecord());
	}

	Settings::ImportController *importer = new Settings::ImportController(getEvasObject(),
			"IDS_PB_HEADER_IMPORT_CONTACTS_ABB2", records.size(), { m_VcardPath }, std::move(records));
	importer->setFinishCallback(std::bind(&VcardView::onImportFinished, this, importer));
	importer->run();

	return false;
}

void VcardView::onImportFinished(Settings::ImportController *importer)
{
	size_t count = importer->getTotalCount();
	int err = NOTIFICATION_ERROR_NONE;

	if (count > 1) {
		char buffer[NOTI_BUFFER_SIZE] = { 0, };
		snprintf(buffer, sizeof(buffer), _("IDS_PB_TPOP_PD_CONTACTS_IMPORTED"), count);
		err = notification_status_message_post(buffer);
	} else if (count == 1){
		err = notification_status_message_post(_("IDS_PB_TPOP_1_CONTACT_IMPORTED"));
	}
	WARN_IF_ERR(err, "notification_status_message_post() failed.");

	getPage()->close();
}
