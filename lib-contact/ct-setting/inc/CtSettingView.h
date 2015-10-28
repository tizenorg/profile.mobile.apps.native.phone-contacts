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

#ifndef _CT_SETTING_VIEW_H_
#define _CT_SETTING_VIEW_H_

#include <thread>
#include <string>
#include <vector>
#include <memory>
#include "WView.h"
#include "CtProgressBarPopup.h"

class CtSettingData;
class CtProgressController;

class WAPP_ASSIST_EXPORT CtSettingView : public WView
{
public:
	CtSettingView();

private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);

	CtSettingData* __getSettingData();
	Elm_Genlist_Item_Class *getOneLineItc();

	static void __onDisplayNameByItemSelectedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onImportItemSelectedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onExportItemSelectedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onSortByItemSelectedCb(void *data, Evas_Object *obj, void *event_info);

	void __importFromStorage(bool phone);

	void __exportContactsTo(TargetStorage saveTo);
	void __excuteContactExporter(TargetStorage saveTo, app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	void __createExportResultPopup(CtProgressController &controller);
	void __excuteContactImporter(std::vector<std::string> vcards);

	void __finishImportCb(CtProgressController &controller);
	static void __importFromStrorageReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);


private:
	WDECLARE_PRIVATE_IMPL(CtSettingView);
	WDISABLE_COPY_AND_ASSIGN(CtSettingView);

	virtual ~CtSettingView();

private:
	Evas_Object* __genlist;
	CtSettingData* __settingData;
};

#endif //_CT_SETTING_VIEW_H_
