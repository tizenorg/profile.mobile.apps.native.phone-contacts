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
		 * @brief Run progress.
		 */
		void run();

		/**
		 * @brief Finish progress callback.
		 */
		typedef std::function<void(ProgressController &)> FinishCallback;

		/**
		 * @brief Set finish function which is called when the progress is over.
		 * @param[in]   handlerFunc Finish function
		 */
		void setOnFinish(FinishCallback handlerFunc);

	protected:
		ProgressController(Evas_Object *parent, const char *title, int maxValue);
		virtual ~ProgressController();

		virtual void onStart(Ecore_Thread *thread) = 0;
		virtual void onCanceled() = 0;
		virtual bool onCancel();

		void cancel();
		bool onProgress(int value);

	private:
		void createProgressPopup(Evas_Object *parent, const char *title, int maxValue);

		static void onStart(void *data, Ecore_Thread *thread);
		static void onFinish(void *data, Ecore_Thread *thread);
		static void onCanceled(void *data, Ecore_Thread *thread);

	private:
		bool m_Cancel;
		FinishCallback m_FinishCb;
		ProgressPopup *m_ProgressPopup;
		Ecore_Thread *m_Thread;
	};
}

#endif /* PROGRESS_CONTROLLER_H */
