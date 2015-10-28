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

#include "PhCommon.h"
#include "PhLogDbMgr.h"
#include "ContactsDebug.h"
#include "PhLogUtil.h"
#include "PhLogIcu.h"

#include <vector>

PhLogDbMgr::RecordInfo::RecordInfo()
{
	displayName = NULL;
	imgPath = NULL;
	number = NULL;
	callDesc = NULL;

	logId = 0;
	personId = 0;
	numberType = 0;
	logTime = 0;
	logType = 0;
	callDuration = 0;

	groupedList = NULL;
};
PhLogDbMgr::RecordInfo::~RecordInfo()
{
	SAFE_FREE(displayName);
	SAFE_FREE(imgPath);
	SAFE_FREE(number);
	SAFE_FREE(callDesc);

	delete groupedList;
};

PhLogDbMgr *PhLogDbMgr::getInstance()
{
	static PhLogDbMgr __instance("LogDbMgr");
	return &__instance;
}

PhLogDbMgr::PhLogDbMgr(const char* name)
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_db_add_changed_cb(_contacts_person._uri, onNotify, this);
	if (CONTACTS_ERROR_NONE != err) WWARNING("contacts_db_remove_changed_cb() Failed(%d)", err);
	err = contacts_db_add_changed_cb(_contacts_phone_log._uri, onNotify, this);
	if (CONTACTS_ERROR_NONE != err) WWARNING("contacts_db_remove_changed_cb() Failed(%d)", err);
}

PhLogDbMgr::~PhLogDbMgr()
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_db_remove_changed_cb(_contacts_person._uri, onNotify, this);
	if (CONTACTS_ERROR_NONE != err) WWARNING("contacts_db_remove_changed_cb() Failed(%d)", err);
//	err = contacts_db_remove_changed_cb_with_info(_contacts_phone_log._uri, onNotify, this);
	err = contacts_db_remove_changed_cb(_contacts_phone_log._uri, onNotify, this);
	if (CONTACTS_ERROR_NONE != err) WWARNING("contacts_db_remove_changed_cb() Failed(%d)", err);
}

unsigned int PhLogDbMgr::__getDetailListDataFromDb(contacts_list_h* ctsList, const char* number)
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	int recordCnt = 0;

	do{
		if (CONTACTS_ERROR_NONE != (err = contacts_query_create(_contacts_person_phone_log._uri, &query))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_create( _contacts_person_phone_log._uri, &filter))) break;

		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING)))break;
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND)))break;
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED)))break;

		if (SAFE_STRLEN(number)>0)
		{
			WHIT();
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND)))break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_str(filter, _contacts_person_phone_log.address, CONTACTS_MATCH_EXACTLY, number)))break;
		}

		if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter)))break;
		if (CONTACTS_ERROR_NONE != (err = contacts_query_set_sort(query, _contacts_person_phone_log.log_time, false)))break;

		if(ctsList != NULL)
		{
			if (CONTACTS_ERROR_NONE != (err = contacts_db_get_records_with_query(query, 0, 0, ctsList)))break;
			if (CONTACTS_ERROR_NONE != (err = contacts_list_get_count(*ctsList, &recordCnt)))break;
		}
		else
		{
			if (CONTACTS_ERROR_NONE != (err = contacts_db_get_count_with_query(query, (int *)(&recordCnt))))break;;
			WDEBUG("get count! (%d records)", recordCnt);
		}
	}while(0);

	if (query)
		contacts_query_destroy(query);
	if (filter)
		contacts_filter_destroy(filter);

	WDEBUG("get db records (ctsList: 0x%x, count:%d, error:%d)", ctsList, recordCnt, err);
	return recordCnt;

}

