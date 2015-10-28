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

#include <app.h>
#include <notification.h>
#include <system_settings.h>
#include <app_preference.h>

#include "WPopup.h"
#include "WMenuPopup.h"
#include "WUiTimer.h"

#include "ContactsAppControl.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "ContactsNumberItemIcons.h"

#include "PhCommon.h"
#include "PhPath.h"

#include "PhLogUtil.h"
#include "PhLogIcu.h"
#include "PhLogDefines.h"

#include "PhLogDetailCheckView.h"
#include "PhLogDetailView.h"
#include "SystemSettingsMgr.h"

PhLogDetailView::PhLogDetailView(const PhLogDbMgr::RecordInfo *recordInfo):
__mainLayout(NULL),
__itcLogInfo(NULL),
__itcGroupIndex(NULL),
__itcHistoryList(NULL),
__detailList(NULL),
__contactInfo(NULL),
__recordInfo(recordInfo)
{
	WDEBUG("detailView for Number[%s], callDesc[%s]", recordInfo->number, recordInfo->callDesc);

	if(recordInfo->number)
		__number = recordInfo->number;

	if(recordInfo->callDesc)
		__callDesc = recordInfo->callDesc;
}

PhLogDetailView::~PhLogDetailView()
{
	WHIT();

	if (__contactInfo) {
		edje_object_signal_callback_del(_EDJ(__contactInfo->getEvasObj()), "mouse,clicked,*", "touch.back.btn",  __detailBackBtnCb);
	}

	STD_LIST_FREE(__detailList)

	elm_genlist_item_class_free(__itcLogInfo);

	elm_genlist_item_class_free(__itcGroupIndex);
	elm_genlist_item_class_free(__itcHistoryList);
}

void PhLogDetailView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();
	WPRET_M( !naviItem, "Invalid param : naviItem");

	elm_naviframe_item_title_enabled_set(naviItem, EINA_FALSE, EINA_FALSE);
	WUiTimer::addAnimator([naviItem](void* data)->Eina_Bool {
			PhLogDetailView* p = (PhLogDetailView*)data;
			p->__setMoreBtn(naviItem);
			return ECORE_CALLBACK_CANCEL;
		}, this, getWeakPtr());
}

void PhLogDetailView::__detailBackBtnCb(void *data, Evas_Object *obj, const char *emission, const char *source) {
	PhLogDetailView* view = (PhLogDetailView*)data;
	view->popOut();
}

Evas_Object* PhLogDetailView::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();
	__mainLayout = PhCommon::createLayout( parent, LOG_LIST_EDJ, "log/detail/info/contact/main_layout" );

	WPRET_VM( NULL == __mainLayout, NULL, "PhCommon::createLayout() is failed");
	__setGenlistItcs();

	return __mainLayout;
}

void PhLogDetailView::onCreated()
{
	WHIT();
	PhLogListView::onCreated();
	updateList();
}

void PhLogDetailView::onDestroy()
{
	PhLogListView::onDestroy();
}

void PhLogDetailView::onDbChanged()
{
	WENTER();
	updateList();
}

void PhLogDetailView::updateList()
{
	Evas_Object *genlist = getGlHelper()->getEvasObj();

	elm_object_part_content_set(__mainLayout, "elm.swallow.content", genlist);


	std::list<PhLogDbMgr::RecordInfo*> *__oldList = __detailList;
	__detailList = PhLogDbMgr::getInstance()->loadDetailList( __number.c_str(), __callDesc.c_str());

	if(!__detailList || __detailList->empty())
		setNoContentLayout();
	else
	{
		setGenlistLayout();
		__setContactInfo();
		__setList();
	}

	STD_LIST_FREE(__oldList)
}

void PhLogDetailView::__setMoreBtn(Elm_Object_Item *naviItem)
{
	WHIT();
	WPRET_M(!naviItem, "Invalid Param : naviItem");

	enableMoreButton(naviItem, [](void *data, Evas_Object *obj, void *event_info){
		WPRET_M( NULL == data, "Invalid Parm : data");
		PhLogDetailView* p = static_cast<PhLogDetailView*>(data);
		WPRET_M( NULL == p->__detailList || p->__detailList->empty() , "__detaiilList is Empty");

		WMenuPopup* popup = new WMenuPopup();
		popup->prepare( p->getWindow()->getEvasObj(), p->getNaviframe()->getEvasObj() );

		popup->appendItem(T_("IDS_LOGS_HEADER_DELETE"), [p](){
			WNaviframe* frame = (WNaviframe*)p->getNaviframe();
			frame->push(new PhLogDetailCheckView(p->__number, p->__callDesc));
		});

		p->attachPopup(popup);
	}, this);
}

