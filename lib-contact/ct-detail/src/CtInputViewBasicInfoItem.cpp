/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <app_preference.h>
#include <system_info.h>

#include "CtInputViewBasicInfoItem.h"
#include "CtType.h"
#include "CtInputData.h"
#include "ContactsDebug.h"
#include "ContactsThumbnail.h"
#include "CtPath.h"
#include "CtInputView.h"
#include "CtString.h"
#include "CtCommon.h"
#include "CtThemeMgr.h"

#define LABEL_FONT_SIZE 40
#define W_HEIGHT_Z1 800
#define W_HEIGHT_Z3 1280

namespace
{
	const std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
}

CtInputViewBasicInfoItem::CtInputViewBasicInfoItem(std::shared_ptr<CtData>& inputData, Evas_Object** nameEntry,
		const std::function<void ()>& itemChangedCb,
		const std::function<void (CtInputViewBasicInfoItem::ITEM_DELETED)>& itemDeletedCb,
		const std::function<void (Evas_Object*)>& imageClickedCb,
		const std::function<void (Evas_Object*, Elm_Object_Item*)>& activateNextEntryCb,
		const std::function<void (Evas_Object*)>& showCreateKeypadCb,
		const std::function<void (Elm_Object_Item*)>& updateEntryReturnKeyType)
{
	__inputData = inputData;
	__nameEntry = nameEntry;
	__itemChangedCb = itemChangedCb;
	__itemDeletedCb = itemDeletedCb;
	__imageClickedCb = imageClickedCb;
	__activateNextEntryCb = activateNextEntryCb;
	__showCreateKeypadCb = showCreateKeypadCb;
	__updateEntryReturnKeyType = updateEntryReturnKeyType;

	__initialize();

	system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_FONT_SIZE, __onFontChangedCb, this);
};

CtInputViewBasicInfoItem::~CtInputViewBasicInfoItem()
{
	WHIT();

	SAFE_FREE( __oldFullName );
	SAFE_FREE( __oldPrefix );
	SAFE_FREE( __oldFirstName );
	SAFE_FREE( __oldMiddleName );
	SAFE_FREE( __oldLastName );
	SAFE_FREE( __oldSuffix );
	SAFE_FREE( __oldPhoneticFirstName );
	SAFE_FREE( __oldPhoneticMiddleName );
	SAFE_FREE( __oldPhoneticLastName );
	SAFE_FREE( __oldNickname );
	SAFE_FREE( __oldCompany );
	SAFE_FREE( __oldJobTitle );
	SAFE_FREE( __oldImagePath );

	system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_FONT_SIZE);
}

void CtInputViewBasicInfoItem::__initialize()
{
	__focusEntry = NULL;
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
	__oldBaseInfoExpanded = false;
	__baseInfoExpanded = true;

	__fullNameIsChanged = false;
	__prefixIsChanged = false;
	__firstNameIsChanged = false;
	__middleNameIsChanged = false;
	__lastNameIsChanged = false;
	__suffixIsChanged = false;
	__phoneticFirstNameIsChanged = false;
	__phoneticMiddleNameIsChanged = false;
	__phoneticLastNameIsChanged = false;
	__nicknameIsChanged = false;
	__companyIsChanged = false;
	__jobTitleIsChanged = false;

	__fullNameIsEmpty = true;
	__prefixIsEmpty = true;
	__firstNameIsEmpty = true;
	__middleNameIsEmpty = true;
	__lastNameIsEmpty = true;
	__suffixIsEmpty = true;
	__phoneticFirstNameIsEmpty = true;
	__phoneticMiddleNameIsEmpty = true;
	__phoneticLastNameIsEmpty = true;
	__nicknameIsEmpty = true;
	__companyIsEmpty = true;
	__jobTitleIsEmpty = true;

	std::string name;
	__inputData->getFullName(name);
	__oldFullName = SAFE_STRDUP(name.c_str());
	__oldPrefix = SAFE_STRDUP(__inputData->getPrefixName());
	__oldFirstName = SAFE_STRDUP(__inputData->getFirstName());
	__oldMiddleName = SAFE_STRDUP(__inputData->getMiddleName());
	__oldLastName = SAFE_STRDUP(__inputData->getLastName());
	__oldSuffix = SAFE_STRDUP(__inputData->getSuffixName());
	__oldPhoneticFirstName = SAFE_STRDUP(__inputData->getPhoneticFirstName());
	__oldPhoneticMiddleName = SAFE_STRDUP(__inputData->getPhoneticMiddleName());
	__oldPhoneticLastName = SAFE_STRDUP(__inputData->getPhoneticLastName());
	__oldNickname = SAFE_STRDUP(__inputData->getNickname());
	__oldCompany = SAFE_STRDUP(__inputData->getCompany());
	__oldJobTitle = SAFE_STRDUP(__inputData->getJobTitle());
	__oldImagePath = SAFE_STRDUP(__inputData->getImagePath());

	if (__inputData->getPhoneticFirstName() || __inputData->getPhoneticMiddleName() || __inputData->getPhoneticLastName()) {
		__addPhoneticName = true;
	} else {
		__addPhoneticName = false;
	}
	if (__inputData->getNickname()) {
		__addNickname = true;
	} else {
		__addNickname = false;
	}
	if (__inputData->getCompany() || __inputData->getJobTitle()) {
		__addOrganization = true;
	} else {
		__addOrganization = false;
	}
	__updateItem = false;
	__isRealized = false;
	__subItemSize = 0;
}