unsigned int PhLogDbMgr::__getLogListDataFromDb(contacts_list_h* ctsList, ListType type, int limit)
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	int recordCnt = 0;

	int logTypeMin = CONTACTS_PLOG_TYPE_NONE;
	int logTypeMax = CONTACTS_PLOG_TYPE_NONE;
	err = contacts_query_create(_contacts_person_phone_log._uri, &query);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_query_create() Failed(%d)", err);

	err = contacts_filter_create( _contacts_person_phone_log._uri, &filter);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_filter_create() Failed(%d)", err);
		err = contacts_query_destroy(query);
		if(CONTACTS_ERROR_NONE != err)
			WWARNING("contacts_query_destroy() Failed(%d)", err);
		return 0;
	}

	switch (type)
	{
	case LIST_TYPE_MISSED_CALL:
		logTypeMin = CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN;
		logTypeMax = CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_SEEN;
		break;
	case LIST_TYPE_ALL_CALL:
		logTypeMin = CONTACTS_PLOG_TYPE_VOICE_INCOMMING;
		logTypeMax = CONTACTS_PLOG_TYPE_VIDEO_BLOCKED;
		break;
	default:
		logTypeMin = CONTACTS_PLOG_TYPE_VOICE_INCOMMING;
		logTypeMax = CONTACTS_PLOG_TYPE_MMS_BLOCKED;
		break;
	}

	do{
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, logTypeMin)))break;
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND)))break;
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, logTypeMax)))break;
	}while(0);

	if (CONTACTS_ERROR_NONE == err)
	{
		WHIT();
		do{
			if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter)))break;
			if (CONTACTS_ERROR_NONE != (err = contacts_query_set_sort(query, _contacts_person_phone_log.log_time, false)))break;

			// if output param NULL, just count the records
			if(ctsList != NULL)
			{
				if (CONTACTS_ERROR_NONE != (err = contacts_db_get_records_with_query(query, 0, limit, ctsList)))break;
				if (CONTACTS_ERROR_NONE != (err = contacts_list_get_count(*ctsList, &recordCnt)))break;
			}
			else
			{
				if (CONTACTS_ERROR_NONE != (err = contacts_db_get_count_with_query(query, (int *)(&recordCnt))))break;;
				WDEBUG("get count! (%d records)", recordCnt);
			}
		}while(0);
	}

	if (query)
		contacts_query_destroy(query);
	if (filter)
		contacts_filter_destroy(filter);

	WDEBUG("get db records (ctsList: 0x%x, type:%d, count:%d, error:%d)", ctsList, type, recordCnt, err);
	return recordCnt;

}

bool PhLogDbMgr::deleteLogList(std::list<RecordInfo*> *logList, int totalCount)
{
	WHIT();

	WPRET_VM( NULL == logList, false, "Invalid parameter(data) is NULL");

	int err = CONTACTS_ERROR_NONE;
	std::vector<int> logIdArray;
	logIdArray.reserve(totalCount);

	for (auto &selectedRecordInfo : *logList) {
		logIdArray.push_back(selectedRecordInfo->logId);

		if (selectedRecordInfo->groupedList) {
			for (auto &groupedRecordInfo : *selectedRecordInfo->groupedList) {
				logIdArray.push_back(groupedRecordInfo->logId);
			}
		}
	}

	err = contacts_db_delete_records(_contacts_phone_log._uri, logIdArray.data(), logIdArray.size());
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_delete_records() Failed(%d)", err);

	return true;
}

std::list<PhLogDbMgr::RecordInfo*> *PhLogDbMgr::loadDetailList(const char *searchNumber, const char *searchDesc)
{
	WENTER();
	WDEBUG("searchNumber[%s], searchDesc[%s]", searchNumber, searchDesc);

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h ctsList = NULL;
	contacts_record_h record = NULL;
	__getDetailListDataFromDb(&ctsList, searchNumber);
	WPRET_VM(ctsList == NULL, NULL, "__getLogListDataFromDb Failed.");

	std::list<RecordInfo*>* list = new std::list<RecordInfo*>;
	do{
		err = contacts_list_get_current_record_p(ctsList, &record);
		if(CONTACTS_ERROR_NONE != err){
			WERROR("contacts_list_get_current_record_p() Failed(%d)", err);
			break;
		}

		char *tmpNumber = NULL;
		err = contacts_record_get_str_p(record, _contacts_person_phone_log.address, &tmpNumber);
		if(CONTACTS_ERROR_NONE != err){
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		char * tempDesc = NULL;
		err = contacts_record_get_str_p(record, _contacts_person_phone_log.extra_data2, &tempDesc);
		if(CONTACTS_ERROR_NONE != err){
			WERROR("contacts_record_get_int() Failed(%d)", err);
			break;
		}

		if( !( SAFE_STRLEN(searchNumber) > 0 ) )
		{
			if ( tmpNumber || !PhLogUtil::isGroupedByDesc(searchDesc, (const char*)tempDesc)) {
				err = contacts_list_next(ctsList);
				if (CONTACTS_ERROR_NONE != err) {
					WERROR("contacts_list_next() Failed(%d)", err);
					break;
				}
				continue;
			}
		}

		RecordInfo *recordInfo = new RecordInfo;
		recordInfo->number = SAFE_STRDUP(tmpNumber);
		recordInfo->callDesc = SAFE_STRDUP(tempDesc);
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_id, &recordInfo->logId);
		err = contacts_record_get_int(record, _contacts_person_phone_log.person_id, &recordInfo->personId);
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_time, &recordInfo->logTime);
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_type, &recordInfo->logType);
		err = contacts_record_get_int(record, _contacts_person_phone_log.extra_data1, &recordInfo->callDuration);
		err = contacts_record_get_int(record, _contacts_person_phone_log.address_type, &recordInfo->numberType);
		err = contacts_record_get_str(record, _contacts_person_phone_log.display_name, &recordInfo->displayName);
		err = contacts_record_get_str(record, _contacts_person_phone_log.image_thumbnail_path, &recordInfo->imgPath);

		if(PhLogUtil::isVoiceCallType(recordInfo->logType) || PhLogUtil::isVideoCallType(recordInfo->logType))
			recordInfo->logTime -= recordInfo->callDuration;

		list->push_back(recordInfo);

		err = contacts_list_next(ctsList);
		if(CONTACTS_ERROR_NONE != err)
		{
			WERROR("contacts_list_next() Failed(%d)", err);
			break;
		}
	}while (1);

	contacts_list_destroy(ctsList, true);

	return list;
}

