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

#include "Utils/Thread.h"
#include "Utils/Callback.h"

using namespace Utils;

Thread::Thread(Callback task, Callback finishCallback)
	: m_Task(std::move(task)), m_OnFinished(std::move(finishCallback))
{
	ecore_thread_run(makeCallback(&Thread::onStart), makeCallback(&Thread::onFinished),
			nullptr, this);
}

void Thread::onStart(Ecore_Thread *thread)
{
	if (m_Task) {
		m_Task();
	}
}

void Thread::onFinished(Ecore_Thread *thread)
{
	if (m_OnFinished) {
		m_OnFinished();
	}

	delete this;
}
