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
#include "Utils/Range.h"

#include "DetailsItemLayout.h"

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
	: m_RecordId(recordId), m_Contact(ContactObjectType(type)),
	  m_FilterType(filterType), m_SelectMode(SelectNone), m_ResultType(ResultNone),
	  m_Genlist(nullptr), m_BasicInfoItem(nullptr), m_Items{nullptr}
{
}

void DetailsView::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

void DetailsView::setSelectMode(SelectMode mode, ResultType type)
{
	m_SelectMode = mode;
	m_ResultType = type;

	if (m_BasicInfoItem) {
		m_BasicInfoItem->setSelectMode(mode);

		auto item = m_BasicInfoItem->getNextItem();
		for (; item; item = item->getNextItem()) {
			FieldItem *fieldItem = static_cast<FieldItem *>(item);
			fieldItem->setSelectMode(mode, type);
		}
	}

	updatePageMode();
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

	m_BasicInfoItem = new BasicInfoItem(m_Contact);
	m_BasicInfoItem->setSelectMode(m_SelectMode);
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
							this, _1, _2));
				}
				break;
			default:
				break;
		}
	}
}

void DetailsView::onPageAttached()
{
	updatePageMode();
}

void DetailsView::onMenuPressed()
{
	if (m_SelectMode != SelectNone) {
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

void DetailsView::updatePageMode()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	switch (m_SelectMode) {
		case SelectNone:
			page->setTitle(nullptr);
			break;

		case SelectSingle:
			page->setTitle("IDS_PB_HEADER_SELECT");
			break;

		default:
			break;
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

	item->setSelectMode(m_SelectMode, m_ResultType);
	item->setSelectCallback(std::bind(&DetailsView::onSingleSelected, this, _1));
	item->setRemoveCallback(std::bind(&DetailsView::removeFieldItem, this, _1));

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

	if (!m_Items[fieldId]) {
		m_Items[fieldId] = item;
	}

	return item;
}

void DetailsView::removeFieldItem(FieldItem *item)
{
	ContactFieldId fieldId = ContactFieldId(item->getObject().getId());
	item->getObject().setUpdateCallback(std::bind(&DetailsView::onObjectUpdated,
			this, _1, _2));

	if (item == m_Items[fieldId]) {
		FieldItem *nextItem = static_cast<FieldItem *>(item->getNextItem());
		if (nextItem && nextItem->getObject().getId() == fieldId) {
			m_Items[fieldId] = nextItem;
		} else {
			m_Items[fieldId] = nullptr;
		}
	}

	delete item;
}

void DetailsView::onSingleSelected(SelectResult result)
{
	if (!m_OnSelected || m_OnSelected({ &result, 1 })) {
		delete this;
	}
}

void DetailsView::onArrayUpdated(ContactField &field, contacts_changed_e change)
{
	if (change == CONTACTS_CHANGE_INSERTED) {
		addFieldItem(field.cast<ContactObject>());
	}
}

void DetailsView::onObjectUpdated(ContactField &field, contacts_changed_e change)
{
	if (change == CONTACTS_CHANGE_UPDATED) {
		if (!field.isEmpty()) {
			addFieldItem(field.getParent()->cast<ContactObject>());
		}
	}
}