std::list<PhLogDbMgr::RecordInfo*> *PhLogDbMgr::loadLogList(ListType listType, int limit /* = 0 */ )
{
	WENTER();

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h ctsList;
	contacts_record_h record = NULL;
	std::list<RecordInfo*>* list = new std::list<RecordInfo*>;

	__requestedListType = listType;
	__requestedIsUnknownIncluded = true;

	__getLogListDataFromDb(&ctsList, __requestedListType, limit);

	if (ctsList == NULL) {
			WERROR("__getLogListDataFromDb Failed.");
			delete list;
			return NULL;
		}
	do{
		err = contacts_list_get_current_record_p(ctsList, &record);
		if(CONTACTS_ERROR_NONE != err){
			WERROR("contacts_list_get_current_record_p() Failed(%d)", err);
			break;
		}

		char *number = NULL;
		err = contacts_record_get_str_p(record, _contacts_person_phone_log.address, &number);
		if(CONTACTS_ERROR_NONE != err){
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		if (__requestedIsUnknownIncluded == false && (!number || !*number)) {
			err = contacts_list_next(ctsList);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_list_next() Failed(%d)", err);
				break;
			}
			continue;
		}

		RecordInfo *recordInfo = new RecordInfo;
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_id, &recordInfo->logId);
		err = contacts_record_get_int(record, _contacts_person_phone_log.person_id, &recordInfo->personId);
		recordInfo->number = SAFE_STRDUP(number);
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_time, &recordInfo->logTime);
		err = contacts_record_get_int(record, _contacts_person_phone_log.log_type, &recordInfo->logType);
		err = contacts_record_get_int(record, _contacts_person_phone_log.extra_data1, &recordInfo->callDuration);
		err = contacts_record_get_int(record, _contacts_person_phone_log.address_type, &recordInfo->numberType);
		err = contacts_record_get_str(record, _contacts_person_phone_log.display_name, &recordInfo->displayName);
		err = contacts_record_get_str(record, _contacts_person_phone_log.image_thumbnail_path, &recordInfo->imgPath);
		err = contacts_record_get_str(record, _contacts_person_phone_log.extra_data2, &recordInfo->callDesc);

		if(PhLogUtil::isVoiceCallType(recordInfo->logType) || PhLogUtil::isVideoCallType(recordInfo->logType))
			recordInfo->logTime -= recordInfo->callDuration;
		recordInfo->displayLogTime = PhLogIcu::getTimeString(recordInfo->logTime);

		list->push_back(recordInfo);

		err = contacts_list_next(ctsList);
		if(CONTACTS_ERROR_NONE != err)
		{
			WERROR("contacts_list_next() Failed(%d)", err);
			break;
		}
	}while (1);

	contacts_list_destroy(ctsList, true);

	WLEAVE();
	return list;
}

