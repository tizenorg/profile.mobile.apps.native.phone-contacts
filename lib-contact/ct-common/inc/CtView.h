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

#ifndef _CT_VIEW_H_
#define _CT_VIEW_H_

#include "WView.h"

/**
* @brief The CtView is the abstract class managing view
*/
class WAPP_ASSIST_EXPORT CtView
{
public:
	CtView();
	virtual ~CtView();

protected:
	virtual void onLanguageChanged() = 0;

	Evas_Object* createBaseLayout(Evas_Object* parent);

private:
	static void __languageChangedCb(void* data, Evas_Object* obj, void* event_info);

private:
	Evas_Object *__layout;
	int __easyMode;
	int __emergencyMode;
};

#endif // _CT_VIEW_H_
