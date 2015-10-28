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

#ifndef _CT_INPUT_VIEW_BASIC_INFO_ITEM_H_
#define _CT_INPUT_VIEW_BASIC_INFO_ITEM_H_

#include <functional>
#include "CtCommon.h"
#include "CtInputViewGenlistItem.h"
#include "CtInputViewControl.h"
#include "CtData.h"

class CtInputView;
class CtInputData;

class CtInputViewBasicInfoItem : public CtInputViewGenlistItem
{
public:
	enum ITEM_DELETED
	{
		PHONETIC_NAME_DELETED,
		NICKNAME_DELETED,
		ORGANIZATION_DELETED,
	};

	enum ITEM_CHANGED
	{
		NAME_CHANGED,
		PREFIX_CHANGED,
		FIRST_NAME_CHANGED,
		MIDDLE_NAME_CHANGED,
		LAST_NAME_CHANGED,
		SUFFIX_CHANGED,
		PHONETIC_FIRST_NAME_CHANGED,
		PHONETIC_MIDDLE_NAME_CHANGED,
		PHONETIC_LAST_NAME_CHANGED,
		NICKNAME_CHANGED,
		COMPANY_CHANGED,
		JOB_TITLE_CHANGED
	};
	CtInputViewBasicInfoItem(std::shared_ptr<CtData>& inputData, Evas_Object** nameEntry,
			const std::function<void ()>& itemChangedCb,
			const std::function<void (CtInputViewBasicInfoItem::ITEM_DELETED)>& itemDeletedCb,
			const std::function<void (Evas_Object*)>& imageClickedCb,
			const std::function<void (Evas_Object*, Elm_Object_Item* )>& activateNextEntryCb,
			const std::function<void (Evas_Object*)>& showCreateKeypadCb,
			const std::function<void (Elm_Object_Item* )>& updateEntryReturnKeyType);

	virtual ~CtInputViewBasicInfoItem();

public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();

	virtual bool getIsChanged();
	virtual bool getIsEmpty();
	void setPhoneticNameIsChanged(bool val);
	bool getPhoneticNameIsChanged();
	void setNicknameIsChanged(bool val);
	bool getNicknameIsChanged();
	void setCompanyIsChanged(bool val);
	bool getCompanyIsChanged();
	void setJobTitleIsChanged(bool val);
	bool getJobTitleIsChanged();

	bool getAddPhoneticName();
	void setAddPhoneticName(bool addPhoneticName);
	bool getAddNickname();
	void setAddNickname(bool addNickname);
	bool getAddOrganization();
	void setAddOrganization(bool addOrganization);
	void setAddedIndex(int addedIndex);

	virtual void onRealized();
	virtual void onUnrealized();
private:
	void __initialize();

	static Evas_Object* __getContentBasicInfo(void *data, Evas_Object *obj, const char *part);
	static Evas_Object* __createNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createPrefixField( CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createFirstNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createMiddleNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createLastNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createSuffixField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createPhoneticFirstNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createPhoneticMiddleNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createPhoneticLastNameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createNicknameField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createCompanyField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static Evas_Object* __createJobTitleField(CtInputViewBasicInfoItem* item, Evas_Object *parent);
	static void __set_entry_text(Evas_Object *entry, const char *text);
	static Evas_Object* __createEditField(Evas_Object *parent, const char *guideText, bool multiline,
			Evas_Smart_Cb unfocusedCallback, void *userData1, Evas_Smart_Cb focusedCallback, void *userData2);
	static void __onEditfieldTextMaxReached(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoExpandClickedCb(void *data, Evas_Object *obj, void *event_info);

	static void __basicInfoImageClickedCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static void __subItemResizeCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static void __onFontChangedCb(system_settings_key_e key, void *user_data);

	bool __compareWithOldVal(const char* text, const char* oldVal);
	void __entryTextChanged( Evas_Object* obj, int changedInfo );
	static void __basicInfoNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoPrefixChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoFirstNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoMiddleNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoLastNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoSuffixChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoPhoneticFirstNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoPhoneticMiddleNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoPhoneticLastNameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoNicknameChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoCompanyChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoJobTitleChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __activatedCb(void *data, Evas_Object *obj, void *event_info);
	static void __basicInfoNameShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static void __basicInfoSuffixShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static void __phoneticNameDeleteCb(void *data, Evas_Object *obj, void *event_info);
	static void __nicknameDeleteCb(void *data, Evas_Object *obj, void *event_info);
	static void __organizationDeleteCb(void *data, Evas_Object *obj, void *event_info);
private:
	CtInputViewBasicInfoItem();

	int __addedIndex;
	std::shared_ptr<CtData> __inputData;
	Evas_Object** __nameEntry;
	Evas_Object* __focusEntry;

	std::function<void ()> __itemChangedCb;
	std::function<void (ITEM_DELETED)> __itemDeletedCb;

	std::function<void (Evas_Object*)> __imageClickedCb;
	std::function<void (Evas_Object*, Elm_Object_Item*)> __activateNextEntryCb;
	std::function<void (Evas_Object*)> __showCreateKeypadCb;
	std::function<void (Elm_Object_Item*)> __updateEntryReturnKeyType;

	bool __oldBaseInfoExpanded;
	bool __baseInfoExpanded;

	bool __fullNameIsChanged;
	bool __prefixIsChanged;
	bool __firstNameIsChanged;
	bool __middleNameIsChanged;
	bool __lastNameIsChanged;
	bool __suffixIsChanged;
	bool __phoneticFirstNameIsChanged;
	bool __phoneticMiddleNameIsChanged;
	bool __phoneticLastNameIsChanged;
	bool __nicknameIsChanged;
	bool __companyIsChanged;
	bool __jobTitleIsChanged;

	bool __fullNameIsEmpty;
	bool __prefixIsEmpty;
	bool __firstNameIsEmpty;
	bool __middleNameIsEmpty;
	bool __lastNameIsEmpty;
	bool __suffixIsEmpty;
	bool __phoneticFirstNameIsEmpty;
	bool __phoneticMiddleNameIsEmpty;
	bool __phoneticLastNameIsEmpty;
	bool __nicknameIsEmpty;
	bool __companyIsEmpty;
	bool __jobTitleIsEmpty;

	char* __oldFullName;
	char* __oldPrefix;
	char* __oldFirstName;
	char* __oldMiddleName;
	char* __oldLastName;
	char* __oldSuffix;
	char* __oldPhoneticFirstName;
	char* __oldPhoneticMiddleName;
	char* __oldPhoneticLastName;
	char* __oldNickname;
	char* __oldCompany;
	char* __oldJobTitle;
	char* __oldImagePath;

	bool __addPhoneticName;
	bool __addNickname;
	bool __addOrganization;
	bool __updateItem;
	bool __isRealized;

	Evas_Coord __subItemSize;
};

#endif /* _CT_INPUT_VIEW_BASIC_INFO_ITEM_H_ */
