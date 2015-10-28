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

#ifndef _CT_INPUT_DATA_TYPE_POPUP2_H_
#define _CT_INPUT_DATA_TYPE_POPUP2_H_

#include <Ecore.h>
#include <Elementary.h>
#include <functional>
#include "WControl.h"

class CtInputDataTypeCtxPopup : public WControl
{
public:
	CtInputDataTypeCtxPopup(Evas_Object *genlist, Evas_Object *typeBtn, const char *viewUri, const char* customLabel);

	void setOnSelectCb(std::function<void (int type)> selectCb);

	void setRemoveBirthdayType(){ __removeBirthdayType = true; }
private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
private:

	static void __dataTypeNumberChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeEmailChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeAddressChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeMessengerChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeFriendshipChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeEventChangedCb(void *data, Evas_Object *obj, void *event_info);

	void __movePopup(Evas_Object *popup);
private:
	CtInputDataTypeCtxPopup();
	virtual ~CtInputDataTypeCtxPopup();

private:
	char* __viewUri;
	char* __customLabel;
	Evas_Coord __x, __y, __w, __h;
	std::function<void (int type)> __selectCb;
	bool __removeBirthdayType;
	Ecore_Timer *__createPopupTimer;
	Evas_Object *__genlist;
	Evas_Object *__typeButton;

};

#endif //_CT_INPUT_DATA_TYPE_POPUP_H_