Elm_Genlist_Item_Class* CtInputViewBasicInfoItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};

	itc.item_style = "baseinfo.contract";
	itc.func.content_get = __getContentBasicInfo;
	itc.func.del = WGENLIST_ITEM_DEL(); //__deleteGenlistItem;
	return &itc;
}

Evas_Object* CtInputViewBasicInfoItem::__getContentBasicInfo(void *data, Evas_Object *obj, const char *part)
{
	//WHIT();
	WPRET_VM(NULL == data, NULL, "data is NULL");
	CtInputViewBasicInfoItem *item = (CtInputViewBasicInfoItem*)data;

	//WDEBUG("part %s", part);
	if (0 == strcmp(part, "elm.icon.1")) {
		Evas_Object *box = NULL;
		Evas_Object *layout = NULL;
		int count = 0;

		box = elm_box_add(obj);
		evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(box);
		if (item->__baseInfoExpanded) {
			layout = __createPrefixField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createFirstNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createMiddleNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createLastNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createSuffixField(item, box);
			elm_box_pack_end(box, layout);
			count++;
		}
		else {
			layout = __createNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;
		}

		if( item->__addPhoneticName ) {
			layout = __createPhoneticFirstNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createPhoneticMiddleNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createPhoneticLastNameField(item, box);
			elm_box_pack_end(box, layout);
			count++;
		}
		if( item->__addNickname ) {
			layout = __createNicknameField(item, box);
			elm_box_pack_end(box, layout);
			count++;
		}
		if( item->__addOrganization ) {
			layout = __createCompanyField(item, box);
			elm_box_pack_end(box, layout);
			count++;

			layout = __createJobTitleField(item, box);
			elm_box_pack_end(box, layout);
			count++;
		}

		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_min_set(box, 0, item->__subItemSize * count);

		return box;
	} else if (0 == strcmp(part, "elm.icon.2")) {
		Evas_Object *icon = createThumbnail(obj, THUMBNAIL_166, item->__inputData->getImagePath());
		evas_object_event_callback_add(icon, EVAS_CALLBACK_MOUSE_UP, __basicInfoImageClickedCb, item);
		return icon;
	} else if (0 == strcmp(part, "elm.name.label")) {
		Evas_Object* label = elm_label_add(obj);
		char text[CT_TEXT_MAX_LEN] = {0};
		snprintf(text, CT_TEXT_MAX_LEN, "<font_size=%d>%s</font_size>", LABEL_FONT_SIZE, V_("IDS_PB_BODY_NAME"));
		elm_object_text_set(label, text);
		evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(label);

		return label;
	}

	return NULL;
}

