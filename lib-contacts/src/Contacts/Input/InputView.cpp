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

#include "Contacts/Input/InputView.h"
#include "Contacts/Input/AddFieldsItem.h"
#include "Contacts/Input/ContactCompoundFieldItem.h"
#include "Contacts/Input/ContactImageFieldItem.h"
#include "Contacts/Input/ContactRelationshipFieldItem.h"
#include "Contacts/Input/ContactTypedFieldItem.h"
#include "Contacts/Details/DetailsView.h"

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Model/ContactCompoundObject.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Navigator.h"
#include "Ui/Popup.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Utils/Range.h"

#include "InputPath.h"

using namespace Contacts;
using namespace Contacts::Input;
using namespace Contacts::Model;
using namespace std::placeholders;

namespace
{
	bool isFieldVisible[] = {
		/* [FieldFavorite]     = */ false,
		/* [FieldDisplayName]  = */ false,
		/* [FieldImage]        = */ true,
		/* [FieldName]         = */ true,
		/* [FieldPhoneticName] = */ true,
		/* [FieldCompany]      = */ true,
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

InputView::InputView(int recordId, Type type)
	: m_Contact(ContactObjectType(type)),
	  m_DoneButton(nullptr), m_Genlist(nullptr),
	  m_Items{nullptr}, m_AddFieldsItem(nullptr)
{
	m_Contact.initialize(recordId);
	m_Contact.setFillCallback(std::bind(&InputView::onContactFilled,
				this, std::placeholders::_1));
}

void InputView::addField(Model::ContactFieldId fieldId, const char *value)
{
	if (!value) {
		return;
	}

	ContactObject &field = addField(fieldId);
	if (field.getInterfaces() & InterfaceCompoundObject) {
		field.cast<ContactCompoundObject>().setValue(value);
	} else {
		ContactField *subField = field.getField(0);
		if (subField && subField->getType() == TypeText) {
			subField->cast<ContactTextField>().setValue(value);
		} else {
			removeField(field);
			return;
		}
	}

	if (m_Genlist) {
		ContactFieldItem *item = m_Items[fieldId];
		if (item && (&item->getObject() == &field)) {
			item->update();
		} else {
			addFieldItem(field);
		}
	}
}

void InputView::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

Evas_Object *InputView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(INPUT_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);
	elm_genlist_select_mode_set(m_Genlist->getEvasObject(), ELM_OBJECT_SELECT_MODE_NONE);

	m_AddFieldsItem = new AddFieldsItem();
	m_AddFieldsItem->setAddFieldCallback(std::bind(&InputView::onAddField, this, _1));
	m_Genlist->insert(m_AddFieldsItem);

	for (unsigned id = FieldBegin; id < FieldEnd; ++id) {
		if (!isFieldVisible[id]) {
			m_AddFieldsItem->setAddFieldState(ContactFieldId(id), false);
		}
	}

	return m_Genlist->getEvasObject();
}

void InputView::onCreated()
{
	addFieldItem(addField(FieldImage));
	addFieldItem(addField(FieldName))->focus();

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
				if (!m_Items[fieldId] && !field.isEmpty()) {
					addFieldItem(field.cast<ContactObject>());
					m_AddFieldsItem->setAddFieldState(fieldId, false);
				}
				break;
			default:
				break;
		}
	}

	if (m_Contact.isNew() && m_Contact.getFieldById(FieldNumber)->isEmpty()) {
		addFieldItem(addField(FieldNumber));
	}
}

void InputView::onPageAttached(Ui::NavigatorPage *page)
{
	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_disabled_set(m_DoneButton, !m_Contact.isFilled());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&InputView::onDonePressed), this);

	Evas_Object *cancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(cancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(cancelButton, "IDS_PB_BUTTON_CANCEL");
	evas_object_smart_callback_add(cancelButton, "clicked",
			makeCallback(&InputView::onCancelPressed), this);

	page->setTitle(m_Contact.isNew() ? "IDS_PB_HEADER_CREATE" : "IDS_PB_HEADER_EDIT");
	page->setContent("title_right_btn", m_DoneButton);
	page->setContent("title_left_btn", cancelButton);
}

bool InputView::onBackPressed()
{
	return onCancel();
}