void PhLogDetailView::__setContactInfo()
{
	__contactInfo = new PhLogDetailContactInfoControl(__detailList->front());
	__contactInfo->create(__mainLayout, NULL);

	edje_object_signal_callback_add(_EDJ(__contactInfo->getEvasObj()), "mouse,clicked,*", "touch.back.btn",  __detailBackBtnCb, this);
	elm_object_part_content_set(__mainLayout, "contact_info", __contactInfo->getEvasObj() );
}

void PhLogDetailView::__setList()
{
	WHIT();
	WPRET_M( !__detailList || __detailList->empty(), "__detailList is not set");

	Elm_Object_Item* item = NULL;
	PhLogDbMgr::RecordInfo *recordInfo = NULL;
	PhLogDbMgr::RecordInfo *prevRecordInfo = NULL;
	for (auto it=__detailList->begin(); it != __detailList->end(); it++)
	{
		recordInfo = (PhLogDbMgr::RecordInfo*)(*it);
		if( it == __detailList->begin() )
		{
			if(recordInfo->number && strlen(recordInfo->number) > 0)
			{
				item = getGlHelper()->appendItem(__itcLogInfo, recordInfo, NULL, ELM_GENLIST_ITEM_NONE, NULL, this);
				elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
			}
		}

		if( !prevRecordInfo || !PhLogUtil::isGroupedbyDate(prevRecordInfo->logTime,recordInfo->logTime) ) {
			item = getGlHelper()->appendItem(__itcGroupIndex, recordInfo, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
		}

		item = getGlHelper()->appendItem(__itcHistoryList, recordInfo, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);

		prevRecordInfo = recordInfo;
	}

	getGlHelper()->appendFinish();
}

void PhLogDetailView::__setGenlistItcs()
{
	__itcLogInfo = elm_genlist_item_class_new();
	WPRET_M(!__itcLogInfo, "elm_genlist_item_class_new() failed");
	__itcLogInfo->item_style = "type1";
	__itcLogInfo->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*{
	PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	PhLogDetailView *p = (PhLogDetailView*)itemData->p;

		if (!strcmp(part, "elm.swallow.icon")) {
			Evas_Object *layout = PhCommon::createLayout(obj, LOG_LIST_EDJ, "list/detail/log_number.type");
			WPRET_VM( NULL == layout, NULL, "PhCommon::createLayout() is failed");

			char *text = NULL;
			if ( recordInfo->personId > 0 )
				text = elm_entry_utf8_to_markup(PhLogUtil::getTextOfNumberType(recordInfo->numberType));
			else if ( p->__number.empty() && PhLogUtil::isUnknownNumber(recordInfo->callDesc) )
				text = NULL;
			else if ( !p->__number.empty() && PhLogUtil::isEmergencyNumber(recordInfo->callDesc) )
				text = elm_entry_utf8_to_markup(T_("IDS_KPD_HEADER_EMERGENCY_CALL_ABB2"));
			else
				text = elm_entry_utf8_to_markup(T_("IDS_LOGS_SBODY_UNSAVED_M_STATUS"));
			elm_object_domain_translatable_part_text_set(layout, "elm.text.part", PACKAGE, text);
			free(text);
			text = NULL;

			Evas_Object *entry = elm_entry_add(layout);
			WPRET_VM( NULL == entry, NULL, "entry is NULL");
			elm_entry_scrollable_set(entry, EINA_TRUE);
			elm_entry_single_line_set(entry, EINA_TRUE);
			elm_entry_editable_set(entry, EINA_FALSE);

			if (p->__number.empty() && PhLogUtil::isUnknownNumber(recordInfo->callDesc))
				text = elm_entry_utf8_to_markup(T_("IDS_LOGS_MBODY_UNKNOWN_NUMBER_ABB"));
			else
				text = elm_entry_utf8_to_markup(recordInfo->number);
			elm_entry_entry_set(entry, text);
			free(text);

			elm_layout_content_set(layout, "elm.swallow.2", entry);

			return layout;
		} else if (!strcmp(part, "elm.swallow.end")) {
			return createNumberItemIconsLayout(obj, p->__number.c_str());
		} else
			return NULL;
	};
	__itcLogInfo->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::ItemData* itemData = (PhLogGenlistHelper::ItemData *)data;
		delete itemData;
	};

	__itcGroupIndex = elm_genlist_item_class_new();
	WPRET_M(!__itcGroupIndex, "elm_genlist_item_class_new() failed");
	__itcGroupIndex->item_style = "group_index";
	__itcGroupIndex->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
//		WDEBUG("part :%s", part);
		if (!strcmp(part, "elm.text")) {
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;

			time_t itemTime = recordInfo->logTime;
			time_t nowTime = time(NULL);

			struct tm itemDate;
			struct tm nowDate;

			localtime_r(&itemTime, &itemDate);
			localtime_r(&nowTime, &nowDate);

			if( itemDate.tm_year == nowDate.tm_year &&
				itemDate.tm_mon == nowDate.tm_mon &&
				itemDate.tm_mday == nowDate.tm_mday )
				return elm_entry_utf8_to_markup(T_("IDS_LOGS_HEADER_TODAY"));
			else if( itemDate.tm_year == nowDate.tm_year &&
					 itemDate.tm_mon == nowDate.tm_mon &&
					 ((itemDate.tm_mday)+1) == nowDate.tm_mday )
				return elm_entry_utf8_to_markup(T_("IDS_LOGS_HEADER_YESTERDAY"));
			else
				return PhLogIcu::getGroupIndexDateString(recordInfo->logTime);
		}
		else
			return NULL;
	};
	__itcGroupIndex->func.content_get = NULL;
	__itcGroupIndex->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::ItemData* itemData = (PhLogGenlistHelper::ItemData *)data;
		delete itemData;
	};

	__itcHistoryList = elm_genlist_item_class_new();
	WPRET_M(!__itcHistoryList, "elm_genlist_item_class_new() failed");
	__itcHistoryList->item_style = "type1";
	__itcHistoryList->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;

			if (!strcmp(part, "elm.text")) {
				return PhLogIcu::getTimeString(recordInfo->logTime);
			} else if (!strcmp(part, "elm.text.sub")) {
				if(PhLogUtil::isMissedCallType(recordInfo->logType)) {
					return strdup(T_("IDS_LOGS_SBODY_MISSEDM_CALL_STATUS"));
				} else if(PhLogUtil::isOutgoingCallType(recordInfo->logType) || PhLogUtil::isIncomingCallType(recordInfo->logType)) {
					if (recordInfo->callDuration > 0) {
						char *duration = PhLogIcu::getCallDurationString(recordInfo->callDuration);
						const char *type = PhLogUtil::isIncomingCallType(recordInfo->logType)
								? T_("IDS_CALL_BODY_INCOMING_CALL") : T_("IDS_ST_BODY_OUTGOING_CALL");

						if (duration) {
							char buf[PH_TEXT_SHORT_LEN];
							snprintf(buf, sizeof(buf), "%s, %s",type, duration);
							free(duration);
							return strdup(buf);
						} else {
							return strdup(type);
						}
					} else {
						return strdup(T_("IDS_LOGS_SBODY_CANCELLED_M_STATUS"));
					}
				} else if(PhLogUtil::isRejectCallType(recordInfo->logType)) {
					return strdup(T_("IDS_LOGS_SBODY_CANCELLED_M_STATUS"));
				} else {
					return PhLogIcu::getCallDurationString(recordInfo->callDuration);
				}
			}
			else
				return NULL;
		};
	__itcHistoryList->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*	{
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;

			if (!strcmp(part, "elm.swallow.icon")) {
				Evas_Object *icon = PhLogUtil::getLogTypeIcon(obj, recordInfo->logType);
				WPRET_VM( NULL == icon, NULL, "PhLogUtil::getLogTypeIcon() is failed");
				elm_image_aspect_fixed_set(icon, EINA_TRUE);

				evas_object_size_hint_min_set(icon, ContactsCommon::getScaledWidth(ICON_LOG_DETAIL_WIDTH), ContactsCommon::getScaledHeight(ICON_LOG_DETAIL_HEIGHT));
				return icon;
			}
			else {
				return NULL;
			}

		};
	__itcHistoryList->func.state_get = NULL;
	__itcHistoryList->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::ItemData* itemData = (PhLogGenlistHelper::ItemData *)data;
		delete itemData;
	};
}
