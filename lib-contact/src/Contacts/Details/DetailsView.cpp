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

#include "Contacts/Details/DetailsView.h"
#include "Contacts/Details/ActionFieldItem.h"
#include "Contacts/Details/BasicInfoItem.h"
#include "Contacts/Details/MultilineFieldItem.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Model/ContactArray.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/Popup.h"
#include "Utils/Logger.h"

#include "DetailsItemLayout.h"

using namespace Contacts::Details;
using namespace Contacts::Model;

namespace
{
	bool isFieldVisible[] = {
		/* [FieldFavorite]     = */ false,
		/* [FieldImage]        = */ false,
		/* [FieldName]         = */ false,
		/* [FieldPhoneticName] = */ false,
		/* [FieldCompany]      = */ false,
		/* [FieldNumber]       = */ true,
		/* [FieldEmail]        = */ true,
		/* [FieldAddress]      = */ true,
		/* [FieldUrl]          = */ true,
		/* [FieldMessenger]    = */ true,
		/* [FieldEvent]        = */ true,
		/* [FieldNote]         = */ true,
		/* [FieldNickname]     = */ true,
		/* [FieldRelationship] = */ true
	};
}

DetailsView::DetailsView(int recordId, Type type)
	: m_RecordId(recordId), m_Contact(ContactObjectType(type)),
	  m_Genlist(nullptr)
{
}

Evas_Object *DetailsView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(DETAILS_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	return m_Genlist->create(parent);
}

void DetailsView::onCreated()
{
	int err = m_Contact.initialize(m_RecordId);
	RETM_IF_ERR(err, "Contact::initialize() failed.");

	BasicInfoItem *item = new BasicInfoItem(m_Contact);
	m_Genlist->insert(item);

	for (auto &&field : m_Contact) {
		ContactFieldId fieldId = ContactFieldId(field.getId());
		if (!isFieldVisible[fieldId]) {
			continue;
		}

		switch (field.getType()) {
			case TypeArray:
				for (auto &&element : field.cast<ContactArray>()) {
					addFieldItem(element.cast<ContactObject>());
				}
				break;
			case TypeObject:
				if (!field.isEmpty()) {
					addFieldItem(field.cast<ContactObject>());
				}
				break;
			default:
				break;
		}
	}
}

void DetailsView::onPageAttached()
{
	/* FIXME: Disable naviframe item title by default */
	getPage()->setTitle(nullptr);
}

void DetailsView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_EDIT", [this] {
		auto type = (Input::InputView::Type) m_Contact.getSubType();
		getNavigator()->navigateTo(new Input::InputView(m_RecordId, type));
	});
	menu->addItem("IDS_PB_BUTTON_DELETE_ABB4", [this] {
		Ui::Popup *popup = new Ui::Popup();
		popup->create(getEvasObject());
		popup->setTitle("IDS_PB_BUTTON_DELETE_ABB4");
		popup->setText("IDS_PB_POP_THIS_CONTACT_WILL_BE_DELETED");
		popup->addButton("IDS_PB_BUTTON_CANCEL");
		popup->addButton("IDS_PB_BUTTON_DELETE_ABB4", [this] {
			m_Contact.remove();
			delete this;
			return true;
		});
	});
	menu->addItem("IDS_PB_OPT_SHARE", [this] {
		if (m_Contact.getSubType() == ObjectTypeContact) {
			int personId = 0;
			contacts_record_get_int(m_Contact.getRecord(), _contacts_contact.person_id, &personId);
			m_AppControl = App::requestShareContact(personId);
		} else {
			m_AppControl = App::requestShareMyProfile(m_Contact.getRecordId());
		}
	});

	menu->show();
}

Ui::GenlistItem *DetailsView::createFieldItem(ContactObject &field)
{
	Ui::GenlistItem *item = nullptr;
	ContactFieldId fieldId = ContactFieldId(field.getId());

	if (fieldId == FieldNumber) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionCall);
	} else if (fieldId == FieldEmail) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionComposeEmail);
	} else if (fieldId == FieldUrl) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionOpenWebpage);
	} else if (fieldId == FieldNote) {
		item = new MultilineFieldItem(field);
	} else if (field.getSubType() & ObjectTyped) {
		item = new TypedFieldItem(field);
	} else {
		item = new FieldItem(field);
	}

	return item;
}

Ui::GenlistItem *DetailsView::addFieldItem(ContactObject &field)
{
	Ui::GenlistItem *item = createFieldItem(field);
	m_Genlist->insert(item);
	return item;
}