ContactFieldItem *InputView::createFieldItem(ContactObject &field)
{
	ContactFieldItem *item = nullptr;
	if (field.getId() == FieldImage) {
		item = new ContactImageFieldItem(field);
	} else if (field.getId() == FieldRelationship) {
		item = new ContactRelationshipFieldItem(field);
	} else if (field.getInterfaces() & InterfaceTypedObject) {
		item = new ContactTypedFieldItem(field);
	} else if (field.getInterfaces() & InterfaceCompoundObject) {
		item = new ContactCompoundFieldItem(field);
	} else {
		item = new ContactFieldItem(field);
	}

	item->setRemoveCallback(std::bind(&InputView::onRemoveField, this, _1));

	return item;
}

Ui::GenlistItem *InputView::getNextFieldItem(ContactFieldId fieldId)
{
	for (unsigned id = fieldId + 1; id < Utils::count(m_Items); ++id) {
		if (m_Items[id]) {
			return m_Items[id];
		}
	}

	return m_AddFieldsItem;
}

ContactObject &InputView::addField(ContactFieldId fieldId)
{
	ContactField &parentField = *m_Contact.getFieldById(fieldId);
	if (parentField.getType() == TypeArray) {
		return parentField.cast<ContactArray>().addField().cast<ContactObject>();
	} else {
		if (m_AddFieldsItem) {
			m_AddFieldsItem->setAddFieldState(fieldId, false);
		}
		return parentField.cast<ContactObject>();
	}
}

void InputView::removeField(ContactObject &field)
{
	ContactFieldId fieldId = ContactFieldId(field.getId());
	ContactField &parentField = *m_Contact.getFieldById(fieldId);
	if (parentField.getType() == TypeArray) {
		parentField.cast<ContactArray>().removeField(field);
	} else {
		if (m_AddFieldsItem) {
			m_AddFieldsItem->setAddFieldState(fieldId, true);
		}
		field.reset();
	}
}

ContactFieldItem *InputView::addFieldItem(Model::ContactObject &field)
{
	ContactFieldId fieldId = ContactFieldId(field.getId());

	ContactFieldItem *item = createFieldItem(field);
	m_Genlist->insert(item, nullptr, getNextFieldItem(fieldId));

	if (!m_Items[fieldId]) {
		m_Items[fieldId] = item;
	}

	return item;
}

void InputView::removeFieldItem(ContactFieldItem *item)
{
	ContactFieldId fieldId = ContactFieldId(item->getObject().getId());

	if (item == m_Items[fieldId]) {
		ContactFieldItem *nextItem = static_cast<ContactFieldItem *>(item->getNextGroupItem());
		if (nextItem && nextItem->getObject().getId() == fieldId) {
			m_Items[fieldId] = nextItem;
		} else {
			m_Items[fieldId] = nullptr;
		}
	}

	delete item;
}

void InputView::onAddField(ContactFieldId fieldId)
{
	ContactFieldItem *item = addFieldItem(addField(fieldId));
	item->focus(ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void InputView::onRemoveField(ContactFieldItem *item)
{
	ContactObject &field = item->getObject();
	removeFieldItem(item);
	removeField(field);
}

void InputView::onContactFilled(bool isFilled)
{
	elm_object_disabled_set(m_DoneButton, !isFilled);
}

void InputView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	int id = m_Contact.save();
	if (m_OnResult) {
		m_OnResult(id);
	}

	if (m_Contact.isNew()) {
		using Details::DetailsView;
		DetailsView *view = new DetailsView(id, DetailsView::Type(m_Contact.getSubType()));
		getNavigator()->navigateTo(view);
	}

	getPage()->close();
}

void InputView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	if (onCancel()) {
		getPage()->close();
	}
}

bool InputView::onCancel()
{
	if (!m_Contact.isChanged()) {
		return true;
	}

	Ui::Popup *popup = new Ui::Popup();
	popup->create(getEvasObject());
	popup->setTitle("IDS_PB_HEADER_DISCARD_CHANGES_ABB");
	popup->setText("IDS_PB_POP_ALL_CHANGES_WILL_BE_DISCARDED");

	popup->addButton("IDS_PB_BUTTON_CANCEL");
	popup->addButton("IDS_PB_BUTTON_DISCARD_ABB", [this] {
		getPage()->close();
		return true;
	});

	return false;
}
