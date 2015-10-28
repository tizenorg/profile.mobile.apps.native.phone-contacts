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

#include "ContactsDebug.h"
#include "ContactsLocalization.h"

#include "PhCommon.h"
#include "PhPath.h"

#include "PhLogUtil.h"

enum{
	CALL_VC_SERVICE_TYPE_NONE,
	CALL_VC_SERVICE_TYPE_EMERGENCY=01,
	CALL_VC_SERVICE_TYPE_VOICE_MAIL,
	CALL_VC_SERVICE_TYPE_UNAVAILABLE,
	CALL_VC_SERVICE_TYPE_REJECT_BY_USER,
	CALL_VC_SERVICE_TYPE_OTHER_SERVICE,
	CALL_VC_SERVICE_TYPE_PAYPHONE,
	CALL_VC_SERVICE_TYPE_DCM_REMOTE_OPERATE,
	CALL_VC_SERVICE_TYPE_DCM_CALL_FORWARD,
	CALL_VC_SERVICE_TYPE_DCM_CALLER_ID,
	CALL_VC_SERVICE_TYPE_DCM_ROAMING_GUIDE,
	CALL_VC_SERVICE_TYPE_DCM_ROAMING_CALL_NOTI,
	CALL_VC_SERVICE_TYPE_MAX
};

PhLogUtil::PhLogUtil()
{
	// TODO Auto-generated constructor stub

}

PhLogUtil::~PhLogUtil()
{
	// TODO Auto-generated destructor stub
}

bool PhLogUtil::isEmergencyNumber(const char *callDesc)
{
	if (!callDesc) return false;

	int desc = atoi(callDesc+1);
	WDEBUG("callDesc[0]:%c, desc : %d",callDesc[0], desc);

	switch(desc)
	{
		case CALL_VC_SERVICE_TYPE_EMERGENCY:
			return true;
		case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
		case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
		case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
		case CALL_VC_SERVICE_TYPE_PAYPHONE:
		case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
		default:
			return false;
	}

	return false;
}

bool PhLogUtil::isUnknownNumber(const char *callDesc)
{
	if (!callDesc) return true;

	int desc = atoi(callDesc+1);
	WDEBUG("callDesc[0]:%c, desc : %d",callDesc[0], desc);

	switch(desc)
	{
		case CALL_VC_SERVICE_TYPE_EMERGENCY:
		case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
		case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
		case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
		case CALL_VC_SERVICE_TYPE_PAYPHONE:
			return false;
		case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
		default:
			return true;
	}

	return false;
}

const char* PhLogUtil::getDisplayText(int personId, const char* displayName, const char* number, const char *descInfo)
{
	if(number)
	{
		if( personId > 0 )
			return displayName;

		if (!descInfo)
			return number;

		int desc = atoi(descInfo+1);
		WDEBUG("descInfo[0]:%c, desc : %d",descInfo[0], desc);

		switch(desc)
		{
			case CALL_VC_SERVICE_TYPE_EMERGENCY:
				return T_("IDS_KPD_HEADER_EMERGENCY_CALL_ABB2");
			case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
				return T_("IDS_KPD_OPT_VOICEMAIL_ABB");
			case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
				return T_("IDS_LOGS_MBODY_UNKNOWN");
			case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
				return T_("IDS_COM_BODY_PRIVATE_NUMBER");
			case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
				return T_("IDS_LOGS_MBODY_UNKNOWN");
			case CALL_VC_SERVICE_TYPE_PAYPHONE:
				return T_("IDS_CALL_BODY_PAYPHONE");
			default:
				return number;
		}

		return number;
	}
	else
	{
		if (!descInfo)
		return T_("IDS_COM_BODY_PRIVATE_NUMBER");

		int desc = atoi(descInfo+1);
		WDEBUG("descInfo[0]:%c, desc : %d",descInfo[0], desc);

		switch(desc)
		{
			case CALL_VC_SERVICE_TYPE_EMERGENCY:
				return T_("IDS_KPD_HEADER_EMERGENCY_CALL_ABB2");
			case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
				return T_("IDS_KPD_OPT_VOICEMAIL_ABB");
			case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
				return T_("IDS_LOGS_MBODY_UNKNOWN");
			case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
				return T_("IDS_COM_BODY_PRIVATE_NUMBER");
			case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
				return T_("IDS_LOGS_MBODY_UNKNOWN");
			case CALL_VC_SERVICE_TYPE_PAYPHONE:
				return T_("IDS_CALL_BODY_PAYPHONE");
			default:
				return T_("IDS_LOGS_MBODY_UNKNOWN");
		}

		return NULL;
	}
}

