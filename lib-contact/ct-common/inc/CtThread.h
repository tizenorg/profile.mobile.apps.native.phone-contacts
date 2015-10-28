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

#ifndef _CT_THREAD_H_
#define _CT_THREAD_H_

#include <pthread.h>
#include <Eina.h>
#include <Ecore.h>

/**
 * @brief Called in new thread.
 * @param[in]   data The callback data
 */
typedef void (*ctThreadJob)(void *data);

/**
 * @brief Called after terminated thread
 * @param[in]   data The callback data
 */
typedef void (*ctThreadCompleteCb)(void *data);

/**
 * @brief Handles the threads
 */
class CtThread
{
public:
	/**
	 * @brief Constructor. Initializes this instance of CtThread with the specified value.
	 * @param[in]   job              The callback that called in new thread
	 * @param[in]   jobData          The callback data that is used in the job function
	 * @param[in]   completeCb       The callback that called after terminated thread
	 * @param[in]   completeCbData   The callback data that is used in the completeCb function
	 *
	 * @see ctThreadJob
	 * @see ctThreadCompleteCb
	 */
	CtThread(ctThreadJob job, void *jobData, ctThreadCompleteCb completeCb, void *completeCbData);

	/**
	 * @brief Create a new thread and run the job.
	 */
	void start();

	/**
	 * @brief Make calling thread wait for termination of the thread.
	 */
	void destroy();

private:
	typedef void* PTHREAD_FN;

	~CtThread();

	void __cancel();

	static Eina_Bool __ctThreadTimerCb(void *data);
	static PTHREAD_FN __ctThreadJob(void *data);


private:

	bool __isAlive;
	bool __isComplete;
	pthread_t __pthread;
	Ecore_Timer *__timer;
	ctThreadJob __job;
	void *__jobData;
	ctThreadCompleteCb __completeCb;
	void *__completeCbData;

};


#endif //_CT_THREAD_H_

