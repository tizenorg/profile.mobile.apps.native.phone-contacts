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

#include "DetailsItemLayout.h"

using namespace Contacts::Details;
using namespace Contacts::Model;

BasicInfoItem::BasicInfoItem(Contact &contact)
	: m_Contact(contact),
	  m_Favorite(nullptr), m_ImagePath(nullptr),
	  m_Name(nullptr), m_PhoneticName(nullptr),
	  m_Company(nullptr), m_CompanyName(nullptr), m_JobTitle(nullptr)
{
	for (auto &&field : m_Contact) {
		unsigned fieldId = field.getId();
		if (fieldId > FieldCompany) {
			break;
		}

		switch(fieldId) {
			case FieldImage:        m_ImagePath    = &field.cast<ContactObject>().getField(0)->cast<ContactTextField>(); break;
			case FieldFavorite:     m_Favorite     = &field.cast<ContactBoolField>(); break;
			case FieldName:         m_Name         = &field.cast<ContactCompoundObject>(); break;
			case FieldPhoneticName: m_PhoneticName = &field.cast<ContactCompoundObject>(); break;
			case FieldCompany:      m_Company      = &field.cast<ContactObject>(); break;
		}
	}

	if (m_Company) {
		m_CompanyName = &m_Company->getFieldById(CompanyFieldName)->cast<ContactTextField>();
		m_JobTitle = &m_Company->getFieldById(CompanyFieldJobTitle)->cast<ContactTextField>();
	}
}

Elm_Genlist_Item_Class *BasicInfoItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(DETAILS_ITEM_STYLE);
	return &itc;
}

char *BasicInfoItem::getText(Evas_Object *parent, const char *part)
{
	if (m_Name && strcmp(part, PART_NAME) == 0) {
		return strdup(m_Name->getValue().c_str());
	} else if (m_PhoneticName && strcmp(part, PART_PHONETIC_NAME) == 0) {
		return strdup(m_PhoneticName->getValue().c_str());
	} else if (m_Company && strcmp(part, PART_COMPANY) == 0) {
		return strdup(formatCompanyString().c_str());
	}

	return nullptr;
}

Evas_Object *BasicInfoItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_BACK_BTN) == 0) {
		return createBackButton(parent);
	} else if (m_Favorite && strcmp(part, PART_FAV_BTN) == 0) {
		return createFavButton(parent);
	} else if (m_ImagePath && strcmp(part, PART_THUMBNAIL) == 0) {
		auto control = Ui::Thumbnail::create(parent, Ui::Thumbnail::SizeLarge);
		control->setImagePath(m_ImagePath->getValue());
		return control->getEvasObject();
	}

	return nullptr;
}

Eina_Bool BasicInfoItem::getState(Evas_Object *parent, const char *part)
{
	if (m_PhoneticName && strcmp(part, PART_PHONETIC_NAME) == 0) {
		return !m_PhoneticName->isEmpty();
	} else if (m_Company && strcmp(part, PART_COMPANY) == 0) {
		return !m_Company->isEmpty();
	}

	return EINA_FALSE;
}

std::string BasicInfoItem::formatCompanyString()
{
	const char *companyName = m_CompanyName->getValue();
	const char *jobTitle = m_JobTitle->getValue();

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
	/* FIXME: Add possibility to get View or Navigator from child control */
	delete getParent();
}

void BasicInfoItem::onFavChanged(Evas_Object *check, void *eventInfo)
{
	m_Favorite->setValue(elm_check_state_get(check));
	m_Contact.save();
}
