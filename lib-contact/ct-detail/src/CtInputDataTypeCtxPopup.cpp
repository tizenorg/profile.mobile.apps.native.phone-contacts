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

#include <Elementary.h>

#include <contacts.h>
#include <efl_extension.h>
#include <vector>

#include "CtInputView.h"
#include "WPopup.h"
#include "ContactsDebug.h"
#include "CtCommon.h"
#include "CtPath.h"
#include "CtInputDataTypeCtxPopup.h"
#include "CtData.h"

CtInputDataTypeCtxPopup::CtInputDataTypeCtxPopup(Evas_Object *genlist, Evas_Object *typeBtn, const char *viewUri, const char* customLabel )
{
	WHIT();
	__viewUri = SAFE_STRDUP( viewUri );
	__customLabel = SAFE_STRDUP( customLabel );
	__removeBirthdayType = false;
	__genlist = genlist;
	__typeButton = typeBtn;
	__createPopupTimer = NULL;
}

CtInputDataTypeCtxPopup::~CtInputDataTypeCtxPopup()
{
	WHIT();
	SAFE_FREE( __viewUri );
	SAFE_FREE( __customLabel );

	if(__createPopupTimer)
		ecore_timer_del(__createPopupTimer);
}

void CtInputDataTypeCtxPopup::__movePopup(Evas_Object* popup)
{
	Evas_Coord x, y, w, h;
//	Eina_Bool directionDown = EINA_FALSE;

	evas_object_geometry_get(__typeButton, &x, &y, &w, &h);

	WDEBUG("x %d, y %d, h %d", x, y, h);

	evas_object_move(popup, x, y + h);

//	TODO uncomment when elm_ctxpopup_direction_available_get will be implemented
//	directionDown = elm_ctxpopup_direction_available_get(popup, ELM_CTXPOPUP_DIRECTION_DOWN);
//	if(!directionDown) {
//		elm_ctxpopup_direction_priority_set(popup, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UP);
//		evas_object_move(popup, x , y);
//	}

	evas_object_show(popup);
}

void CtInputDataTypeCtxPopup::setOnSelectCb(std::function<void (int type)> selectCb)
{
	__selectCb = selectCb;
}

Evas_Object* CtInputDataTypeCtxPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object *popup = elm_ctxpopup_add(elm_object_top_widget_get(parent));
//	elm_object_style_set(popup, "dropdown/label");
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", [](void* data, Evas_Object* obj, void* event_info){
		CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)data;
		p->destroy();
	}, this);

	elm_ctxpopup_direction_priority_set(popup, ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_DOWN);
	evas_object_data_set(popup, "popup_data", this);

	if (0 == strcmp(__viewUri, _contacts_number._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeNumberChangedCb, (void *)-1);

		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_MOBILE"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_MOBILE);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_WORK"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_WORK);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_HOME_ABB"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_HOME);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_MAIN"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_MAIN);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_FAX_HWORK_ABB"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_FAX_WORK);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_FAX_HHOME_ABB"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_FAX_HOME);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_PAGER"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_PAGER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_OTHER"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_OTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeNumberChangedCb, (void *)CTTEXT_TYPE_CUSTOM);
	}
	else if (0 == strcmp(__viewUri, _contacts_email._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeEmailChangedCb, (void *)-1);

		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_HOME_ABB"), NULL, __dataTypeEmailChangedCb, (void *)CONTACTS_EMAIL_TYPE_HOME);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_WORK"), NULL, __dataTypeEmailChangedCb, (void *)CONTACTS_EMAIL_TYPE_WORK);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_OTHER"), NULL, __dataTypeEmailChangedCb, (void *)CONTACTS_EMAIL_TYPE_OTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeEmailChangedCb, (void *)CONTACTS_EMAIL_TYPE_CUSTOM);
	}
	else if (0 == strcmp(__viewUri, _contacts_address._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeAddressChangedCb, (void *)-1);

		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_HOME_ABB"), NULL, __dataTypeAddressChangedCb, (void *)CONTACTS_ADDRESS_TYPE_HOME);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_WORK"), NULL, __dataTypeAddressChangedCb, (void *)CONTACTS_ADDRESS_TYPE_WORK);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_OTHER"), NULL, __dataTypeAddressChangedCb, (void *)CONTACTS_ADDRESS_TYPE_OTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeAddressChangedCb, (void *)CONTACTS_ADDRESS_TYPE_CUSTOM);
	}
	else if (0 == strcmp(__viewUri, _contacts_messenger._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeMessengerChangedCb, (void *)-1);

		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_AIM"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_AIM);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_YAHOO_E"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_YAHOO);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_BODY_SKYPE"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_SKYPE);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_QQ"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_QQ);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_HANGOUTS"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_GOOGLE);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_ICQ"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_ICQ);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_JABBER"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_JABBER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeMessengerChangedCb, (void *)CONTACTS_MESSENGER_TYPE_CUSTOM);
	}
	else if (0 == strcmp(__viewUri, _contacts_relationship._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeFriendshipChangedCb, (void *)-1);

		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_ASSISTANT_M_CONTACTS"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_ASSISTANT);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_BROTHER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_BROTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CHILD"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_CHILD);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_DOMESTIC_PARTNER_ABB2"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_DOMESTIC_PARTNER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_FATHER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_FATHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_FRIEND"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_FRIEND);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_MANAGER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_MANAGER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_MOTHER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_MOTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_PARENT"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_PARENT);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_PARTNER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_PARTNER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_REFERRED_BY_ABB"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_REFERRED_BY);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_RELATIVE"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_RELATIVE);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_SISTER"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_SISTER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_SPOUSE"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_SPOUSE);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeFriendshipChangedCb, (void *)CONTACTS_RELATIONSHIP_TYPE_CUSTOM);
	}
	else if (0 == strcmp(__viewUri, _contacts_event._uri)) {
		if( __customLabel && __customLabel[0] !='\0' )
			elm_ctxpopup_item_append(popup, __customLabel, NULL, __dataTypeEventChangedCb, (void *)-1);

		if( !__removeBirthdayType )
			elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_BIRTHDAY"), NULL, __dataTypeEventChangedCb, (void *)CONTACTS_EVENT_TYPE_BIRTH);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_ANNIVERSARY_ABB"), NULL, __dataTypeEventChangedCb, (void *)CONTACTS_EVENT_TYPE_ANNIVERSARY);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_OTHER"), NULL, __dataTypeEventChangedCb, (void *)CONTACTS_EVENT_TYPE_OTHER);
		elm_ctxpopup_item_append(popup, V_("IDS_PB_OPT_CUSTOM"), NULL, __dataTypeEventChangedCb, (void *)CONTACTS_EVENT_TYPE_CUSTOM);
	}
	__movePopup(popup);

	return popup;

}

void CtInputDataTypeCtxPopup::onDestroy()
{
	WHIT();
}


void CtInputDataTypeCtxPopup::__dataTypeNumberChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int indexType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (indexType == -1)
	{
		p->destroy();
		return;
	}

	p->__selectCb( indexType );
}

void CtInputDataTypeCtxPopup::__dataTypeEmailChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int emailType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (emailType == -1) {
		p->destroy();
		return;
	}

	p->__selectCb( emailType );
}

void CtInputDataTypeCtxPopup::__dataTypeAddressChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int addressType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (addressType == -1) {
		p->destroy();
		return;
	}

	p->__selectCb( addressType );
}

void CtInputDataTypeCtxPopup::__dataTypeMessengerChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int MessengerType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (MessengerType == -1) {
		p->destroy();
		return;
	}
	p->__selectCb( MessengerType );
}

void CtInputDataTypeCtxPopup::__dataTypeFriendshipChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int relationshipType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (relationshipType == -1) {
		p->destroy();
		return;
	}
	p->__selectCb( relationshipType );
}

void CtInputDataTypeCtxPopup::__dataTypeEventChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	int eventType = (long) data;
	CtInputDataTypeCtxPopup* p = (CtInputDataTypeCtxPopup*)evas_object_data_get(obj, "popup_data");
	WPRET_M(!p, "popup data is NULL");

	if (eventType == -1) {
		p->destroy();
		return;
	}
	p->__selectCb( eventType );
}

