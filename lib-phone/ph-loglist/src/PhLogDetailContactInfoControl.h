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

#ifndef _PH_LOG_DETAIL_CONTACT_INFO_CONTROL_H_
#define _PH_LOG_DETAIL_CONTACT_INFO_CONTROL_H_

#include <string>

#include <Elementary.h>

#include "WControl.h"

#include "PhLogDbMgr.h"
#include "PhLogGenlistHelper.h"

class PhLogDetailContactInfoControl : public WControl
{
public:
	PhLogDetailContactInfoControl(const PhLogDbMgr::RecordInfo *recordInfo);

	void update();

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param );

private:
	void __setThumbnail();
	void __setDisplayName();
	void __setButton();

	static void onThumbnailPressed(void *data);

private:
	Evas_Object *__baseLayout;

	std::string __number;
	std::string __name;
	std::string __imgPath;
	std::string __callDesc;

	int __personId;
};

#endif /* _PH_LOG_DETAIL_CONTACT_INFO_CONTROL_H_ */