const char *PhLogUtil::getTextOfNumberType(int numberType)
{
	WDEBUG("numberType : %d", numberType);

	if (numberType & CONTACTS_NUMBER_TYPE_CUSTOM) {
		return T_("IDS_PB_OPT_CUSTOM");
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_CELL) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME)
			return T_("IDS_PB_OPT_HOME_ABB");
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK)
			return T_("IDS_PB_OPT_WORK");
		else
			return T_("IDS_PB_OPT_MOBILE");
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_VOICE) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME)
			return T_("IDS_PB_OPT_HOME_ABB");
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK)
			return T_("IDS_PB_OPT_WORK");
		else
			return T_("IDS_PB_BODY_TELEPHONE");
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_FAX) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME)
			return T_("IDS_PB_OPT_FAX_HHOME_ABB");
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK)
			return T_("IDS_PB_OPT_FAX_HWORK_ABB");
		else
			return T_("IDS_PB_BODY_FAX");
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_PAGER)
		return T_("IDS_PB_OPT_PAGER");
	else if (numberType & CONTACTS_NUMBER_TYPE_ASSISTANT)
		return T_("IDS_PB_OPT_ASSISTANT_M_CONTACTS");
	else {
		if (numberType == CONTACTS_NUMBER_TYPE_HOME)
			return T_("IDS_PB_OPT_HOME_ABB");
		else if (numberType == CONTACTS_NUMBER_TYPE_WORK)
			return T_("IDS_PB_OPT_WORK");
		else
			return T_("IDS_PB_OPT_OTHER");
	}
}

Evas_Object* PhLogUtil::getLogTypeIcon(Evas_Object *parent, int log_type)
{
	if ( PhLogUtil::isOutgoingCallType(log_type) || PhLogUtil::isOutgoingMsgType(log_type))
		return PhCommon::createImage(parent, LOG_LIST_EDJ, PH_LOG_ICON_OUTGOING);
	else if (PhLogUtil::isIncomingCallType(log_type) || PhLogUtil::isIncomingMsgType(log_type))
		return PhCommon::createImage(parent, LOG_LIST_EDJ, PH_LOG_ICON_INCOMING);
	else if ( PhLogUtil::isMissedCallType(log_type))
		return PhCommon::createImage(parent, LOG_LIST_EDJ, PH_LOG_ICON_MISSED);
	else if (PhLogUtil::isRejectCallType(log_type))
		return PhCommon::createImage(parent, LOG_LIST_EDJ, PH_LOG_ICON_REJECTED);
	else if (PhLogUtil::isBlockedType(log_type))
		return PhCommon::createImage(parent, LOG_LIST_EDJ, PH_LOG_ICON_AUTO_REJECTED);
	else
	{
		WDEBUG("error invalid type %d", log_type);
		return NULL;
	}
}

std::string PhLogUtil::getLogTypeImageName(int log_type)
{
	if ( PhLogUtil::isOutgoingCallType(log_type) || PhLogUtil::isOutgoingMsgType(log_type))
		return PH_LOG_ICON_OUTGOING;
	else if (PhLogUtil::isIncomingCallType(log_type) || PhLogUtil::isIncomingMsgType(log_type))
		return PH_LOG_ICON_INCOMING;
	else if ( PhLogUtil::isMissedCallType(log_type))
		return PH_LOG_ICON_MISSED;
	else if (PhLogUtil::isRejectCallType(log_type))
		return PH_LOG_ICON_REJECTED;
	else if (PhLogUtil::isBlockedType(log_type))
		return PH_LOG_ICON_AUTO_REJECTED;
	else
	{
		WDEBUG("error invalid type %d", log_type);
		return "";
	}
}

bool PhLogUtil::isOutgoingCallType(int type)
{
	if (type == CONTACTS_PLOG_TYPE_VOICE_OUTGOING
			|| type == CONTACTS_PLOG_TYPE_VIDEO_OUTGOING)
		return TRUE;
	else
		return FALSE;
}

bool PhLogUtil::isMsgType(int type)
{
	if (isOutgoingMsgType(type) || isIncomingMsgType(type))
		return true;
	else
		return false;
}

bool PhLogUtil::isOutgoingMsgType(int type)
{
	if (type == CONTACTS_PLOG_TYPE_MMS_OUTGOING
			|| type == CONTACTS_PLOG_TYPE_SMS_OUTGOING
			|| type == CONTACTS_PLOG_TYPE_EMAIL_SENT)
		return TRUE;
	else
		return FALSE;

}