Evas_Object* CtInputViewBasicInfoItem::__createNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	std::string name;
	item->__inputData->getFullName(name);

	if (name.size() > 0) {
		editfield = item->createEditField(layout, NULL, false, true);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_NAME"), false);
	}
	entry = editfield->getEntry();

	*item->__nameEntry = entry;
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);
	evas_object_smart_callback_add(entry, "changed", __basicInfoNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	elm_entry_input_panel_imdata_set(entry, (void*)"type=name&allowEmoji=true", 25);

	setEntryText( entry, name.c_str());

	std::string detailLayoutEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);

	Evas_Object *button = elm_button_add(layout);
	elm_object_style_set(button, "transparent");

	Evas_Object *button_content_layout = elm_layout_add(button);
	elm_layout_file_set(button_content_layout, detailLayoutEdjePath.c_str(), "input_open");
	elm_object_part_content_set(button, "elm.swallow.content", button_content_layout);

	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(button, EVAS_HINT_FILL, 0.0);
	evas_object_smart_callback_add(button, "clicked", __basicInfoExpandClickedCb, item);

	elm_object_part_content_set(layout, "elm.icon.2", button);

	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createPrefixField( CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *prefix = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	prefix = item->__inputData->getPrefixName();

	if (prefix && *prefix) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_TITLE_M_NAMEPREFIX"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoPrefixChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoPrefixChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, prefix);

	std::string detailLayoutEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);

	Evas_Object* button = elm_button_add(layout);
	elm_object_style_set(button, "transparent");

	Evas_Object *button_content_layout = elm_layout_add(button);
	elm_layout_file_set(button_content_layout, detailLayoutEdjePath.c_str(), "input_close");
	elm_object_part_content_set(button, "elm.swallow.content", button_content_layout);

	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(button, EVAS_HINT_FILL, 0.0);
	evas_object_smart_callback_add(button, "clicked", __basicInfoExpandClickedCb, item);

	elm_object_part_content_set(layout, "elm.icon.2", button);

	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createFirstNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *firstName = NULL;

	layout = elm_layout_add(parent);
	if (item->__subItemSize == 0) {
		evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, CtInputViewBasicInfoItem::__subItemResizeCb, item);
	}
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	firstName = item->__inputData->getFirstName();

	if (firstName && *firstName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_OPT_FIRST_NAME"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoFirstNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoFirstNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, firstName );
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createMiddleNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *middleName = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	middleName = item->__inputData->getMiddleName();

	if (middleName && *middleName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_MIDDLE_NAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoMiddleNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoMiddleNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, middleName);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createLastNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *lastName = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	lastName = item->__inputData->getLastName();

	if (lastName && *lastName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_LAST_NAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoLastNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoLastNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, lastName);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createSuffixField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *suffix = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	suffix = item->__inputData->getSuffixName();

	if (suffix && *suffix) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_NAME_SUFFIX_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoSuffixChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoSuffixChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
	evas_object_event_callback_add(entry, EVAS_CALLBACK_SHOW, __basicInfoSuffixShowCb, item);

	setEntryText(entry, suffix);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createPhoneticFirstNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	////CONTACTS_LOGGING_LOG(LOGGING_LOG_PHONETIC_NAME); //DO NOT REMOVE!!!

	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *phoneticName = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	phoneticName = item->__inputData->getPhoneticFirstName();

	if (phoneticName && *phoneticName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		// TODO: language translation
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_PHONETIC_FIRST_NAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoPhoneticFirstNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoPhoneticFirstNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
	if (INPUT_ADDED_ITEM_INDEX_PHONETIC_INFO == item->__addedIndex ) {
		if (item->__isRealized) {
			elm_object_focus_set(entry, EINA_TRUE);
			item->__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
		} else {
			item->__focusEntry = entry;
		}
	}

	setEntryText(entry, phoneticName);

	Evas_Object* btn_minus = elm_button_add(layout);
	elm_object_style_set(btn_minus, "icon_expand_delete");
	elm_object_part_content_set(layout, "elm.icon.2", btn_minus);
	evas_object_smart_callback_add(btn_minus, "clicked", __phoneticNameDeleteCb, item);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createPhoneticMiddleNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	////CONTACTS_LOGGING_LOG(LOGGING_LOG_PHONETIC_NAME); //DO NOT REMOVE!!!

	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *phoneticName = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	phoneticName = item->__inputData->getPhoneticMiddleName();

	if (phoneticName && *phoneticName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		// TODO: language translation
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_PHONETIC_MIDDLE_NAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoPhoneticMiddleNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoPhoneticMiddleNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, phoneticName);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createPhoneticLastNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	////CONTACTS_LOGGING_LOG(LOGGING_LOG_PHONETIC_NAME); //DO NOT REMOVE!!!

	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *phoneticName = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	phoneticName = item->__inputData->getPhoneticLastName();

	if (phoneticName && *phoneticName) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		// TODO: language translation
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_PHONETIC_LAST_NAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoPhoneticLastNameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoPhoneticLastNameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, phoneticName);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createNicknameField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *nickname = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	nickname = item->__inputData->getNickname();

	if (nickname && *nickname) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_NICKNAME_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoNicknameChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoNicknameChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
	if (INPUT_ADDED_ITEM_INDEX_NICKNAME == item->__addedIndex ) {
		if (item->__isRealized) {
			elm_object_focus_set(entry, EINA_TRUE);
			item->__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
		} else {
			item->__focusEntry = entry;
		}
	}

	setEntryText(entry, nickname);

	Evas_Object* btn_minus = elm_button_add(layout);
	elm_object_style_set(btn_minus, "icon_expand_delete");
	elm_object_part_content_set(layout, "elm.icon.2", btn_minus);
	evas_object_smart_callback_add(btn_minus, "clicked", __nicknameDeleteCb, item);
	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createCompanyField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *company = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	company = item->__inputData->getCompany();

	if (company && *company) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_COMPANY_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoCompanyChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoCompanyChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
	if (INPUT_ADDED_ITEM_INDEX_ORGANIZATION == item->__addedIndex ) {
		if (item->__isRealized) {
			elm_object_focus_set(entry, EINA_TRUE);
			item->__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
		} else {
			item->__focusEntry = entry;
		}
	}

	setEntryText(entry, company);

	Evas_Object* btn_minus = elm_button_add(layout);
	elm_object_style_set(btn_minus, "icon_expand_delete");
	elm_object_part_content_set(layout, "elm.icon.2", btn_minus);
	evas_object_smart_callback_add(btn_minus, "clicked", __organizationDeleteCb, item);

	return layout;
}

