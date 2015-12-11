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
#include "Contacts/Input/ContactTypedObjectItem.h"
#include "Contacts/Model/ContactFactory.h"
#include "Contacts/Model/ContactArray.h"
#include "Contacts/Common/Strings.h"

#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Utils/Range.h"

#include "InputPath.h"

using namespace Contacts;
using namespace Contacts::Input;
using namespace Contacts::Model;

InputView::InputView(int recordId, Type type)
	: m_Contact(ContactObjectType(type), recordId),
	  m_DoneButton(nullptr), m_Genlist(nullptr), m_Items{nullptr}
{
}

Evas_Object *InputView::onCreate(Evas_Object *parent)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(INPUT_ITEM_LAYOUT_EDJ).c_str());

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);
	elm_genlist_select_mode_set(m_Genlist->getEvasObject(), ELM_OBJECT_SELECT_MODE_NONE);

	return m_Genlist->getEvasObject();
}

void InputView::onCreated()
{
	if (m_Contact.isNew()) {
		addFieldItem(m_Contact.getFieldById(FieldImage));
		addFieldItem(m_Contact.getFieldById(FieldName));
		addFieldItem(m_Contact.getFieldById(FieldNumber)->cast<ContactArray>().addField());
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

Evas_Object *InputView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	for (auto &&field : m_Contact) {
		ContactFieldId id = (ContactFieldId) field->getId();
		if (field->getType() == TypeArray || !m_Items[id]) {
			menu->addItem(Common::getContactFieldName(id), [this, id] {
				auto item = addFieldItem(addField(id));
				elm_genlist_item_bring_in(item->getObjectItem(), ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
			});
		}
	}

	return menu->getEvasObject();
}

ContactObjectItem *InputView::createFieldItem(Model::ContactFieldPtr field)
{
	ContactObjectItem *item = nullptr;
	if (field->getSubType() & ObjectTyped) {
		item = new ContactTypedObjectItem(std::move(field));
	} else {
		item = new ContactObjectItem(std::move(field));
	}

	item->setRemoveCallback([this](ContactObjectItem *item, ContactFieldPtr field) {
		removeFieldItem(item, field->getId());
		removeField(std::move(field));
	});

	return item;
}

ContactObjectItem *InputView::getNextFieldItem(unsigned fieldId)
{
	for (++fieldId; fieldId < Utils::count(m_Items); ++fieldId) {
		if (m_Items[fieldId]) {
			return m_Items[fieldId];
		}
	}

	return nullptr;
}

ContactFieldPtr InputView::addField(unsigned fieldId)
{
	ContactFieldPtr parentField = m_Contact.getFieldById(fieldId);
	if (parentField->getType() == TypeArray) {
		return parentField->cast<ContactArray>().addField();
	} else {
		return parentField;
	}
}

void InputView::removeField(ContactFieldPtr field)
{
	ContactFieldPtr parentField = m_Contact.getFieldById(field->getId());
	if (parentField->getType() == TypeArray) {
		parentField->cast<ContactArray>().removeField(std::move(field));
	} else {
		field->reset();
	}
}

ContactObjectItem *InputView::addFieldItem(Model::ContactFieldPtr field)
{
	unsigned fieldId = field->getId();
	ContactObjectItem *item = createFieldItem(std::move(field));

	m_Genlist->insert(item, nullptr, getNextFieldItem(fieldId));
	if (!m_Items[fieldId]) {
		m_Items[fieldId] = item;
	}

	return item;
}

void InputView::removeFieldItem(ContactObjectItem *item, unsigned fieldId)
{
	if (item == m_Items[fieldId]) {
		ContactObjectItem *nextItem = static_cast<ContactObjectItem *>(*item->end());
		if (nextItem && nextItem->getObject()->getId() == fieldId) {
			m_Items[fieldId] = nextItem;
		} else {
			m_Items[fieldId] = nullptr;
		}
	}
}

void InputView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	m_Contact.save();
	delete this;
}

void InputView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	delete this;
}