bool PhLogUtil::isIncomingCallType(int type)
{
	if (type == CONTACTS_PLOG_TYPE_VOICE_INCOMMING
			|| type == CONTACTS_PLOG_TYPE_VIDEO_INCOMMING)
		return TRUE;
	else
		return FALSE;
}

bool PhLogUtil::isIncomingMsgType(int type)
{
	if (type == CONTACTS_PLOG_TYPE_MMS_INCOMMING
			|| type == CONTACTS_PLOG_TYPE_SMS_INCOMMING
			|| type == CONTACTS_PLOG_TYPE_EMAIL_RECEIVED)
		return TRUE;
	else
		return FALSE;
}

bool PhLogUtil::isMissedCallType(int type)
{
	switch (type) {
	case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN:
	case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN:
	case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN:
	case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_SEEN:
		return TRUE;
	default:
		return FALSE;
	}
}

bool PhLogUtil::isRejectCallType(int type)
{
	if (type == CONTACTS_PLOG_TYPE_VOICE_REJECT
			|| type == CONTACTS_PLOG_TYPE_VIDEO_REJECT)
		return TRUE;
	else
		return FALSE;
}

bool PhLogUtil::isBlockedType(int type)
{
	switch (type) {
	case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
	case CONTACTS_PLOG_TYPE_VIDEO_BLOCKED:
	case CONTACTS_PLOG_TYPE_SMS_BLOCKED:
	case CONTACTS_PLOG_TYPE_MMS_BLOCKED:
		return TRUE;
	default:
		return FALSE;
	}
}

bool PhLogUtil::isGroupedByDesc(const char *prevDesc, const char *curDesc)
{
//	WDEBUG("%s:%s", prevDesc, curDesc);
	if (!prevDesc)
	{
		if(!curDesc)
			return true;
		else
		{
//			WDEBUG("curDesc[%s]",curDesc);
			int curDescNum = atoi(curDesc+1);

			switch(curDescNum)
			{
				case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
					return true;
				case CALL_VC_SERVICE_TYPE_EMERGENCY:
				case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
				case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
				case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
				case CALL_VC_SERVICE_TYPE_PAYPHONE:
				default:
					return false;
			}
		}
	}
	else
	{
		if(!curDesc)
		{
//			WDEBUG("prevDesc[%s]",prevDesc);
			int prevDescNum = atoi(prevDesc+1);

			switch(prevDescNum)
			{
				case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
					return true;
				case CALL_VC_SERVICE_TYPE_EMERGENCY:
				case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
				case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
				case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
				case CALL_VC_SERVICE_TYPE_PAYPHONE:
				default:
					return false;
			}
		}

		int prevDescNum = atoi(prevDesc+1);
		int curDescNum = atoi(curDesc+1);
//		WDEBUG("descInfo[0]:%c, desc : %d",prevDesc[0], prevDescNum);
//		WDEBUG("descInfo[0]:%c, desc : %d",curDesc[0], curDescNum);

		switch(prevDescNum)
		{
			case CALL_VC_SERVICE_TYPE_EMERGENCY:
				switch(curDescNum)
				{
					case CALL_VC_SERVICE_TYPE_EMERGENCY:
						return true;
					default:
						return false;
				}
				break;
			case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
				switch(curDescNum)
				{
					case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
						return true;
					default:
						return false;
				}
				break;
			case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
				switch(curDescNum)
				{
					case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
						return true;
					default:
						return false;
				}
				break;
			case CALL_VC_SERVICE_TYPE_PAYPHONE:
				switch(curDescNum)
				{
					case CALL_VC_SERVICE_TYPE_PAYPHONE:
						return true;
					default:
						return false;
				}
				break;
			case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
			case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
			default:
				switch(curDescNum)
				{
					case CALL_VC_SERVICE_TYPE_EMERGENCY:
					case CALL_VC_SERVICE_TYPE_VOICE_MAIL:
					case CALL_VC_SERVICE_TYPE_REJECT_BY_USER:
					case CALL_VC_SERVICE_TYPE_PAYPHONE:
						return false;
					case CALL_VC_SERVICE_TYPE_UNAVAILABLE:
					case CALL_VC_SERVICE_TYPE_OTHER_SERVICE:
					default:
						return true;
				}
				break;
		}
	}
}

bool PhLogUtil::isGroupedbyDate(const int time1, const int time2)
{
	time_t date1 = time1;
	time_t date2 = time2;

	struct tm* ptr;

	ptr = localtime(&date1);
	WPRET_VM(!ptr, false, "localtime returned NULL" );
	struct tm prev = *ptr;

	ptr = localtime(&date2);
	WPRET_VM(!ptr, false, "localtime returned NULL" );
	struct tm cur = *ptr;

	return prev.tm_year == cur.tm_year &&
		 prev.tm_mon == cur.tm_mon &&
		 prev.tm_mday == cur.tm_mday;

}

