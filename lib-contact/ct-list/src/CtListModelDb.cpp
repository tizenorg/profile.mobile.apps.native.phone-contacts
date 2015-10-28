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

#include <contacts.h>
#include <app_preference.h>
#include <algorithm>

#include "ContactsDebug.h"
#include "CtListModelDb.h"
#include "CtListDataPerson.h"
#include "ContactsCommon.h"
#include "CtCommon.h"
#include "CtListModelFilter.h"

#define MAX_MFC_LIST 5

CtListModelDb::CtListModelDb(CtListHasType filterType, int limit)
{
	WENTER();
	__initialize();
	__dataFilter = new CtListModelFilter(filterType);
	__dataFilter->attachListener(this);
	__limitCount = limit;
}

CtListModelDb::CtListModelDb(CtListDataType dataType)
{
	WENTER();
	__initialize();
	__dataFilter = new CtListModelFilter(dataType);
	__dataFilter->attachListener(this);
	__dataType = dataType; //Tdod set dataFilter
}

CtListModelDb::~CtListModelDb()
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;

	if(__idler)
		ecore_idler_del(__idler);

	if(__updateMFCListIdler)
		ecore_idler_del(__updateMFCListIdler);

	if(__listThread)
		__listThread->destroy();

	if(__mfcThread)
		__mfcThread->destroy();

	delete __dataFilter;

	if(__listener) {
		err = contacts_db_remove_changed_cb( _contacts_person._uri, __onDbChanged ,this);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);

		err = contacts_db_remove_changed_cb( _contacts_my_profile._uri, __onDbChanged ,this);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);

		err = contacts_db_remove_changed_cb( _contacts_phone_log._uri, __onDbChanged ,this);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);

		err = contacts_setting_remove_name_display_order_changed_cb( __onDisplayNameOrderChangedCb ,this);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_setting_remove_name_display_order_changed_cb() Failed(%d)", err);

		err = contacts_setting_remove_name_sorting_order_changed_cb( __onNameSortingOrderChangedCb ,this);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_setting_remove_name_sorting_order_changed_cb() Failed(%d)", err);
	}

	__personDataList.clear();
	__checkedDataList.clear();
	__checkedDataListOld.clear();
	__originalCheckedIdList.clear();

}

int CtListModelDb::getContactsCount(CtListQueryType queryType)
{
	int count = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_query_h query = __dataFilter->getQuery(queryType);

	if(query) {
		err = contacts_db_get_count_with_query(query, &count);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_count_with_query() Failed(%d)", err);

		err = contacts_query_destroy(query);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_destroy() Failed(%d)", err);
	}
	else{
		err = contacts_db_get_count(_contacts_person._uri, &count);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_count() Failed(%d)", err);
	}

	WDEBUG("count %d", count);

	return count;
}

int CtListModelDb::getContacts(CtListQueryType queryType, CtGetContactsType syncType)
{
	WENTER();
	WDEBUG("queryType %d, syncType %d", queryType, syncType);

	if(queryType == LIST_QUERY_FAVORITE) {
		__favoritePersonDataList.clear();
		__getFavoritePersonList();
	}
	else if(queryType == LIST_QUERY_MFC) {
		WDEBUG("__dataType %d", __dataType);
		__mfcPersonDataList.clear();
		__getMfcPersonListFromPreference();
	}
	else if(queryType == LIST_QUERY_DEFAULT) {
		__personDataList.clear();

		if(syncType == GET_AS_SYNC_ASYNC){
			__getQueryResult(0, CTLISTDATA_NQUERY_COUNT);
			__offset = CTLISTDATA_NQUERY_COUNT;

			if(__listThread)
				__listThread->destroy();
			__listThread = NULL;
			if(__idler)
				ecore_idler_del(__idler);
			__idler = ecore_idler_add(__onGetContactsIdlerCb, this);

			WLEAVE();
			return getPersonListCount(LIST_QUERY_DEFAULT);
		}
		else if(syncType == GET_AS_SYNC){
			__getQueryResult(0, -1);

			WLEAVE();
			return getPersonListCount(LIST_QUERY_DEFAULT);
		}
		else if(syncType == GET_AS_ASYNC){
			__offset = 0;
			if(__listThread)
				__listThread->destroy();
			__listThread = NULL;
			if(__idler)
				ecore_idler_del(__idler);
			__idler = NULL;

			__listThread = new CtThread(__onGetContactsThreadRunCb, this, __onGetContactsEndCb, this);
			__listThread->start();
		}
	}
	else
		WERROR("no queryType");

	WLEAVE();
	return 0;
}

