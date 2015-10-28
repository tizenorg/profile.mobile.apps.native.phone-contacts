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

#ifndef _PH_LOG_DB_MGR_H_
#define _PH_LOG_DB_MGR_H_

#include <list>
#include <string>

#include <contacts.h>

#include "PhDbListener.h"

/**
 * @brief Provides access to the database logs.
 */
class PhLogDbMgr
{
public:
	/**
	 * @brief Gets the instance of PhLogDbMgr.
	 *
	 * @return instance.
	 */
	static PhLogDbMgr *getInstance();

public:
	/**
	 * @brief Logs list type.
	 */
	typedef enum {
		LIST_TYPE_MISSED_CALL,
		LIST_TYPE_ALL_CALL,//more_popup
	}ListType;

	/**
	 * @brief Provides log record info.
	 */
	typedef struct RecordInfo
	{
		RecordInfo();
		~RecordInfo();

		char *displayName;
		char *imgPath;
		char *number;
		char *callDesc;
		char *displayLogTime;

		int logId;
		int personId;
		int numberType;
		int logTime;
		int logType;
		int callDuration;

		std::list<PhLogDbMgr::RecordInfo*> *groupedList;
	}RecordInfo;

	/**
	 * @brief Loads specified log list from database.
	 *
	 * @param[in]   listType    The type of the logs list to be returned
	 * @param[in]   limit       Maximum number of logs to load or 0 to get all logs
	 *
	 * @return log list.
	 *
	 * @see ListType
	 * @see RecordInfo
	 */
	std::list<PhLogDbMgr::RecordInfo*> *loadLogList(ListType listType, int limit = 0);

	/**
	 * @brief Loads @a searchNumber's log list from database.
	 *
	 * @param[in]   searchNumber    The number for which log list will be loaded
	 * @param[in]   searchDesc      Search description
	 *
	 * @return log list.
	 *
	 * @see ListType
	 * @see RecordInfo
	 */
	std::list<PhLogDbMgr::RecordInfo*> *loadDetailList(const char *searchNumber, const char *searchDesc);

	/**
	 * @brief Deletes logs from database.
	 *
	 * @param[in]   logList     The log list to be deleted
	 * @param[in]   totalCount  The number of logs to delete.
	 *
	 * @return true if logs were deleted.
	 *
	 * @see RecordInfo
	 */
	bool deleteLogList(std::list<RecordInfo*> *logList, int totalCount);

	/**
	 * @brief Updates missed logs (from unseen to seen) in database.
	 *
	 * @param[in]   index   The log id to be updated
	 * @param[in]   type    The type of logs to be updated.
	 */
	void updateMissedLogDbAsSeen(int index, int type);

	/**
	 * @brief Attaches listener to listeners list.
	 *
	 * @param[in]   listener    The listener to be attached.
	 *
	 * @see PhDbListener
	 */
	void attach(PhDbListener *listener);

	/**
	 * @brief Detaches listener from listeners list.
	 *
	 * @param[in]   listener    The listener to be detached.
	 *
	 * @see PhDbListener
	 */
	void detach(PhDbListener *listener);

private:
	PhLogDbMgr(const char* name);
	virtual ~PhLogDbMgr();

	unsigned int __getLogListDataFromDb(contacts_list_h* ctsList, ListType type, int limit);
	unsigned int __getDetailListDataFromDb(contacts_list_h* ctsList, const char* number);

	static void onNotify(const char* view_uri, void* data);

	ListType __requestedListType;
	bool __requestedIsUnknownIncluded;
	std::list<PhDbListener*> __listenerList;
};

#endif //_PH_LOG_DB_MGR_H_

