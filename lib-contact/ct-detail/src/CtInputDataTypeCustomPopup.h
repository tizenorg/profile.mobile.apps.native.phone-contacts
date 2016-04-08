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

#ifndef _CT_INPUT_DATA_TYPE_CUSTOM_POPUP_H_
#define _CT_INPUT_DATA_TYPE_CUSTOM_POPUP_H_

#include <Elementary.h>
#include <functional>
#include "WPopup.h"

class WEditfield;

class CtInputDataTypeCustomPopup : public WPopup
{
public:
	CtInputDataTypeCustomPopup();

	void setOnResultCb(std::function<void (const char* text)> resultCb);
private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual ~CtInputDataTypeCustomPopup();
private:
	void __saveCustomDataType();
	WEditfield* __createEditField(Evas_Object *parent, const char *guideText, bool multiline,
			Evas_Smart_Cb unfocusedCallback, void *userData1, Evas_Smart_Cb focusedCallback, void *userData2);
	static void __dataTypeCustomCancelCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeCustomEntryChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __entryShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info);
	static void __entryActivatedCb(void *data, Evas_Object *obj, void *event_info);

	static Eina_Bool __notificationTimerCb(void *data);
	static void __editfieldTextMaxReachedCb(void *data, Evas_Object *obj, void *event_info);
	void __showTickerNotification(const char *msg);
private:
	Evas_Object* __customEntry;
	Ecore_Timer *__notificationTimer;

	std::function<void (const char* text)> __resultCb;
};

#endif //_CT_INPUT_DATA_TYPE_CUSTOM_POPUP_H_
