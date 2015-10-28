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

#ifndef _CT_NUMBER_POPUP_H_
#define _CT_NUMBER_POPUP_H_

#include "WPopup.h"

#include <Elementary.h>

class CtListDataPerson;

class CtNumberPopup : public WPopup
{
public:
	typedef std::function<void(CtListDataPerson*)> ResultCb;

	CtNumberPopup(CtListDataPerson *personData);

	void setOnResult(ResultCb callback);

private:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);
	Evas_Object *createGenlist(Evas_Object *parent);

	static char *onItemTextGet(void *data, Evas_Object *obj, const char *part);
	static void  onItemSelect(void *data, Evas_Object *obj, void *event_info);

	CtListDataPerson *m_PersonData;
	ResultCb m_OnResult;

	Elm_Object_Item *m_CallItem;
	Elm_Object_Item *m_MessageItem;
};

#endif /* _CT_NUMBER_POPUP_H_ */
