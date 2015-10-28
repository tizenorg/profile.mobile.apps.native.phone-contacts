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

#ifndef __CT_LIST_DATA_PERSON_H_
#define __CT_LIST_DATA_PERSON_H_

#include <Eina.h>
#include <string>
#include <contacts.h>


typedef enum {
	CT_LIST_DATA_VCARD  = 1 << 0,
	CT_LIST_DATA_PERSON = 1 << 1,
	CT_LIST_DATA_NUMBER = 1 << 2,
	CT_LIST_DATA_EMAIL  = 1 << 3,
	CT_LIST_DATA_ALL = CT_LIST_DATA_NUMBER | CT_LIST_DATA_EMAIL,
}CtListItemDataType;

class CtListDataPerson
{
public:
	CtListDataPerson(contacts_record_h record);
	virtual ~CtListDataPerson();

public:
//getter
	CtListDataPerson* makeClone();
	contacts_record_h getPersonRecord();
	const char* getDisplayName();
	const char* getImagePath();
	void setImagePath(const char *imagePath);
	int getPersonId();
	int getLinkedCount();
	const char* getDisplayIndex();
	bool isChecked();
	void setAsChecked(bool checked);
	// for ug
	Eina_List* getNumberList();
	Eina_List* getEmailList();
	const char* getDataString();
	int getDataType();
	int getDataId();
	void setDataString(const char *data);
	void setDataType(int dataType);
	void setDataId(int dataId);
	int getAddressbookId();

	bool getFavorite();
	bool getSearchNumber(const char *searchStr, std::string &numberStr);

private:
	void __initialize();
	void __setPersonRecord(contacts_record_h record);
	Eina_List* __getDataList(int personId, const char* viewUri);
	//bool __checkMFC(int personId);
private:
	contacts_record_h __person;
	char* __displayName;
	char* __imagePath;
	char* __displayIndex;
	bool __favorite;
	int __personId;
	int __addressbookId;

	std::string __searchStr;
	std::string __searchNumber;


	// TODO: review required
	bool __checked;
	// for ug
	char *__dataString;
	int __dataType;
	int __dataId;
	int __linkedCount;

};

#endif