Evas_Object* CtInputViewBasicInfoItem::__createJobTitleField(CtInputViewBasicInfoItem* item, Evas_Object *parent)
{
	//WHIT();
	Evas_Object *layout = NULL;
	WEditfield *editfield = NULL;
	Evas_Object *entry = NULL;
	const char *jobTitle = NULL;

	layout = elm_layout_add(parent);
	CtThemeMgr::getInstance()->setObjectTheme(layout);
	elm_layout_file_set(layout, contactsEdjePath.c_str(), "baseinfo_sub_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0.0);
	evas_object_show(layout);

	jobTitle = item->__inputData->getJobTitle();

	if (jobTitle && *jobTitle) {
		editfield = item->createEditField(layout, NULL, false);
	}
	else {
		editfield = item->createEditField(layout, V_("IDS_PB_BODY_JOB_TITLE_ABB"), false);
	}

	entry = editfield->getEntry();
	elm_object_part_content_set(layout, "elm.icon.1", editfield->getEvasObj());

	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	evas_object_smart_callback_add(entry, "changed", __basicInfoJobTitleChangedCb, item);
	evas_object_smart_callback_add(entry, "preedit,changed", __basicInfoJobTitleChangedCb, item);
	evas_object_smart_callback_add(entry, "activated", __activatedCb, item);

	setEntryText(entry, jobTitle);
	return layout;
}

void CtInputViewBasicInfoItem::__basicInfoExpandClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__baseInfoExpanded = !item->__baseInfoExpanded;
	item->__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;

	Elm_Genlist_Item_Class* itc = item->getItemClassStatic();
	if( item->__baseInfoExpanded)
	{
		itc->item_style = "baseinfo.expand";
	}
	else
	{
		itc->item_style = "baseinfo.contract";
	}

	Evas_Object* box = NULL;
	box = elm_object_item_part_content_get(item->getElmObjectItem(), "elm.icon.1");
	if (box) {
		elm_box_clear(box);
	}

	item->__updateItem = true;
	elm_genlist_item_item_class_update( (Elm_Object_Item*)item->getElmObjectItem(), itc );

	elm_object_item_signal_emit((Elm_Object_Item*)item->getElmObjectItem(), "image,enable", "");
}

