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

#include "CtType.h"
#include "ContactsDebug.h"
#include "CtThread.h"

#define CT_THREAD_TIMER_DURATION 0.02

CtThread::CtThread(ctThreadJob job, void *jobData, ctThreadCompleteCb completeCb, void *completeCbData)
{
	WHIT();
	__isAlive = false;
	__isComplete = false;
	__pthread = 0;
	__timer = NULL;
	__job = job;
	__jobData = jobData;
	__completeCb = completeCb;
	__completeCbData = completeCbData;
}


void CtThread::start()
{
	WENTER();

	int ret;
	ret = pthread_create(&__pthread, NULL, __ctThreadJob, this);
	WPRET_M(0 != ret, "pthread_create() Failed(%d)", ret);

	__isAlive = true;
	__timer = ecore_timer_add(CT_THREAD_TIMER_DURATION, __ctThreadTimerCb, this);

}

void CtThread::__cancel()
{
	WENTER();
	if(__timer){
		ecore_timer_del(__timer);
	}
	__timer = NULL;

	__completeCb = NULL;

	__isAlive = false;
	__isComplete = false;

	WDEBUG("pthread_join(%p)", __pthread);

	if(__pthread > 0) {
		WDEBUG("thread join start");
		pthread_join(__pthread, NULL);
		__pthread = 0;
		WDEBUG("thread join end");
	}

	WLEAVE();
}

void CtThread::destroy()
{
	WHIT();

	__cancel();

	delete this;
}


Eina_Bool CtThread::__ctThreadTimerCb(void *data)
{
	CtThread *self = (CtThread*)data;
	WPRET_VM(NULL == self, ECORE_CALLBACK_CANCEL, "data NULL");
	WPRET_VM(self->__isAlive, ECORE_CALLBACK_RENEW, "thread alive yet");

	WDEBUG("pthread_join(%p)", self->__pthread);

	if(self->__pthread > 0) {
		WDEBUG("thread join start");
		pthread_join(self->__pthread, NULL);
		self->__pthread = 0;
		WDEBUG("thread join end");
	}

	self->__timer = NULL;

	if (self->__isComplete && self->__completeCb) {
		self->__completeCb(self->__completeCbData);
	}

	return ECORE_CALLBACK_CANCEL;
}



CtThread::PTHREAD_FN CtThread::__ctThreadJob(void *data)
{
	WENTER();

	CtThread *self = (CtThread*)data;
	WDEBUG("thread job start");
	if(self->__job)
		self->__job(self->__jobData);
	WDEBUG("thread job end");
	self->__isAlive = false;
	self->__isComplete = true;
	WDEBUG("thread pthread_exit");
 	pthread_exit(NULL);

	WLEAVE();
}

CtThread::~CtThread()
{

}

