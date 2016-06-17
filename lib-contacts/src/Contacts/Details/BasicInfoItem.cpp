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

#include "Contacts/Details/BasicInfoItem.h"
#include "Contacts/Model/Contact.h"
#include "Contacts/Model/ContactBoolField.h"
#include "Contacts/Model/ContactCompoundObject.h"
#include "Contacts/Model/ContactTextField.h"

#include "Ui/Genlist.h"
#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"

#include "ContactsDetailsItemLayout.h"

using namespace Contacts::Details;
using namespace Contacts::Model;
using namespace std::placeholders;

BasicInfoItem::BasicInfoItem(Contact &contact)
	: m_Contact     (contact),
	  m_Favorite    (contact.getFieldById<ContactBoolField>(FieldFavorite)),

	  m_Image       (*contact.getFieldById<ContactObject>(FieldImage)),
	  m_ImagePath   (*m_Image.getField<ContactTextField>(0)),

	  m_Name        (*contact.getFieldById<ContactTextField>(FieldDisplayName)),
	  m_PhoneticName(*contact.getFieldById<ContactCompoundObject>(FieldPhoneticName)),

	  m_Company     (*contact.getFieldById<ContactObject>(FieldCompany)),
	  m_CompanyName (*m_Company.getFieldById<ContactTextField>(CompanyFieldName)),
	  m_JobTitle    (*m_Company.getFieldById<ContactTextField>(CompanyFieldJobTitle)),

	  m_SelectMode(Ux::SelectNone)
{
	m_Contact.setUpdateCallback(std::bind(&BasicInfoItem::onFieldUpdated, this, _1, _2));
}

void BasicInfoItem::setSelectMode(Ux::SelectMode mode)
{
	m_SelectMode = mode;
	elm_genlist_item_fields_update(getObjectItem(), PART_BACK_BTN, ELM_GENLIST_ITEM_FIELD_CONTENT);
	elm_genlist_item_fields_update(getObjectItem(), PART_FAV_BTN, ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void BasicInfoItem::setBackCallback(BackCallback callback)
{
	m_OnBackPressed = std::move(callback);
}

Elm_Genlist_Item_Class *BasicInfoItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(CONTACTS_DETAILS_ITEM_STYLE);
	return &itc;
}

char *BasicInfoItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_NAME) == 0) {
		return Utils::safeDup(m_Name.getValue());
	} else if (strcmp(part, PART_PHONETIC_NAME) == 0) {
		return strdup(m_PhoneticName.getValue().c_str());
	} else if (strcmp(part, PART_COMPANY) == 0) {
		return strdup(formatCompanyString().c_str());
	}

	return nullptr;
}

Evas_Object *BasicInfoItem::getContent(Evas_Object *parent, const char *part)
{
	if (m_SelectMode == Ux::SelectNone) {
		if (strcmp(part, PART_BACK_BTN) == 0) {
			return createBackButton(parent);
		} else if (m_Favorite && strcmp(part, PART_FAV_BTN) == 0) {
			return createFavButton(parent);
		}
	}

	if (strcmp(part, PART_THUMBNAIL) == 0) {
		auto control = Ui::Thumbnail::create(parent, Ui::Thumbnail::SizeLarge);
		control->setImagePath(m_ImagePath.getValue());
		return control->getEvasObject();
	}

	return nullptr;
}

Eina_Bool BasicInfoItem::getState(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_PHONETIC_NAME) == 0) {
		return !m_PhoneticName.isEmpty();
	} else if (strcmp(part, PART_COMPANY) == 0) {
		return !m_Company.isEmpty();
	}

	return EINA_FALSE;
}

std::string BasicInfoItem::formatCompanyString()
{
	const char *companyName = m_CompanyName.getValue();
	const char *jobTitle = m_JobTitle.getValue();

	std::string value;
	if (companyName) {
		value += companyName;
	}

	if (jobTitle) {
		if (!value.empty()) {
			value += ", ";
		}
		value += jobTitle;
	}

	return value;
}

Evas_Object *BasicInfoItem::createBackButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "naviframe/back_btn/default");
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&BasicInfoItem::onBackPressed), this);
	return button;
}

Evas_Object *BasicInfoItem::createFavButton(Evas_Object *parent)
{
	Evas_Object *check = elm_check_add(parent);
	elm_object_style_set(check, "favorite");
	elm_check_state_set(check, m_Favorite->getValue());
	evas_object_smart_callback_add(check, "changed",
			makeCallback(&BasicInfoItem::onFavChanged), this);
	return check;
}

void BasicInfoItem::onBackPressed(Evas_Object *button, void *eventInfo)
{
	if (m_OnBackPressed) {
		m_OnBackPressed();
	}
}

void BasicInfoItem::onFavChanged(Evas_Object *check, void *eventInfo)
{
	m_Favorite->setValue(elm_check_state_get(check));
	m_Contact.save();
}

void BasicInfoItem::onFieldUpdated(ContactField &field, contacts_changed_e change)
{
	if (&field == &m_Contact && change == CONTACTS_CHANGE_DELETED) {
		/* FIXME: Add possibility to get View from child control */
		if (m_OnBackPressed) {
			m_OnBackPressed();
		}
		return;
	}

	const char *part = nullptr;
	if (&field == m_Favorite) {
		part = PART_FAV_BTN;
	} else if (&field == &m_Image) {
		part = PART_THUMBNAIL;
	} else if (&field == &m_Name) {
		part = PART_NAME;
	} else if (&field == &m_PhoneticName) {
		part = PART_PHONETIC_NAME;
	} else if (&field == &m_Company) {
		part = PART_COMPANY;
	}

	if (part) {
		elm_genlist_item_fields_update(getObjectItem(), part, ELM_GENLIST_ITEM_FIELD_ALL);
	}
}