void CtInputViewBasicInfoItem::__basicInfoImageClickedCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	elm_object_item_signal_emit(item->getElmObjectItem(), "play_sound", "elm.icon.2");
	item->__imageClickedCb( obj );
}

void CtInputViewBasicInfoItem::__subItemResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord height = 0;
	evas_object_geometry_get(obj, NULL, NULL, NULL, &height);
	evas_object_event_callback_del(obj, EVAS_CALLBACK_RESIZE, CtInputViewBasicInfoItem::__subItemResizeCb);
	CtInputViewBasicInfoItem *self = (CtInputViewBasicInfoItem *)data;
	self->__subItemSize = height;
	self->__baseInfoExpanded = self->__oldBaseInfoExpanded;

	elm_genlist_item_update(self->getElmObjectItem());
}

void CtInputViewBasicInfoItem::__onFontChangedCb(system_settings_key_e key, void *user_data)
{
	CtInputViewBasicInfoItem *self = (CtInputViewBasicInfoItem *)user_data;
	self->__subItemSize = 0;

	self->__oldBaseInfoExpanded = self->__baseInfoExpanded;
	if(!self->__baseInfoExpanded) {
		self->__baseInfoExpanded = true;
	}

	elm_genlist_item_update(self->getElmObjectItem());
}

bool CtInputViewBasicInfoItem::__compareWithOldVal(const char* text, const char* oldVal)
{
	WPRET_VM(NULL == text, false, "text is NULL");
	bool ret = false;
	if (NULL == oldVal) {
		if (SAFE_STRLEN(text)) {
			ret = true;
		}
		else {
			ret = false;
		}
	}
	else {
		if (!strcmp(text, oldVal)) {
			ret = false;
		}
		else {
			ret = true;
		}
	}
	return ret;
}