void CtListModelDb::cancelGetContacts()
{
	WENTER();

	if(__listThread)
		__listThread->destroy();
	__listThread = NULL;
	if(__idler)
		ecore_idler_del(__idler);
	__idler = NULL;

	WLEAVE();
}


std::shared_ptr<CtListDataPerson> CtListModelDb::getAtPersonData(int index, CtListQueryType queryType)
{
	if(queryType == LIST_QUERY_DEFAULT) {
		if(index < (int)__personDataList.size())
			return __personDataList[index].second;
		else
			return NULL;
	}
	else if(queryType == LIST_QUERY_MFC) {
		if(index < (int)__mfcPersonDataList.size())
			return __mfcPersonDataList[index].second;
		else
			return NULL;
	}
	else if(queryType == LIST_QUERY_FAVORITE) {
		if(index < (int)__favoritePersonDataList.size())
			return __favoritePersonDataList[index].second;
		else
			return NULL;
	}
	else
		return NULL;
}

int CtListModelDb::getPersonListCount(CtListQueryType queryType)
{
	int count = 0;
	if(queryType == LIST_QUERY_DEFAULT)
		count = (int)__personDataList.size();
	else if(queryType == LIST_QUERY_MFC)
		count = (int)__mfcPersonDataList.size();
	else if(queryType == LIST_QUERY_FAVORITE)
		count = (int)__favoritePersonDataList.size();

	WDEBUG("count %d", count);

	return count;
}

void CtListModelDb::setSelectedPersonId(int selectedPersonId)
{
	__selectedPersonId = selectedPersonId;
}

bool CtListModelDb::setSearchString(char *search)
{
	WENTER();

	if(SAFE_STRLEN(search)> 0){
		__search = search;
	}
	else{
		__search.clear();
	}

	return true;
}

const char *CtListModelDb::getSearchString(void)
{
	if(__search.length() >0)
		return __search.c_str();
	else
		return NULL;
}

int CtListModelDb::getCheckedDataCount()
{
	WENTER();
	return __checkedDataList.size();
}

void CtListModelDb::onFilterChanged()
{
	WHIT();
	if (__listener)
		__listener->onDbChanged(NULL);
}

