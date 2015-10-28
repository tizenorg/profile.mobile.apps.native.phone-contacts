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

#include "ContactsDebug.h"
#include "CtProgressBarPopup.h"
#include "CtString.h"
#include "CtProgressController.h"

CtProgressController::CtProgressController(const char *title, int totalCount)
: m_Cancel(false)
, m_ProgressPopup(NULL)
, m_Thread(NULL)
{
	WHIT();
	createProgressPopup(title, totalCount);
}

CtProgressController::~CtProgressController()
{
	WHIT();
	if (auto popup = m_PopupMonitor.lock()) {
		popup->destroy();
	}
}

CtProgressBarPopup *CtProgressController::getProgressPopup()
{
	return m_ProgressPopup;
}

void CtProgressController::run()
{
	WHIT();
	m_Thread = ecore_thread_run(onStart, onFinish, onCanceled, this);
}

void CtProgressController::setOnFinish(std::function<void(CtProgressController&)> handlerFunc)
{
	m_FinishFunction = std::move(handlerFunc);
}

bool CtProgressController::onProgress(int currentCount)
{
	m_ProgressPopup->setProgressStatusCount(currentCount);
	return !m_Cancel;
}

void CtProgressController::onStart(void *data, Ecore_Thread *thread)
{
	WPRDT_M(!data, "invalid data");
	CtProgressController *vcardProgressPopup = (CtProgressController *) data;
	vcardProgressPopup->onStart(vcardProgressPopup->m_Thread);
}

void CtProgressController::onFinish(void *data, Ecore_Thread *thread)
{
	WPRDT_M(!data, "invalid data");
	CtProgressController *vcardProgressPopup = (CtProgressController *) data;
	vcardProgressPopup->m_FinishFunction(*vcardProgressPopup);

	delete vcardProgressPopup;
}

void CtProgressController::onCanceled(void *data, Ecore_Thread *thread)
{
	WPRDT_M(!data, "invalid data");
	CtProgressController *vcardProgressPopup = (CtProgressController *) data;
	vcardProgressPopup->onCanceled();

	delete vcardProgressPopup;
}

bool CtProgressController::onCancel()
{
	WHIT();
	return true;
}

void CtProgressController::cancel()
{
	WHIT();
	m_Cancel = true;
}

void CtProgressController::createProgressPopup(const char *title, int totalCount)
{
	m_ProgressPopup = new CtProgressBarPopup(totalCount, title, true);

	auto cancelFunction = [this](bool *destroyPopup)->void {
		if (onCancel()) {
			cancel();
		}
		*destroyPopup = false;
	};

	m_ProgressPopup->addButton(V_("IDS_PB_BUTTON_CANCEL"), cancelFunction);
	m_ProgressPopup->setOnBack(cancelFunction);

	m_PopupMonitor = m_ProgressPopup->getWeakPtr();
}