void CtInputViewBasicInfoItem::__entryTextChanged( Evas_Object* obj, int changedInfo )
{
	WHIT();
	char *text = NULL;
	ITEM_CHANGED info = (ITEM_CHANGED)changedInfo;

	switch( info )
	{
	case NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		std::string name;
		__inputData->getFullName(name);
		if(!(name.size() > 0 && text && strcmp(name.c_str(), text) == 0)) {
			__inputData->setFullName(text);
		}
		__fullNameIsChanged = __compareWithOldVal(text, __oldFullName);
		if (SAFE_STRLEN(text)) {
			__fullNameIsEmpty = false;
		}
		else {
			__fullNameIsEmpty = true;
		}
	}
		break;
	case PREFIX_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setPrefixName(text);
		__prefixIsChanged = __compareWithOldVal(text, __oldPrefix);
		if (SAFE_STRLEN(text)) {
			__prefixIsEmpty = false;
		}
		else {
			__prefixIsEmpty = true;
		}
	}
		break;
	case FIRST_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setFirstName(text);
		__firstNameIsChanged = __compareWithOldVal(text, __oldFirstName);
		if (SAFE_STRLEN(text)) {
			__firstNameIsEmpty = false;
		}
		else {
			__firstNameIsEmpty = true;
		}
	}
		break;
	case MIDDLE_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setMiddleName(text);
		__middleNameIsChanged = __compareWithOldVal(text, __oldMiddleName);
		if (SAFE_STRLEN(text)) {
			__middleNameIsEmpty = false;
		}
		else {
			__middleNameIsEmpty = true;
		}
	}
		break;
	case LAST_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setLastName(text);
		__lastNameIsChanged = __compareWithOldVal(text, __oldLastName);
		if (SAFE_STRLEN(text)) {
			__lastNameIsEmpty = false;
		}
		else {
			__lastNameIsEmpty = true;
		}
	}
		break;
	case SUFFIX_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setSuffixName(text);
		__suffixIsChanged = __compareWithOldVal(text, __oldSuffix);
		if (SAFE_STRLEN(text)) {
			__suffixIsEmpty = false;
		}
		else {
			__suffixIsEmpty = true;
		}
	}
		break;
	case PHONETIC_FIRST_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setPhoneticFirstName(text);
		__phoneticFirstNameIsChanged = __compareWithOldVal(text, __oldPhoneticFirstName);
		if (SAFE_STRLEN(text)) {
			__phoneticFirstNameIsEmpty = false;
		}
		else {
			__phoneticFirstNameIsEmpty = true;
		}
	}
		break;
	case PHONETIC_MIDDLE_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setPhoneticMiddleName(text);
		__phoneticMiddleNameIsChanged = __compareWithOldVal(text, __oldPhoneticMiddleName);
		if (SAFE_STRLEN(text)) {
			__phoneticMiddleNameIsEmpty = false;
		}
		else {
			__phoneticMiddleNameIsEmpty = true;
		}
	}
		break;
	case PHONETIC_LAST_NAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setPhoneticLastName(text);
		__phoneticLastNameIsChanged = __compareWithOldVal(text, __oldPhoneticLastName);
		if (SAFE_STRLEN(text)) {
			__phoneticLastNameIsEmpty = false;
		}
		else {
			__phoneticLastNameIsEmpty = true;
		}
	}
		break;
	case NICKNAME_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setNickname(text);
		__nicknameIsChanged = __compareWithOldVal(text, __oldNickname);
		if (SAFE_STRLEN(text)) {
			__nicknameIsEmpty = false;
		}
		else {
			__nicknameIsEmpty = true;
		}
	}
		break;
	case COMPANY_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setCompany(text);
		__companyIsChanged = __compareWithOldVal(text, __oldCompany);
		if (SAFE_STRLEN(text)) {
			__companyIsEmpty = false;
		}
		else {
			__companyIsEmpty = true;
		}
	}
		break;
	case JOB_TITLE_CHANGED:
	{
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		__inputData->setJobTitle(text);
		__jobTitleIsChanged = __compareWithOldVal(text, __oldJobTitle);
		if (SAFE_STRLEN(text)) {
			__jobTitleIsEmpty = false;
		}
		else {
			__jobTitleIsEmpty = true;
		}
	}
		break;
	default:
		WERROR("Not handled case =%d", changedInfo);
		break;
	}
	SAFE_FREE(text);
}

void CtInputViewBasicInfoItem::__basicInfoNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, NAME_CHANGED );
	item->__itemChangedCb();
	item->__updateEntryReturnKeyType(NULL);

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_NAME");
	}
}

void CtInputViewBasicInfoItem::__basicInfoPrefixChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, PREFIX_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_TITLE_M_NAMEPREFIX");
	}
}

void CtInputViewBasicInfoItem::__basicInfoFirstNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, FIRST_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_OPT_FIRST_NAME");
	}
}

void CtInputViewBasicInfoItem::__basicInfoMiddleNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, MIDDLE_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_MIDDLE_NAME_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoLastNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, LAST_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_LAST_NAME_ABB");
	}
}
void CtInputViewBasicInfoItem::__basicInfoSuffixChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, SUFFIX_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_NAME_SUFFIX_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoPhoneticFirstNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, PHONETIC_FIRST_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		// TODO: language translation
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_PHONETIC_FIRST_NAME_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoPhoneticMiddleNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, PHONETIC_MIDDLE_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		// TODO: language translation
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_PHONETIC_MIDDLE_NAME_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoPhoneticLastNameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, PHONETIC_LAST_NAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		// TODO: language translation
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_PHONETIC_LAST_NAME_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoNicknameChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, NICKNAME_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_NICKNAME_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoCompanyChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, COMPANY_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_COMPANY_ABB");
	}
}

void CtInputViewBasicInfoItem::__basicInfoJobTitleChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__entryTextChanged( obj, JOB_TITLE_CHANGED );
	item->__itemChangedCb();

	// this code is for fixing the text blink issue when touch expand button
	const char* entryText = elm_entry_entry_get(obj);
	if (!(entryText && *entryText)) {
		elm_object_domain_translatable_part_text_set(obj, "elm.guide", PACKAGE, "IDS_PB_BODY_JOB_TITLE_ABB");
	}
}

