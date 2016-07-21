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

#include "Contacts/Groups/InputView.h"
#include "Contacts/Groups/Model/MembersProvider.h"

#include "App/AppControlRequest.h"
#include "Common/Database/RecordUtils.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/ProcessPopup.h"
#include "Utils/Thread.h"

#define BUFFER_SIZE 1024

using namespace Common::Database;
using namespace Contacts::Groups;
using namespace Contacts::List;
using namespace Ui;
using namespace Ux;
using namespace std::placeholders;

namespace
{
	struct ComposerData {
		Contacts::Groups::Model::MembersProvider::FilterType type;
		contacts_person_property_e propertyType;
		const char *uri;
		unsigned int propertyId;
		const char *scheme;
	} composerData[] = {
		/*ComposerMessage = */ { Contacts::Groups::Model::MembersProvider::FilterNumber,
			CONTACTS_PERSON_PROPERTY_NUMBER, _contacts_number._uri, _contacts_number.number, "sms:" },
		/*ComposerEmail   = */ { Contacts::Groups::Model::MembersProvider::FilterEmail,
			CONTACTS_PERSON_PROPERTY_EMAIL, _contacts_email._uri, _contacts_email.email, "mailto:" }
	};
}

MembersListView::MembersListView(int groupId)
	: ListView(new Model::MembersProvider(groupId)), m_GroupId(groupId)
{
	setAddButtonVisibility(false);
	setNoContentHelpText("IDS_PB_BODY_AFTER_YOU_ADD_CONTACTS_THEY_WILL_BE_SHOWN_HERE");
	for (size_t i = ListView::SectionFirst; i < ListView::SectionMax; ++i) {
		setSectionVisibility(static_cast<ListView::SectionId>(i), false);
	}
}

void MembersListView::onPageAttached(NavigatorPage *page)
{
	if (getSelectMode() == SelectNone) {
		page->setTitle(getTitle().c_str());
	}
}

void MembersListView::onMenuPressed()
{
	Menu *menu = new Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_EDIT", [this] {
		getNavigator()->navigateTo(new InputView(m_GroupId));
	});

	menu->addItem("IDS_PB_OPT_ADD", std::bind(&MembersListView::onAddSelected, this));

	if(!isListEmpty()) {
		menu->addItem("IDS_PB_OPT_REMOVE", std::bind(&MembersListView::onRemoveSelected, this));

		menu->addItem("IDS_KPD_OPT_SEND_MESSAGE_ABB", [this] {
			addSelectView(ComposerMessage);
		});

		menu->addItem("Send email", [this] {
			addSelectView(ComposerEmail);
		});
	}

	menu->show();
}

void MembersListView::onUpdateFinished()
{
	ListView::onUpdateFinished();

	if (getSelectMode() == SelectNone) {
		getPage()->setTitle(getTitle().c_str());
	}
}

std::string MembersListView::getTitle() const
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_group._uri, m_GroupId, &record);
	const char *name = getRecordStr(record, _contacts_group.name);

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

void MembersListView::onAddSelected()
{
	ListView *view = new ListView(new Model::MembersProvider(m_GroupId,
			Model::MembersProvider::ModeExclude));
	view->setSelectMode(SelectMulti);
	view->setNoContentHelpText("");
	view->setSectionVisibility(ListView::SectionFavorites, false);

	view->setSelectCallback([this, view](SelectResults results) {
		auto popup = ProcessPopup::create(view->getEvasObject(), "IDS_PB_TPOP_PROCESSING_ING");
		new Utils::Thread(std::bind(&MembersListView::selectMembers, this, std::move(results), EditAdd),
				[view, popup] {
					popup->destroy();
					view->getPage()->close();
				});
		return false;
	});

	getNavigator()->navigateTo(view);
}

void MembersListView::onRemoveSelected()
{
	setSelectMode(SelectMulti);
	setCancelCallback(std::bind(&MembersListView::onRemoveFinished, this));

	setSelectCallback([this](SelectResults results) {
		auto popup = ProcessPopup::create(getEvasObject(), "IDS_PB_TPOP_PROCESSING_ING");
		new Utils::Thread(std::bind(&MembersListView::selectMembers, this, std::move(results), EditRemove),
				[this, popup] {
					popup->destroy();
					onRemoveFinished();
				});
		return false;
	});
}

bool MembersListView::onRemoveFinished()
{
	setSelectMode(SelectNone);
	getPage()->setTitleVisibility(true);
	getPage()->setTitle(getTitle().c_str());
	setCancelCallback(nullptr);
	setSelectCallback(nullptr);
	return false;
}

void MembersListView::selectMembers(SelectResults results, EditType type)
{
	contacts_connect_on_thread();
	for (auto &&result : results) {
		contacts_record_h record = nullptr;
		contacts_db_get_record(_contacts_person._uri, result.value.id, &record);
		if (type == EditAdd) {
			contacts_group_add_contact(m_GroupId, getRecordInt(record,
					_contacts_person.display_contact_id));
		} else {
			contacts_group_remove_contact(m_GroupId, getRecordInt(record,
					_contacts_person.display_contact_id));
		}
		contacts_record_destroy(record, true);
	}
	contacts_disconnect_on_thread();
}

void MembersListView::addSelectView(ComposerType composerType)
{
	ListView *view = new ListView(new Model::MembersProvider(m_GroupId,
			Model::MembersProvider::ModeDefault, composerData[composerType].type));
	view->setSelectMode(SelectMulti);
	view->setNoContentHelpText("");
	view->setSectionVisibility(ListView::SectionFavorites, false);
	view->setSelectCallback(std::bind(&MembersListView::launchComposer,
			this, std::placeholders::_1, composerType));

	getNavigator()->navigateTo(view);
}

bool MembersListView::launchComposer(SelectResults results, ComposerType type)
{
	int length = results.size();
	std::vector<std::string> recipientsString(length);
	std::vector<const char *> recipients(length);

	int index = 0;
	for (auto &&result : results) {
		int id = 0;
		contacts_person_get_default_property(composerData[type].propertyType,
				result.value.id, &id);

		contacts_record_h record = nullptr;
		contacts_db_get_record(composerData[type].uri, id, &record);

		recipientsString[index] = getRecordStr(record, composerData[type].propertyId);
		recipients[index] = recipientsString[index].c_str();

		++index;
		contacts_record_destroy(record, true);
	}

	App::AppControl request = App::requestComposer(composerData[type].scheme, nullptr,
			nullptr, nullptr, recipients.data(), length);
	request.launch();
	request.detach();

	return true;
}
