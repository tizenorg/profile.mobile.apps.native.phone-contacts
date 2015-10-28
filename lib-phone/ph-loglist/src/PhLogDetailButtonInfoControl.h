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

#ifndef _PH_LOG_DETAIL_LOG_INFO_CONTROL_H_
#define _PH_LOG_DETAIL_LOG_INFO_CONTROL_H_

#include <string>

#include <Elementary.h>
#include "WControl.h"

class PhLogDetailButtonInfoControl: public WControl
{
public:
	PhLogDetailButtonInfoControl(Evas_Object* window, Evas_Object* naviframe);

	void update(const std::string& number);
	void setOnMsgHandler( const std::function<void (const std::string& number)>& handlerFunc );

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param );
	virtual void onDestroy();

private:
	virtual ~PhLogDetailButtonInfoControl();

private:
	Evas_Object *__messageIcon;
	Evas_Object *__voiceIcon;

	std::string __number;
	void *__dlHandle;
	std::function<void (const std::string& number)> __msgHandler;
};

#endif /* _PH_LOG_DETAIL_LOG_INFO_CONTROL_H_ */