void CtInputViewBasicInfoItem::__activatedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	Evas_Object* entry = NULL;
	Evas_Object* box = NULL;
	Eina_List* list = NULL;
	Eina_List* cursor = NULL;
	Evas_Object* layout = NULL;
	bool find = false;
	Evas_Object* focusedEntry = NULL;
	void *tmp = NULL;

	box = elm_object_item_part_content_get(item->getElmObjectItem(), "elm.icon.1");
	if (box) {
		list = elm_box_children_get(box);
		EINA_LIST_FOREACH(list, cursor, tmp) {
			layout = (Evas_Object*)tmp;
			if (layout) {
				entry = elm_object_part_content_get(layout, "elm.icon.1");
				if (find) {
					focusedEntry = entry;
					break;
				}

				if (entry == obj) {
					find = true;
				}
			}
		}
	}

	if (focusedEntry) {
		elm_object_focus_set(focusedEntry, EINA_TRUE);
	}
	else {
		item->__activateNextEntryCb( entry, item->getElmObjectItem() );
	}
}

void CtInputViewBasicInfoItem::__basicInfoSuffixShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	evas_object_event_callback_del_full( obj, EVAS_CALLBACK_SHOW, __basicInfoSuffixShowCb, item );

	Evas_Object* entry = NULL;
	Evas_Object* box = NULL;
	Eina_List* list = NULL;
	Eina_List* cursor = NULL;
	void *tmp = NULL;
	Evas_Object* layout = NULL;
	if (item->__inputData->getContactId() == 0 && item->__updateItem) {
		box = elm_object_item_part_content_get(item->getElmObjectItem(), "elm.icon.1");
		if (box) {
			list = elm_box_children_get(box);
			EINA_LIST_FOREACH(list, cursor, tmp) {
				layout = (Evas_Object*)tmp;
				if (layout) {
					entry = elm_object_part_content_get(layout, "elm.icon.1");
					if (entry) {
						elm_object_focus_set(entry, EINA_TRUE);
						elm_entry_cursor_end_set(entry);
						break;
					}
				}
			}
		}
		item->__updateItem = false;
	}
	item->__updateEntryReturnKeyType(NULL);
}

