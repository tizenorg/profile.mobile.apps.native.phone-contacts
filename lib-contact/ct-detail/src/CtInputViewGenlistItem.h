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

#ifndef _CT_INPUT_VIEW_GENLIST_ITEM_H_
#define _CT_INPUT_VIEW_GENLIST_ITEM_H_

#include <list>
#include "WEditfield.h"
#include "WGenlistItem.h"
#include "ContactsDebug.h"

class CtInputViewGenlistItem : public WGenlistItem
{
public:
	CtInputViewGenlistItem(const char* oldVal);
	CtInputViewGenlistItem();
	virtual ~CtInputViewGenlistItem();

public:
	virtual void onRealized();
	virtual void onUnrealized();

	virtual bool getIsChanged();
	virtual bool getIsEmpty();
	virtual const char* getEntryObjPartName();
	Evas_Object* getEntry();
	void focus();

protected:
	void setIsChanged(bool isChanged);
	void updateChangeState( const char *text);
	WEditfield* createEditField(Evas_Object *parent, const char *guideText, bool multiline, bool isMainField = false);
	static void setEntryText(Evas_Object *entry, const char *text);
private:
	static Eina_Bool __notificationTimerCb(void *data);
	static void __editfieldTextMaxReachedCb(void *data, Evas_Object *obj, void *event_info);
	void __showTickerNotification(const char *msg);

private:
	bool __isChanged;
	bool __isEmpty;
	char* __oldVal;
	Ecore_Timer *__notificationTimer;

	Evas_Object *__entry;
	bool __focused;
};

#endif /* _CT_INPUT_VIEW_GENLIST_ITEM_H_ */