void CtListModelDb::attachListener(ICtChangeListener *listener)
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	err = contacts_db_add_changed_cb( _contacts_person._uri, __onDbChanged ,this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() _contacts_person Failed(%d)", err);

	err = contacts_db_add_changed_cb( _contacts_my_profile._uri, __onDbChanged ,this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() _contacts_person Failed(%d)", err);

	err = contacts_db_add_changed_cb( _contacts_phone_log._uri, __onDbChanged ,this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() _contacts_person Failed(%d)", err);

	err = contacts_setting_add_name_display_order_changed_cb(__onDisplayNameOrderChangedCb, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_setting_add_name_display_order_changed_cb() Failed(%d)", err);

	err = contacts_setting_add_name_sorting_order_changed_cb( __onNameSortingOrderChangedCb ,this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_setting_add_name_sorting_order_changed_cb() Failed(%d)", err);

	__listener = listener;

	WLEAVE();
}

void CtListModelDb::pushCheckedData(std::shared_ptr<CtListDataPerson> dataPerson)
{
	__setAsCheck(dataPerson, true, false);
}

void CtListModelDb::removeCheckedData(std::shared_ptr<CtListDataPerson> dataPerson)
{
	__setAsCheck(dataPerson, false, false);
}

void CtListModelDb::doCheckedDataNoRelation(std::shared_ptr<CtListDataPerson> dataPerson, bool check)
{
	__setAsCheck(dataPerson, check, true);
}

std::vector<std::shared_ptr<CtListDataPerson>>* CtListModelDb::getCheckedDataList() const
{
    std::vector<std::shared_ptr<CtListDataPerson>>* checkedDataList(new std::vector<std::shared_ptr<CtListDataPerson>>);
    for (auto iter = __checkedDataList.begin(); iter != __checkedDataList.end(); iter++)
    {
    	checkedDataList->push_back((*iter).second);
    }

    return checkedDataList;
}

int CtListModelDb::getLimitCount()
{
	return __limitCount;
}

void CtListModelDb::__onDbChanged(const char* view_uri, void* user_data)
{
	WENTER();

	CtListModelDb* listModelDb = (CtListModelDb*)user_data;

	if (listModelDb->__listener)
		listModelDb->__listener->onDbChanged(view_uri);

}

Eina_Bool CtListModelDb::__onGetContactsIdlerCb(void* data)
{
	WENTER();
	CtListModelDb* listModel = (CtListModelDb*)data;
	listModel->__idler = NULL;

	if(listModel->__listThread)
		listModel->__listThread->destroy();
	listModel->__listThread = NULL;

	listModel->__listThread = new CtThread(__onGetContactsThreadRunCb, listModel, __onGetContactsEndCb, listModel);
	listModel->__listThread->start();

	return ECORE_CALLBACK_CANCEL;
}

void CtListModelDb::__onGetContactsThreadRunCb(void *data)
{
	WENTER();

	CtListModelDb* listModel = (CtListModelDb*)data;

	listModel->__getQueryResult(listModel->__offset, -1);

	WLEAVE();
}

void CtListModelDb::__onGetContactsEndCb(void *data)
{
	WENTER();

	CtListModelDb* listModel = (CtListModelDb*)data;

	if(listModel->__listThread)
		listModel->__listThread->destroy();
	listModel->__listThread = NULL;

	if (listModel->__listener)
		listModel->__listener->onCompleteGetContacts(listModel->__offset);

}

void CtListModelDb::__initialize(void)
{
	WENTER();
	__dataType = LIST_DATA_DEFAULT;
	__listener = NULL;
	__offset = 0;
	__listThread = NULL;
	__idler = NULL;
	__selectedPersonId = -1;
	__excludeIdList.clear();
	__checkedDataList.clear();
	__checkedDataListOld.clear();
	__search.clear();
	__personDataList.clear();
	__addressbookList.clear();
	__mfcPersonDataList.clear();
	__favoritePersonDataList.clear();
	__originalCheckedIdList.clear();
	__originalPersonDataListSize = 0;
	__limitCount = 0;
	__madePersonList = false;
	__updateMFCListIdler = NULL;
	__mfcThread = NULL;
	__excludeAddressbookId = 0;
	__needUpdate = false;
	__originalCount = 0;
	__curCheckCount = 0;

	WLEAVE();
}

void CtListModelDb::__onDisplayNameOrderChangedCb(contacts_name_display_order_e name_display_order, void* user_data)
{
	WENTER();

	CtListModelDb* listModelDb = (CtListModelDb*)user_data;

	if (listModelDb->__listener)
		listModelDb->__listener->onDbChanged(NULL);

}

void CtListModelDb::__onNameSortingOrderChangedCb(contacts_name_sorting_order_e name_display_order, void* user_data)
{
	WENTER();

	CtListModelDb* listModelDb = (CtListModelDb*)user_data;

	if (listModelDb->__listener)
		listModelDb->__listener->onDbChanged(NULL);

}


int CtListModelDb::__getQueryResult(int offset, int limit)
{
	WENTER();

	WDEBUG("offset : %d, limit : %d",offset, limit);

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h personList = NULL;
	contacts_query_h query = __dataFilter->getQuery(LIST_QUERY_DEFAULT);

	unsigned int search_person_projection[] = {
		_contacts_person.id,
		_contacts_person.display_name,
		_contacts_person.addressbook_ids,
		_contacts_person.has_phonenumber,
		_contacts_person.image_thumbnail_path,
	};

	if(query) {
		if (__search.length() > 0 ) {
			err = contacts_query_set_projection(query, search_person_projection, sizeof(search_person_projection)/sizeof(int));
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_projection() Failed(%d)", err);

			err = contacts_db_search_records_with_query(query, __search.c_str(), offset, limit, &personList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_search_records_with_query() Failed(%d)", err);
		}
		else {
			WDEBUG("contacts_db_get_records_with_query");
			err = contacts_db_get_records_with_query(query, offset, limit, &personList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_records_with_query() Failed(%d)", err);
		}

		err = contacts_query_destroy(query);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_destroy() Failed(%d)", err);
	}
	else{
		if (__search.length() > 0){
			err = contacts_db_search_records(_contacts_person._uri, __search.c_str(), offset, limit, &personList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_search_records() Failed(%d)", err);
		}
		else {
			WDEBUG("contacts_db_get_all_records");
			err = contacts_db_get_all_records(_contacts_person._uri, offset, limit, &personList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);
		}
	}
	int count = 0;
	err = contacts_list_get_count(personList, &count);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_count() Failed(%d)", err);

	WDEBUG("count : %d", count);

	__mutexLock.lock();
	__makePersonList(personList, offset);
	__mutexLock.unlock();

	err = contacts_list_destroy(personList, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);

	WLEAVE();

	return count;
}

bool CtListModelDb::__makePersonList(contacts_list_h list, int offset)
{
	WENTER();
	WPRET_VM(list == NULL, false, "list is NULL");

	if(offset == 0) {
		__curCheckCount = 0;
		__personDataList.clear();
		__checkedDataListOld.clear();
		__checkedDataListOld.insert(__checkedDataList.begin(), __checkedDataList.end());
		if (__search.length() == 0) { // on searching, no change check data
			__checkedDataList.clear();
		}
	}

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h recordPerson = NULL;
	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &recordPerson)) {
		if(recordPerson){
			std::shared_ptr<CtListDataPerson> dataPerson(std::make_shared<CtListDataPerson>(recordPerson));

			int personId = 0;
			err = contacts_record_get_int(recordPerson, _contacts_person.id, &personId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
			if (__selectedPersonId == personId) {
				err = contacts_list_next(list);
				continue;
			}
			if(std::find(__excludeIdList.begin(), __excludeIdList.end(), personId) != __excludeIdList.end()) {
				err = contacts_list_next(list);
				continue;
			}

			//reserve check state
			std::map<int, std::shared_ptr<CtListDataPerson>>::iterator iter;
			iter = __checkedDataListOld.find(dataPerson->getPersonId());
			if(iter != __checkedDataListOld.end()) {
				dataPerson->setAsChecked(true);
				dataPerson->setDataId((*iter).second->getDataId());
				dataPerson->setDataString((*iter).second->getDataString());
				dataPerson->setDataType((*iter).second->getDataType());
				__setAsCheck(dataPerson, true, false);
				__curCheckCount++;
			}

			if(__excludeAddressbookId > 0) {
				char *value = NULL;
				err = contacts_record_get_str_p(recordPerson, _contacts_person.addressbook_ids, &value);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

				int addressbookId = atoi(value);
				if(addressbookId > 0 )  {
					if(__excludeAddressbookId == addressbookId) {
						WDEBUG("exclude addressbookId %d", addressbookId);
						err = contacts_list_next(list);
						continue;
					}
				}
			}

			__personDataList.push_back(std::make_pair(dataPerson->getPersonId(), dataPerson));
		}
		err = contacts_list_next(list);
	}

	if(__search.length() == 0) {
		__originalCount = __personDataList.size();
	}
	__madePersonList = true;

	return true;
}

bool CtListModelDb::getMadePersonListStatus()
{
	return __madePersonList;
}

bool CtListModelDb::checkMostFrequentContact(int personId)
{
	int mfcPersonCount = __mfcPersonDataList.size();
	if (mfcPersonCount == 0) {
		return false;
	}

	for (auto iter = __mfcPersonDataList.begin(); iter != __mfcPersonDataList.end(); iter++) {
		if (((*iter).second)->getPersonId() == personId) {
			return true;
		}
	}

	return false;
}

Eina_Bool CtListModelDb::__getCurentMfcListIdlerCb(void* data)
{
	WENTER();
	CtListModelDb* listModel = (CtListModelDb*)data;

	if(listModel->__updateMFCListIdler)
		ecore_idler_del(listModel->__updateMFCListIdler);
	listModel->__updateMFCListIdler = NULL;


	if(listModel->__mfcThread)
		listModel->__mfcThread->destroy();
	listModel->__mfcThread = NULL;

	listModel->__mfcThread = new CtThread(__onGetCurrentMfcListThreadRunCb, listModel, __onGetCurrentMfcListThreadEndCb, listModel);
	listModel->__mfcThread->start();

	WLEAVE();

	return ECORE_CALLBACK_CANCEL;
}

void CtListModelDb::__onGetCurrentMfcListThreadRunCb(void* data)
{
	WENTER();
	CtListModelDb* listModel = (CtListModelDb*)data;

	listModel->__getMfcPersonList();

	WLEAVE();
}

void CtListModelDb::__onGetCurrentMfcListThreadEndCb(void* data)
{
	WENTER();
	CtListModelDb* listModel = (CtListModelDb*)data;

	if(listModel->__mfcThread)
		listModel->__mfcThread->destroy();
	listModel->__mfcThread = NULL;

	if(listModel->__needUpdate == true) {
		listModel->__needUpdate = false;
		if(listModel->__listener)
			listModel->__listener->onDbChanged(NULL);
	}

	WLEAVE();
}

int CtListModelDb::__setImage(contacts_record_h record, char *image)
{
	int err = CONTACTS_ERROR_NONE;

	if(image && *image) {
		err = contacts_record_set_str(record, _contacts_image.path, image);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
		//err = contacts_record_set_int(record, _contacts_image.type, CONTACTS_IMAGE_TYPE_EX_CUSTOM);
		err = contacts_record_set_int(record, _contacts_image.type, CONTACTS_IMAGE_TYPE_CUSTOM);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
	}
	return err;
}

bool CtListModelDb::__setMFCImage(contacts_record_h personRecord, char *imagePath){
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h imageRecord = NULL;
	int personId = 0;
	char *thumbnailPath = NULL;
	bool result = false;
	int displayContactId = 0;

	err = contacts_record_get_int(personRecord, _contacts_person.id, &personId);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

	err = contacts_record_get_int(personRecord, _contacts_person.display_contact_id, &displayContactId);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int(displayContactId) Failed(%d)", err);

	WDEBUG("personId %d, imagePath %s", personId, imagePath);

	if(imagePath) {
		err = contacts_record_get_str_p(personRecord, _contacts_person.image_thumbnail_path, &thumbnailPath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

		if(thumbnailPath) {
			WERROR("already image has %s", thumbnailPath);
			return false;
		}

		Eina_List *contactList = ctCommonGetWritableContactList(personId);
		void *data = NULL;
		Eina_List *cursor = NULL;

		EINA_LIST_FOREACH(contactList, cursor, data) {
			contacts_record_h contactRecord = (contacts_record_h)data;

			int contactId = 0;
			err = contacts_record_get_int(contactRecord, _contacts_contact.id, &contactId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

			if(contactId == displayContactId) {
				err = contacts_record_create(_contacts_image._uri, &imageRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d)", err);
				err = contacts_record_set_str(imageRecord, _contacts_image.path, imagePath);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_str() Failed(%d)", err);
//				err = contacts_record_set_int(imageRecord, _contacts_image.type, CONTACTS_IMAGE_TYPE_EX_CUSTOM);
				err = contacts_record_set_int(imageRecord, _contacts_image.type, CONTACTS_IMAGE_TYPE_CUSTOM);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_int() Failed(%d)", err);
				err = contacts_record_add_child_record(contactRecord, _contacts_contact.image, imageRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_add_child_record() Failed(%d)", err);

				err = contacts_db_update_record(contactRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);
				result = true;
				break;
			}
		}

		ctCommonDestroyRecordList(contactList);
		WDEBUG("image set as %s %d", imagePath, result);
		return result;
	}
	else {
		err = contacts_record_get_str_p(personRecord, _contacts_person.image_thumbnail_path, &thumbnailPath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

		if(thumbnailPath == NULL) {
			WERROR("already image absent");
			return false;
		}

		Eina_List *contactList = ctCommonGetWritableContactList(personId);

		void *data = NULL;
		Eina_List *cursor = NULL;

		//set default image to first contact, it is enough
		EINA_LIST_FOREACH(contactList, cursor, data) {
			contacts_record_h contactRecord = (contacts_record_h)data;

			int contactId = 0;
			err = contacts_record_get_int(contactRecord, _contacts_contact.id, &contactId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

			if(contactId == displayContactId) {
				err = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.image, 0, &imageRecord);
				WPWARN(CONTACTS_ERROR_NONE != err,  "contacts_record_get_child_record_at_p() Failed(%d)", err);

				int imageType = 0;
				err = contacts_record_get_int(imageRecord, _contacts_image.type, &imageType);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

//				if(imageType != CONTACTS_IMAGE_TYPE_EX_CUSTOM) {
				if(imageType != CONTACTS_IMAGE_TYPE_CUSTOM) {
					WERROR("not custom default image");
					break;
				}

				err = contacts_record_remove_child_record(contactRecord, _contacts_contact.image, imageRecord);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

				err = contacts_record_destroy(imageRecord, true);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_destroy() Failed(%d)", err);

				err = contacts_db_update_record(contactRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);

				result = true;
				break;
			}
		}

		ctCommonDestroyRecordList(contactList);
		WDEBUG("image removed %d", result);
		return result;
	}

	return true;

}

bool CtListModelDb::__checkToDisplayMfc(char *addressbookIds)
{
	if(addressbookIds) {
		int hideCount = 0;
		int addressbookCount = 0;
		int addressbookId = 0;
		char* ptr = NULL;
		char* token = strtok_r(addressbookIds, " ", &ptr);
		std::map<int, CtAddressbookInfo*> addressbookList = __dataFilter->getAddressbookList();
		while(token) {
			addressbookId = atoi(token);

			if(addressbookList[addressbookId]->display == false)
				hideCount++;
			addressbookCount++;

			token = strtok_r(NULL, " ", &ptr);
		}

		if(hideCount == addressbookCount)
			return false;
	}

	return true;

}


void CtListModelDb::__getMfcPersonListFromPreference()
{
	WENTER();

	int mfc_personid = -1;
	char *pch = NULL;
	char *mfc_personid_list = NULL;
	int error = preference_get_string(CT_KEY_LIST_MFC_PERSON_IDS, &mfc_personid_list);
	WPRET_M(error != PREFERENCE_ERROR_NONE, "preference_get_string failed error code (%d)", error);

	if(__updateMFCListIdler)
		ecore_idler_del(__updateMFCListIdler);
	__updateMFCListIdler = NULL;

	if(NULL == mfc_personid_list) {
		__updateMFCListIdler = ecore_idler_add(__getCurentMfcListIdlerCb, this);
		WLEAVE();
		return;
	}

	WDEBUG("mfc_personid_list %s", mfc_personid_list);
	pch = strtok (mfc_personid_list," ");
	while (pch != NULL) {
		mfc_personid = atoi(pch);
		pch = strtok (NULL, " ");

		contacts_record_h recordPerson = NULL;
		if (CONTACTS_ERROR_NONE == contacts_db_get_record(_contacts_person._uri, mfc_personid,  &recordPerson) ) {
			if(recordPerson){
				if(std::find(__excludeIdList.begin(), __excludeIdList.end(), mfc_personid) != __excludeIdList.end()) {
					continue;
				}

				int err = CONTACTS_ERROR_NONE;
				char *temp = NULL;
				err = contacts_record_get_str_p(recordPerson, _contacts_person.addressbook_ids, &temp);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

				if(temp && __checkToDisplayMfc(temp) == false)
					continue;

				std::shared_ptr<CtListDataPerson> dataPerson(std::make_shared<CtListDataPerson>(recordPerson));

				//reserve check state
				std::map<int, std::shared_ptr<CtListDataPerson>>::iterator iter;
				iter = __checkedDataList.find(dataPerson->getPersonId());
				if(iter != __checkedDataList.end()) {
					dataPerson->setAsChecked(true);
					dataPerson->setDataId((*iter).second->getDataId());
					dataPerson->setDataString((*iter).second->getDataString());
					dataPerson->setDataType((*iter).second->getDataType());
				}

				__mfcPersonDataList.push_back(std::make_pair(dataPerson->getPersonId(), dataPerson));
			}
		}
	}
	free(mfc_personid_list);

	if (__dataType != LIST_DATA_ALL_FAVORITES) {
		__updateMFCListIdler = ecore_idler_add(__getCurentMfcListIdlerCb, this);
	}
}

void CtListModelDb::__getMfcPersonList()
{
	WENTER();

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h personUsageList = NULL;
	contacts_query_h query = __dataFilter->getQuery(LIST_QUERY_MFC);

	err = contacts_db_get_records_with_query(query, 0, 5, &personUsageList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_get_list() Failed(%d)", err);

	contacts_query_destroy(query);

	__makeMfcPersonList(personUsageList);

	err = contacts_list_destroy(personUsageList, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);
	WLEAVE();

}


void CtListModelDb::__getFavoritePersonList()
{
	WENTER();

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h personList = NULL;
	contacts_query_h query = __dataFilter->getQuery(LIST_QUERY_FAVORITE);

	int queryCount = 0;
	err = contacts_db_get_count_with_query(query, &queryCount);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_count_with_query() Failed(%d)", err);
	WDEBUG("count : %d", queryCount);
	if(queryCount == 0) {
		contacts_query_destroy(query);
		return;
	}

	err = contacts_db_get_records_with_query(query, 0, 0, &personList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_get_list() Failed(%d)", err);

	contacts_query_destroy(query);

	__makeFavoritePersonList(personList);

	err = contacts_list_destroy(personList, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);
	WLEAVE();
}

bool CtListModelDb::__makeMfcPersonList(contacts_list_h list)
{
	WENTER();
	WPRET_VM(list == NULL, false, "list is NULL");

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h recordPerson = NULL;
	std::vector<int> mfcPersonIds;

	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &recordPerson)) {
		if(recordPerson){
			int personId = 0;
			err = contacts_record_get_int(recordPerson, _contacts_person.id, &personId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

			contacts_record_h record = NULL;
			if(CONTACTS_ERROR_NONE == contacts_db_get_record(_contacts_person._uri, personId,  &record)) {
				char *temp = NULL;
				err = contacts_record_get_str_p(record, _contacts_person.addressbook_ids, &temp);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

				if(temp && __checkToDisplayMfc(temp) == false) {
					contacts_list_next(list);
					continue;
				}
			}

			mfcPersonIds.push_back(personId);
			if(find_if( __mfcPersonDataList.begin(), __mfcPersonDataList.end(),
					[personId](std::pair<int, std::shared_ptr<CtListDataPerson>> dataPerson)->bool {
					if(dataPerson.first == personId) {
						return true;
					}
					return false;
				}
			) == __mfcPersonDataList.end())
				__needUpdate = true;
		}
		contacts_list_next(list);
	}

	if(__mfcPersonDataList.size() != mfcPersonIds.size()) {
		WDEBUG("count changed old %d, new %d", __mfcPersonDataList.size(), mfcPersonIds.size());
		__needUpdate = true;
	}

	//set mfc to preference no matter chaged
	WDEBUG("MFC are changed");
	std::string mfcPersonIdsStr;
	int index = 0;
	for(index=0;index<(int)mfcPersonIds.size();index++) {
		std::string tempStr = std::to_string(mfcPersonIds[index]);

		if(mfcPersonIdsStr.size() > 0)
			mfcPersonIdsStr += " ";
		mfcPersonIdsStr += tempStr;
	}
	WDEBUG("mfcPersonList %s", mfcPersonIdsStr.c_str());
	preference_set_string(CT_KEY_LIST_MFC_PERSON_IDS, mfcPersonIdsStr.c_str());

	return true;
}

bool CtListModelDb::__makeFavoritePersonList(contacts_list_h list)
{
	WENTER();
	WPRET_VM(list == NULL, false, "list is NULL");

	int err = CONTACTS_ERROR_NONE;

	contacts_record_h recordPerson = NULL;
	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &recordPerson)) {
		if(recordPerson){
			std::shared_ptr<CtListDataPerson> dataPerson(std::make_shared<CtListDataPerson>(recordPerson));

			int personId = 0;
			err = contacts_record_get_int(recordPerson, _contacts_person.id, &personId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

			if(std::find(__excludeIdList.begin(), __excludeIdList.end(), personId) != __excludeIdList.end()) {
				err = contacts_list_next(list);
				continue;
			}
			//reserve check state
			std::map<int, std::shared_ptr<CtListDataPerson>>::iterator iter;
			iter = __checkedDataList.find(dataPerson->getPersonId());
			if(iter != __checkedDataList.end()) {
				dataPerson->setAsChecked(true);
				dataPerson->setDataId((*iter).second->getDataId());
				dataPerson->setDataString((*iter).second->getDataString());
				dataPerson->setDataType((*iter).second->getDataType());
			}

			__favoritePersonDataList.push_back(std::make_pair(dataPerson->getPersonId(), dataPerson));
		}
		err = contacts_list_next(list);
	}
	return true;
}

void CtListModelDb::initialOriginalCheckedIdList()
{
	WENTER();

	__originalCheckedIdList.clear();
	for (auto iter = __checkedDataList.begin(); iter != __checkedDataList.end(); iter++)
	{
		__originalCheckedIdList.push_back((*iter).second->getPersonId());
	}
}

void CtListModelDb::clearOriginalCheckedIdList()
{
	WENTER();
	__originalCheckedIdList.clear();
}

bool CtListModelDb::checkIsSelectAllOnSearching()
{
	WENTER();
	WPRET_VM(0 == __originalCheckedIdList.size(), false, "__originalCheckedIdList is empty");

	int originCheckedCount = __originalCheckedIdList.size();
	int curCheckedCount = __curCheckCount;
	int curCount = __personDataList.size();
	int maxCount = __originalCount;

	WDEBUG("originCheckedCount %d, maxCount %d, curCount %d, curCheckedCount %d", originCheckedCount, maxCount, curCount, curCheckedCount);

	return (curCheckedCount == curCount);
}

void CtListModelDb::initialOriginalPersonDataListSize()
{
	WENTER();
	__originalPersonDataListSize = __personDataList.size();
}

void CtListModelDb::clearOriginalPersonDataListSize()
{
	WENTER();
	__originalPersonDataListSize = 0;
}

int CtListModelDb::getOriginalPersonDataListSize()
{
	WENTER();
	return __originalPersonDataListSize;
}

void CtListModelDb::__setAsCheck(std::shared_ptr<CtListDataPerson> personData, bool check, bool noRelation)
{
	std::map<int, std::shared_ptr<CtListDataPerson>>::iterator iter;

	iter = __checkedDataList.find(personData->getPersonId());

	if(check == true) {
		if(iter == __checkedDataList.end()) {
			__checkedDataList.insert(std::make_pair(personData->getPersonId(), personData));
			__curCheckCount++;
		}

		if(std::find(__originalCheckedIdList.begin(), __originalCheckedIdList.end(), personData->getPersonId()) == __originalCheckedIdList.end()) {
			__originalCheckedIdList.push_back(personData->getPersonId());
		}
	}
	else {
		if(iter != __checkedDataList.end()) {
			__checkedDataList.erase(iter);
			__curCheckCount--;
		}

		std::vector<int>::iterator originalIter = std::find(__originalCheckedIdList.begin(), __originalCheckedIdList.end(), personData->getPersonId());
		if(originalIter != __originalCheckedIdList.end()) {
			__originalCheckedIdList.erase(originalIter);
		}
	}

	//set check other same person
	int personId = personData->getPersonId();
	if(noRelation) {
		//for check all performance, consider default contact based check
		personData->setAsChecked(check);
	}
	else {
		find_if( __personDataList.begin(), __personDataList.end(),
				[personId, check](std::pair<int, std::shared_ptr<CtListDataPerson>> dataPerson)->bool {
				if(dataPerson.first == personId) {
					dataPerson.second->setAsChecked(check);
					return true;
				}
				return false;
			}
		);
	}

	find_if( __mfcPersonDataList.begin(), __mfcPersonDataList.end(),
			[personId, check](std::pair<int, std::shared_ptr<CtListDataPerson>> dataPerson)->bool {
			if(dataPerson.first == personId) {
				dataPerson.second->setAsChecked(check);
				return true;
			}
			return false;
		}
	);

	find_if( __favoritePersonDataList.begin(), __favoritePersonDataList.end(),
			[personId, check](std::pair<int, std::shared_ptr<CtListDataPerson>> dataPerson)->bool {
			if(dataPerson.first == personId) {
				dataPerson.second->setAsChecked(check);
				return true;
			}
			return false;
		}
	);


}
