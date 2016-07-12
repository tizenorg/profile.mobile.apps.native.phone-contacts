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
		 * @brief Finish progress callback.
		 */
		typedef std::function<void()> FinishCallback;

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

		/**
		 * @brief Set finish function which is called when the progress is over.
		 * @param[in]   callback    Finish function
		 */
		void setFinishCallback(FinishCallback callback);

	protected:
		/**
		 * @brief Called from another thread when progress starts.
		 */
		virtual void onStart() = 0;

		/**
		 * @brief Called from main loop if the running progress is canceled or failed to start.
		 */
		virtual void onCanceled() { }

		/**
		 * @brief Called when user cancels the progress.
		 * @return true for further cancellation of progress's thread, false - do nothing.
		 */
		virtual bool onCancel();

		/**
		 * @brief Cancel the thread in which progress is running.
		 */
		void cancel();

		/**
		 * @brief Set progress value.
		 * @remark Should be called from onStart().
		 * @param[in]   value   The value that will be set to progress.
		 * @return false if progress's thread is pending cancellation, otherwise - true.
		 */
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
		FinishCallback m_OnFinish;
		ProgressPopup *m_ProgressPopup;
		Ecore_Thread *m_Thread;
	};
}

#endif /* PROGRESS_CONTROLLER_H */
