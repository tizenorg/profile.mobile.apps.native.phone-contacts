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

#ifndef PROGRESS_CONTROLLER_H
#define PROGRESS_CONTROLLER_H

#include <Elementary.h>
#include <condition_variable>
#include <functional>
#include <tizen.h>

namespace Ui
{
	class ProgressPopup;

	/**
	 * @brief Controller for ProgressPopup
	 */
	class EXPORT_API ProgressController
	{
	public:
		/**
		 * @brief Create controller for ProgressPopup.
		 * @param[in]   parent      Parent object
		 * @param[in]   title       Progress popup title
		 * @param[in]   maxValue    max value of progress popup
		 */
		ProgressController(Evas_Object *parent, const char *title, int maxValue);
		ProgressController(const ProgressController &that) = delete;
		virtual ~ProgressController();

		ProgressController & operator=(const ProgressController &that) = delete;

		/**
		 * @brief Run progress.
		 */
		void run();

	protected:
		virtual void onStart() = 0;
		virtual void onFinish() { }
		virtual void onCanceled() { }
		virtual bool onCancel();

		void cancel();
		bool onProgress(size_t value);

	private:
		void createProgressPopup(Evas_Object *parent, const char *title, int maxValue);

		static void onStart(void *data, Ecore_Thread *thread);
		static void onNotify(void *data, Ecore_Thread *thread, void *msgData);
		static void onFinish(void *data, Ecore_Thread *thread);
		static void onCanceled(void *data, Ecore_Thread *thread);

	private:
		std::condition_variable m_ContinueCondition;
		bool m_IsPopupUpdating;
		Ecore_Thread *m_MainThread;
		ProgressPopup *m_ProgressPopup;
		Ecore_Thread *m_Thread;
	};
}

#endif /* PROGRESS_CONTROLLER_H */