void CtInputViewBasicInfoItem::__phoneticNameDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__itemDeletedCb( PHONETIC_NAME_DELETED );
	item->__phoneticFirstNameIsEmpty = true;
	item->__phoneticMiddleNameIsEmpty = true;
	item->__phoneticLastNameIsEmpty = true;
	elm_genlist_item_fields_update( item->getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void CtInputViewBasicInfoItem::__nicknameDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__itemDeletedCb( NICKNAME_DELETED );
	item->__nicknameIsEmpty = true;
	elm_genlist_item_fields_update( item->getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

void CtInputViewBasicInfoItem::__organizationDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewBasicInfoItem* item = (CtInputViewBasicInfoItem*)data;
	item->__itemDeletedCb( ORGANIZATION_DELETED );
	item->__companyIsEmpty = true;
	item->__jobTitleIsEmpty = true;
	elm_genlist_item_fields_update( item->getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

bool CtInputViewBasicInfoItem::getIsChanged()
{
	WHIT();
	bool ret = false;

	const char* imagePath = __inputData->getImagePath();
	bool imagePathIsChanged = __compareWithOldVal(imagePath ? imagePath : "", __oldImagePath);

	if (__baseInfoExpanded) {
		if (true == __prefixIsChanged
			|| true == __firstNameIsChanged
			|| true == __middleNameIsChanged
			|| true == __lastNameIsChanged
			|| true == __suffixIsChanged
			|| true == __phoneticFirstNameIsChanged
			|| true == __phoneticMiddleNameIsChanged
			|| true == __phoneticLastNameIsChanged
			|| true == __nicknameIsChanged
			|| true == __companyIsChanged
			|| true == __jobTitleIsChanged
			|| true == imagePathIsChanged) {
			ret = true;
		}
	} else {
		if (true == __fullNameIsChanged
			|| true == __phoneticFirstNameIsChanged
			|| true == __phoneticMiddleNameIsChanged
			|| true == __phoneticLastNameIsChanged
			|| true == __nicknameIsChanged
			|| true == __companyIsChanged
			|| true == __jobTitleIsChanged
			|| true == imagePathIsChanged) {
			ret = true;
		}
	}
	return ret;
}

void CtInputViewBasicInfoItem::setPhoneticNameIsChanged(bool val)
{
	__phoneticFirstNameIsChanged = val;
	__phoneticMiddleNameIsChanged = val;
	__phoneticLastNameIsChanged = val;
}

bool CtInputViewBasicInfoItem::getPhoneticNameIsChanged()
{
	if (__phoneticFirstNameIsChanged || __phoneticMiddleNameIsChanged || __phoneticLastNameIsChanged) {
		return true;
	}
	else {
		return false;
	}
}

void CtInputViewBasicInfoItem::setNicknameIsChanged(bool val)
{
	__nicknameIsChanged = val;
}

bool CtInputViewBasicInfoItem::getNicknameIsChanged()
{
	return __nicknameIsChanged;
}

void CtInputViewBasicInfoItem::setCompanyIsChanged(bool val)
{
	__companyIsChanged = val;
}

bool CtInputViewBasicInfoItem::getCompanyIsChanged()
{
	return __companyIsChanged;
}

void CtInputViewBasicInfoItem::setJobTitleIsChanged(bool val)
{
	__jobTitleIsChanged = val;
}

bool CtInputViewBasicInfoItem::getJobTitleIsChanged()
{
	return __jobTitleIsChanged;
}

bool CtInputViewBasicInfoItem::getIsEmpty()
{
	WHIT();
	bool ret = true;
	if (__baseInfoExpanded) {
		if (false == __prefixIsEmpty
			|| false == __firstNameIsEmpty
			|| false == __middleNameIsEmpty
			|| false == __lastNameIsEmpty
			|| false == __suffixIsEmpty
			|| false == __phoneticFirstNameIsEmpty
			|| false == __phoneticMiddleNameIsEmpty
			|| false == __phoneticLastNameIsEmpty
			|| false == __nicknameIsEmpty
			|| false == __companyIsEmpty
			|| false == __jobTitleIsEmpty) {
			ret = false;
		}
	} else {
		if (false == __fullNameIsEmpty
			|| false == __phoneticFirstNameIsEmpty
			|| false == __phoneticMiddleNameIsEmpty
			|| false == __phoneticLastNameIsEmpty
			|| false == __nicknameIsEmpty
			|| false == __companyIsEmpty
			|| false == __jobTitleIsEmpty) {
			ret = false;
		}
	}
	return ret;
}

bool CtInputViewBasicInfoItem::getAddPhoneticName()
{
	return __addPhoneticName;
}

void CtInputViewBasicInfoItem::setAddPhoneticName(bool addPhoneticName)
{
	__addPhoneticName = addPhoneticName;
}

bool CtInputViewBasicInfoItem::getAddNickname()
{
	return __addNickname;
}

void CtInputViewBasicInfoItem::setAddNickname(bool addNickname)
{
	__addNickname = addNickname;
}

bool CtInputViewBasicInfoItem::getAddOrganization()
{
	return __addOrganization;
}

void CtInputViewBasicInfoItem::setAddOrganization(bool addOrganization)
{
	__addOrganization = addOrganization;
}

void CtInputViewBasicInfoItem::setAddedIndex(int addedIndex)
{
	__addedIndex = addedIndex;
}

void CtInputViewBasicInfoItem::onRealized()
{
	__isRealized = true;
	if (__focusEntry) {
		elm_object_focus_set(__focusEntry, EINA_TRUE);
		__focusEntry = NULL;
		__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
	}

	// Set focus in the Name field in case of Create contact
	if (__showCreateKeypadCb && *__nameEntry) {
		__showCreateKeypadCb( *__nameEntry );
		__showCreateKeypadCb = nullptr;
	}
}

void CtInputViewBasicInfoItem::onUnrealized()
{
	__isRealized = false;
	__focusEntry = NULL;
}