bool PhLogUtil::isGroupedbyType(int pre_type, int type)
{
	switch (pre_type) {
		case CONTACTS_PLOG_TYPE_VOICE_INCOMMING:
		case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING:
		case CONTACTS_PLOG_TYPE_VOICE_OUTGOING:
		case CONTACTS_PLOG_TYPE_VIDEO_OUTGOING:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_VOICE_INCOMMING:
			case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING:
			case CONTACTS_PLOG_TYPE_VOICE_OUTGOING:
			case CONTACTS_PLOG_TYPE_VIDEO_OUTGOING:
				return true;
			default :
				return false;
			}
			break;
		}
		case CONTACTS_PLOG_TYPE_MMS_INCOMMING:
		case CONTACTS_PLOG_TYPE_SMS_INCOMMING:
		case CONTACTS_PLOG_TYPE_MMS_OUTGOING:
		case CONTACTS_PLOG_TYPE_SMS_OUTGOING:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_MMS_INCOMMING:
			case CONTACTS_PLOG_TYPE_SMS_INCOMMING:
			case CONTACTS_PLOG_TYPE_MMS_OUTGOING:
			case CONTACTS_PLOG_TYPE_SMS_OUTGOING:
				return true;
			default :
				return false;
			}
			break;
		}
		case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN:
		case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN:
		case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN:
		case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_SEEN:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN:
			case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN:
			case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN:
			case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_SEEN:
				return true;
			default :
				return false;
			}
			break;
		}
		case CONTACTS_PLOG_TYPE_VOICE_REJECT:
		case CONTACTS_PLOG_TYPE_VIDEO_REJECT:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_VOICE_REJECT:
			case CONTACTS_PLOG_TYPE_VIDEO_REJECT:
				return true;
			default :
				return false;
			}
			break;
		}
		case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
		case CONTACTS_PLOG_TYPE_VIDEO_BLOCKED:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
			case CONTACTS_PLOG_TYPE_VIDEO_BLOCKED:
				return true;
			default :
				return false;
			}
			break;
		}
		case CONTACTS_PLOG_TYPE_SMS_BLOCKED:
		{
			switch (type) {
			case CONTACTS_PLOG_TYPE_SMS_BLOCKED:
				return true;
			default :
				return false;
			}
			break;
		}
		default:
			return false;
	}
}

bool PhLogUtil::isGroupedbyNumber(const char * preNumber, const char *curNumber)
{
	if (!preNumber && curNumber)
		return false;

	if (preNumber && !curNumber)
		return false;

	if ( preNumber && curNumber && ( strcmp( preNumber, curNumber) ) )
		return false;

	return true;
}

bool PhLogUtil::isGrouped(const PhLogDbMgr::RecordInfo *prevRecordInfo, const PhLogDbMgr::RecordInfo *curRecordInfo)
{
	WPRET_VM( NULL == prevRecordInfo, false, "Invalid parameter: prevRecordInfo is NULL" );
	WPRET_VM( NULL == curRecordInfo, false, "Invalid parameter: curRecordInfo is NULL" );

	if ( !isGroupedbyType( prevRecordInfo->logType, curRecordInfo->logType ))
		return false;
	if ( !isGroupedbyNumber(prevRecordInfo->number, curRecordInfo->number))
		return false;
	if( !isGroupedbyDate( prevRecordInfo->logTime, curRecordInfo->logTime ))
		return false;

	return true;
}

bool PhLogUtil::isVideoCallType(int type)
{
	switch (type) {
	case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING:
	case CONTACTS_PLOG_TYPE_VIDEO_OUTGOING:
	case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_UNSEEN:
	case CONTACTS_PLOG_TYPE_VIDEO_INCOMMING_SEEN:
	case CONTACTS_PLOG_TYPE_VIDEO_REJECT:
	case CONTACTS_PLOG_TYPE_VIDEO_BLOCKED:
		return TRUE;
	default:
		return FALSE;
	}
}

bool PhLogUtil::isVoiceCallType(int type)
{
	switch (type) {
	case CONTACTS_PLOG_TYPE_VOICE_INCOMMING:
	case CONTACTS_PLOG_TYPE_VOICE_OUTGOING:
	case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN:
	case CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN:
	case CONTACTS_PLOG_TYPE_VOICE_REJECT:
	case CONTACTS_PLOG_TYPE_VOICE_BLOCKED:
		return TRUE;
	default:
		return FALSE;
	}
}
