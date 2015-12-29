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
#include "Contacts/Input/ContactTypedFieldItem.h"
#include "Contacts/Input/ContactCompoundFieldItem.h"
#include "Contacts/Model/ContactArray.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Utils/Range.h"

#include "InputPath.h"

using namespace Contacts;
using namespace Contacts::Input;
using namespace Contacts::Model;
using namespace std::placeholders;

InputView::InputView(int recordId, Type type)
	: m_RecordId(recordId), m_Contact(ContactObjectType(type)),
	  m_DoneButton(nullptr), m_Genlist(nullptr),
	  m_Items{nullptr}, m_AddFieldsItem(nullptr)
{
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

	return m_Genlist->getEvasObject();
}

void InputView::onCreated()
{
	int err = m_Contact.initialize(m_RecordId);
	RETM_IF_ERR(err, "Contact::initialize() failed.");

	if (m_Contact.isNew()) {
		addFieldItem(addField(FieldImage));
		addFieldItem(addField(FieldName));
		addFieldItem(addField(FieldNumber));
		return;
	}

	for (auto &&field : m_Contact) {
		switch (field->getType()) {
			case TypeArray:
				for (auto &&element : field->cast<ContactArray>()) {
					addFieldItem(std::move(element));
				}
				break;
			case TypeObject:
				if (!field->isEmpty()) {
					m_AddFieldsItem->setAddFieldState(ContactFieldId(field->getId()), false);
					addFieldItem(std::move(field));
				}
				break;
			default:
				break;
		}
	}
}

void InputView::onPageAttached()
{
	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&InputView::onDonePressed), this);

	Evas_Object *cancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(cancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(cancelButton, "IDS_PB_BUTTON_CANCEL");
	evas_object_smart_callback_add(cancelButton, "clicked",
			makeCallback(&InputView::onCancelPressed), this);

	Ui::NavigatorPage *page = getPage();
	page->setTitle(m_Contact.isNew() ? "IDS_PB_HEADER_CREATE" : "IDS_PB_HEADER_EDIT");
	page->setContent("title_right_btn", m_DoneButton);
	page->setContent("title_left_btn", cancelButton);
}

ContactFieldItem *InputView::createFieldItem(Model::ContactFieldPtr field)
{
	ContactFieldItem *item = nullptr;
	if (field->getSubType() & ObjectTyped) {
		item = new ContactTypedFieldItem(std::move(field));
	} else if (field->getSubType() & ObjectCompound) {
		item = new ContactCompoundFieldItem(std::move(field));
	} else {
		item = new ContactFieldItem(std::move(field));
	}

	item->setRemoveCallback(std::bind(&InputView::onRemoveField, this, _1, _2));

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

ContactFieldPtr InputView::addField(ContactFieldId fieldId)
{
	ContactFieldPtr parentField = m_Contact.getFieldById(fieldId);
	if (parentField->getType() == TypeArray) {
		return parentField->cast<ContactArray>().addField();
	} else {
		m_AddFieldsItem->setAddFieldState(fieldId, false);
		return parentField;
	}
}

void InputView::removeField(ContactFieldPtr field)
{
	ContactFieldId fieldId = ContactFieldId(field->getId());
	ContactFieldPtr parentField = m_Contact.getFieldById(fieldId);
	if (parentField->getType() == TypeArray) {
		parentField->cast<ContactArray>().removeField(std::move(field));
	} else {
		m_AddFieldsItem->setAddFieldState(fieldId, true);
		field->reset();
	}
}

ContactFieldItem *InputView::addFieldItem(Model::ContactFieldPtr field)
{
	ContactFieldId fieldId = ContactFieldId(field->getId());

	ContactFieldItem *item = createFieldItem(std::move(field));
	m_Genlist->insert(item, nullptr, getNextFieldItem(fieldId));

	if (!m_Items[fieldId]) {
		m_Items[fieldId] = item;
	}

	return item;
}

void InputView::removeFieldItem(ContactFieldItem *item, ContactFieldId fieldId)
{
	if (item == m_Items[fieldId]) {
		ContactFieldItem *nextItem = static_cast<ContactFieldItem *>(item->getNextGroupItem());
		if (nextItem && nextItem->getObject().getId() == fieldId) {
			m_Items[fieldId] = nextItem;
		} else {
			m_Items[fieldId] = nullptr;
		}
	}
}

void InputView::onAddField(ContactFieldId fieldId)
{
	ContactFieldItem *item = addFieldItem(addField(fieldId));
	item->focus(ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void InputView::onRemoveField(ContactFieldItem *item, ContactFieldPtr field)
{
	removeFieldItem(item, ContactFieldId(field->getId()));
	removeField(std::move(field));
}

void InputView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	int err = m_Contact.save();
	RETM_IF_ERR(err, "Contact::save() failed.");

	delete this;
}

void InputView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	delete this;
}
