/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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


#include "Ui/ProgressController.h"
#include "Ui/ProgressPopup.h"
#include "Utils/Logger.h"

using namespace Ui;

ProgressController::ProgressController(Evas_Object *parent, const char *title, int maxValue)
	: m_Cancel(false), m_ProgressPopup(nullptr), m_Thread(nullptr)
{
	createProgressPopup(parent, title, maxValue);
}

void ProgressController::createProgressPopup(Evas_Object *parent, const char *title, int maxValue)
{
	m_ProgressPopup = new Ui::ProgressPopup(maxValue);
	RETM_IF_ERR(!m_ProgressPopup, "m_ProgressPopup is NULL");

	m_ProgressPopup->create(parent);
	m_ProgressPopup->setTitle(title);

	auto cancelFunction = [this](void)->bool {
		if (onCancel()) {
			cancel();
		}
		return false;
	};
	m_ProgressPopup->addButton("IDS_PB_BUTTON_CANCEL", cancelFunction);

	elm_popup_orient_set(m_ProgressPopup->getEvasObject(), ELM_POPUP_ORIENT_BOTTOM);
}

bool ProgressController::onCancel()
{
	return true;
}

void ProgressController::cancel()
{
	m_Cancel = true;
}

ProgressController::~ProgressController()
{
	delete m_ProgressPopup;
}

void ProgressController::run()
{
	m_Thread = ecore_thread_run(onStart, onFinish, onCanceled, this);
}

void ProgressController::onStart(void *data, Ecore_Thread *thread)
{
	RETM_IF_ERR(!data, "invalid data");
	ProgressController *controller = (ProgressController *)data;
	controller->onStart(controller->m_Thread);
}

void ProgressController::onFinish(void *data, Ecore_Thread *thread)
{
	RETM_IF_ERR(!data, "invalid data");
	ProgressController *controller = (ProgressController *)data;
	controller->m_FinishCb(*controller);

	delete controller;
}

void ProgressController::onCanceled(void *data, Ecore_Thread *thread)
{
	RETM_IF_ERR(!data, "invalid data");
	ProgressController *controller = (ProgressController *)data;
	controller->onCanceled();

	delete controller;
}

void ProgressController::setOnFinish(FinishCallback handlerFunc)
{
	m_FinishCb = std::move(handlerFunc);
}

bool ProgressController::onProgress(int value)
{
	m_ProgressPopup->setProgress(value);
	return !m_Cancel;
}