void PhLogDbMgr::updateMissedLogDbAsSeen(int index, int type)
{
	WENTER();
	int logType = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h logRecord = NULL;

	WPRET_M(CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN != type &&
			CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN != type &&
			CONTACTS_PLOG_TYPE_NONE != type,
			"The type is invalid. It must be CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN"
			" or CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN or none");

	WDEBUG("update phonelogs table records(index: %d, log_type:%d)",index, type);
	if (0 == index) {
		contacts_list_h ctsList = NULL;
		contacts_filter_h filter = NULL;
		contacts_query_h query = NULL;
		int recordCnt = 0;

		err = contacts_query_create(_contacts_phone_log._uri, &query);
		WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_query_create() Failed(%d)", err);
		err = contacts_filter_create(_contacts_phone_log._uri, &filter);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_filter_create() Failed(%d)", err);
			contacts_query_destroy(query);
			return;
		}

		do {
			if (CONTACTS_PLOG_TYPE_NONE == type)
			{
				if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN)))break;
				if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_OR)))break;
				if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN)))break;
				if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter)))break;
			}
			else
			{
				if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_phone_log.log_type, CONTACTS_MATCH_EQUAL, type)))break;
				if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter)))break;
			}

			if (CONTACTS_ERROR_NONE != (err = contacts_db_get_records_with_query(query, 0, 0, &ctsList)))
			{
				err = contacts_list_destroy(ctsList, true);
				if(CONTACTS_ERROR_NONE != err)
					WWARNING("contacts_list_destroy() result : error(%d)", err);
				break;
			}
			if (CONTACTS_ERROR_NONE != (err = contacts_list_get_count(ctsList, &recordCnt)))break;

			if (0 < recordCnt)
			{
				while(CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(ctsList, &logRecord)) {
					err = contacts_record_get_int(logRecord, _contacts_phone_log.log_type, &logType);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
					err = contacts_record_set_int( logRecord, _contacts_phone_log.log_type, logType+1);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_int() Failed(%d)", err);
					contacts_list_next(ctsList);
				}

				WDEBUG("mard_as_read(%d)", recordCnt);
				ecore_thread_run([](void *data, Ecore_Thread *thread)
				{
					WDEBUG("BEGIN(thread) - mark_as_read");
					contacts_list_h ctsList = (contacts_list_h)data;

					contacts_db_update_records(ctsList);
					contacts_list_destroy(ctsList, true);

					WDEBUG("LEAVE(thread) - mark_as_read");
				}, NULL, NULL, ctsList);
			}
			else
			{
				contacts_list_destroy(ctsList, true);
				break;
			}
		}while(0);

		if (query)
			contacts_query_destroy(query);
		if (filter)
			contacts_filter_destroy(filter);
	}
	else {
		if (CONTACTS_ERROR_NONE != (err = contacts_db_get_record( _contacts_phone_log._uri, index, &logRecord)))
		{
			err = contacts_record_get_int( logRecord, _contacts_phone_log.log_type, &logType );
			if (CONTACTS_ERROR_NONE != err) {
				WDEBUG("contacts_record_get_int() Failed(%d)", err);
			}
			err = contacts_record_set_int( logRecord, _contacts_phone_log.log_type, logType+1);
			if (CONTACTS_ERROR_NONE != err) {
				WDEBUG("contacts_record_set_int() Failed(%d)", err);
			}
			err = contacts_db_update_record(logRecord);
			if (CONTACTS_ERROR_NONE != err) {
				WDEBUG("contacts_db_update_record() Failed(%d)", err);
			}
			contacts_record_destroy(logRecord, true);
		}
	}
	WDEBUG("contacts_service err : %d", err);
	return;
}

void PhLogDbMgr::attach(PhDbListener *listener)
{
	WASSERT_EX(listener, "no listener");
	WDEBUG("attach Listener: %p", listener);
	std::list<PhDbListener*>::iterator it;
	for (it=__listenerList.begin(); it!=__listenerList.end(); it++)
	{
		if (*it == listener)
		{
			__listenerList.erase(it);
			break;
		}
	}
	__listenerList.push_back(listener);
}

void PhLogDbMgr::detach(PhDbListener *listener)
{
	WASSERT_EX(listener, "no listener");
	WDEBUG("detach Listener: %p", listener);
	std::list<PhDbListener*>::iterator it;

	for (it=__listenerList.begin(); it!=__listenerList.end(); it++)
	{
		if (*it == listener)
		{
			__listenerList.erase(it);
			break;
		}
	}
}

void PhLogDbMgr::onNotify(const char* view_uri, void* data)
{
	WENTER();
	WPRET_M(NULL == data , "Invalid Param : data");
	PhLogDbMgr* p = (PhLogDbMgr*) data;

	std::list<PhDbListener*> copyList(p->__listenerList);
	std::list<PhDbListener*>::iterator it;
	for (it=copyList.begin(); it!=copyList.end(); it++)
		(*it)->onDbChanged();
}


