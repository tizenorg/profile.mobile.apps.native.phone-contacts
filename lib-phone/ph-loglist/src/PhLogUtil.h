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

#ifndef _PH_LOG_UTIL_H_
#define _PH_LOG_UTIL_H_

#include <string>
#include "PhLogDbMgr.h"

#define V_(str) dgettext("contacts", str)

class PhLogUtil
{
public:
	PhLogUtil();
	virtual ~PhLogUtil();

	static Evas_Object* getLogTypeIcon(Evas_Object *parent, int log_type);
	static std::string getLogTypeImageName(int log_type);

	static const char *getTextOfNumberType(int number_type);
	static const char* getDisplayText(int personId, const char* displayName, const char* number, const char *descInfo);

	static bool isGrouped(const PhLogDbMgr::RecordInfo *prevRecordInfo, const PhLogDbMgr::RecordInfo *curRecordInfo);
	static bool isGroupedbyDate(const int time1, const int time2);
	static bool isGroupedByDesc(const char *prevDesc, const char *curDesc);

	static bool isUnknownNumber(const char *callDesc);
	static bool isEmergencyNumber(const char *callDesc);

	static bool isMsgType(int type);
	static bool isVideoCallType(int type);
	static bool isVoiceCallType(int type);
	static bool isMissedCallType(int type);
	static bool isRejectCallType(int type);
	static bool isOutgoingCallType(int type);
	static bool isIncomingCallType(int type);
private:
	static bool isOutgoingMsgType(int type);
	static bool isIncomingMsgType(int type);
	static bool isBlockedType(int type);

	static bool isGroupedbyType(int pre_type, int type);
	static bool isGroupedbyNumber(const char * preNumber, const char *curNumber);
};

#endif /* _PH_LOG_UTIL_H_ */
