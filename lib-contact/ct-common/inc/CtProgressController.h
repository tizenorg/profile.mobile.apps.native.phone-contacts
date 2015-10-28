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

#ifndef _CT_PROGRESS_CONTROLLER_H_
#define _CT_PROGRESS_CONTROLLER_H_

#include <functional>
#include <vector>

#include "IWUiObject.h"
#include "WDefine.h"

class CtProgressBarPopup;

/**
 * @brief Controller for ProgressBarPopup
 */
class WAPP_ASSIST_EXPORT CtProgressController
{
public:

	/**
	 * @brief Get Progress popup
	 * @return Progress popup
	 */
	CtProgressBarPopup *getProgressPopup();

	/**
	 * @brief Run thread.
	 */
	void run();

	/**
	 * @brief Set End thread callback.
	 * @param[in]   handlerFunc The callback
	 */
	void setOnFinish(std::function<void(CtProgressController&)> handlerFunc);

protected:
	CtProgressController(const char *title, int totalCount);
	virtual ~CtProgressController();

	bool onProgress(int count);

	virtual void onStart(Ecore_Thread *thread) = 0;
	virtual void onCanceled() = 0;
	virtual bool onCancel();

	void cancel();
	void createProgressPopup(const char *title, int totalCount);

private:
	static void onStart(void *data, Ecore_Thread *thread);
	static void onFinish(void *data, Ecore_Thread *thread);
	static void onCanceled(void *data, Ecore_Thread *thread);

	WDISABLE_COPY_AND_ASSIGN(CtProgressController);

private:
	bool m_Cancel;
	std::function<void(CtProgressController&)> m_FinishFunction;
	CtProgressBarPopup *m_ProgressPopup;
	WUiObjectPtr m_PopupMonitor;
	Ecore_Thread *m_Thread;
};

#endif // _CT_PROGRESS_CONTROLLER_H_
