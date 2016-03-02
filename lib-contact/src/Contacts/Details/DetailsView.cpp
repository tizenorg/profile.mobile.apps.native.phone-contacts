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
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Utils/Range.h"

#include "ContactsDetailsItemLayout.h"

#include <app_i18n.h>

#define TITLE_SIZE 32

using namespace Contacts::Details;
using namespace Contacts::Model;
using namespace std::placeholders;

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

DetailsView::DetailsView(int recordId, Type type, int filterType)
	: m_RecordId(recordId), m_Contact(ContactObjectType(type)), m_FilterType(filterType),
	  m_Genlist(nullptr), m_BasicInfoItem(nullptr), m_Items{nullptr}
{
}

Evas_Object *DetailsView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(DETAILS_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);
	elm_genlist_homogeneous_set(m_Genlist->getEvasObject(), EINA_FALSE);

	return m_Genlist->getEvasObject();
}

void DetailsView::onCreated()
{
	int err = m_Contact.initialize(m_RecordId);
	RETM_IF_ERR(err, "Contact::initialize() failed.");

	m_BasicInfoItem = new BasicInfoItem(m_Contact);
	m_BasicInfoItem->setSelectMode(getSelectMode());
	m_Genlist->insert(m_BasicInfoItem);

	for (auto &&field : m_Contact) {
		ContactFieldId fieldId = ContactFieldId(field.getId());
		if (!isFieldVisible[fieldId] || !(m_FilterType & (1 << fieldId))) {
			continue;
		}

		switch (field.getType()) {
			case TypeArray:
				field.setUpdateCallback(std::bind(&DetailsView::onArrayUpdated,
						this, _1, _2));

				for (auto &&element : field.cast<ContactArray>()) {
					addFieldItem(element.cast<ContactObject>());
				}
				break;
			case TypeObject:
				if (!field.isEmpty()) {
					addFieldItem(field.cast<ContactObject>());
				} else {
					field.setUpdateCallback(std::bind(&DetailsView::onObjectUpdated,
							this, nullptr, _1, _2));
				}
				break;
			default:
				break;
		}
	}
}

void DetailsView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

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

		m_AppControl.launch();
	});

	menu->show();
}

void DetailsView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, m_BasicInfoItem, Ui::Genlist::After);
}

void DetailsView::onSelectModeChanged(SelectMode selectMode)
{
	if (m_BasicInfoItem) {
		m_BasicInfoItem->setSelectMode(selectMode);
	}
}

FieldItem *DetailsView::createFieldItem(ContactObject &field)
{
	FieldItem *item = nullptr;
	ContactFieldId fieldId = ContactFieldId(field.getId());

	if (fieldId == FieldNumber) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionCall);
	} else if (fieldId == FieldEmail) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionEmail);
	} else if (fieldId == FieldUrl) {
		item = new ActionFieldItem(field, ActionFieldItem::ActionUrl);
	} else if (fieldId == FieldNote) {
		item = new MultilineFieldItem(field);
	} else if (field.getSubType() & ObjectTyped) {
		item = new TypedFieldItem(field);
	} else {
		item = new FieldItem(field);
	}

	return item;
}

FieldItem *DetailsView::getNextFieldItem(ContactFieldId fieldId)
{
	for (unsigned id = fieldId + 1; id < Utils::count(m_Items); ++id) {
		if (m_Items[id]) {
			return m_Items[id];
		}
	}

	return nullptr;
}

FieldItem *DetailsView::addFieldItem(ContactObject &field)
{
	ContactFieldId fieldId = ContactFieldId(field.getId());

	FieldItem *item = createFieldItem(field);
	m_Genlist->insert(item, nullptr, getNextFieldItem(fieldId));
	onItemInserted(item);

	if (!m_Items[fieldId]) {
		m_Items[fieldId] = item;
	}

	field.setUpdateCallback(std::bind(&DetailsView::onObjectUpdated,
			this, item, _1, _2));

	return item;
}

void DetailsView::removeFieldItem(FieldItem *item)
{
	ContactFieldId fieldId = ContactFieldId(item->getObject().getId());
	onItemRemove(item);

	if (item == m_Items[fieldId]) {
		FieldItem *nextItem = static_cast<FieldItem *>(item->getNextItem());
		if (nextItem && nextItem->getObject().getId() == fieldId) {
			m_Items[fieldId] = nextItem;
		} else {
			m_Items[fieldId] = nullptr;
		}
	}

	item->getObject().setUpdateCallback(std::bind(&DetailsView::onObjectUpdated,
			this, nullptr, _1, _2));

	delete item;
}

void DetailsView::onArrayUpdated(ContactField &field, contacts_changed_e change)
{
	if (change == CONTACTS_CHANGE_INSERTED) {
		addFieldItem(field.cast<ContactObject>());
	}
}

void DetailsView::onObjectUpdated(FieldItem *item, ContactField &field, contacts_changed_e change)
{
	if (change == CONTACTS_CHANGE_UPDATED) {
		if (item) {
			if (field.isEmpty()) {
				removeFieldItem(item);
			} else {
				item->onFieldUpdated(field, change);
			}
		} else {
			if (!field.isEmpty()) {
				addFieldItem(field.getParent()->cast<ContactObject>());
			}
		}
	} else if (change == CONTACTS_CHANGE_DELETED) {
		if (item) {
			removeFieldItem(item);
		}
	}
}
