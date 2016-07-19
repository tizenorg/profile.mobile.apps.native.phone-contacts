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

#include "Contacts/Groups/MembersListView.h"

#include "Contacts/Groups/Model/Queries.h"
#include "Contacts/Groups/Model/MembersProvider.h"

#include "Common/Database/RecordUtils.h"

#define BUFFER_SIZE 1024

using namespace Contacts::Groups;
using namespace Contacts::List;

MembersListView::MembersListView(int groupId)
	: ListView(new Model::MembersProvider(groupId)), m_GroupId(groupId)
{
	setAddButtonVisibility(false);
	setNoContentHelpText("IDS_PB_BODY_AFTER_YOU_ADD_CONTACTS_THEY_WILL_BE_SHOWN_HERE");
	for (size_t i = ListView::SectionFirst; i < ListView::SectionMax; ++i) {
		setSectionVisibility(static_cast<ListView::SectionId>(i), false);
	}
}

void MembersListView::onPageAttached(Ui::NavigatorPage *page)
{
	if (getSelectMode() == Ux::SelectNone) {
		page->setTitle(getTitle().c_str());
	}
}

void MembersListView::onMenuPressed()
{
}

void MembersListView::onUpdateFinished()
{
	ListView::onUpdateFinished();

	if (getSelectMode() == Ux::SelectNone) {
		getPage()->setTitle(getTitle().c_str());
	}
}

std::string MembersListView::getTitle() const
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_group._uri, m_GroupId, &record);
	const char *name = Common::Database::getRecordStr(record, _contacts_group.name);

	char title[BUFFER_SIZE] = { 0, };
	int count = getProvider()->getDataList().size();
	if (count) {
		snprintf(title, sizeof(title), "%s (%d)", name, count);
	} else {
		snprintf(title, sizeof(title), "%s", name);
	}
	contacts_record_destroy(record, true);
	return std::string(title);
}
