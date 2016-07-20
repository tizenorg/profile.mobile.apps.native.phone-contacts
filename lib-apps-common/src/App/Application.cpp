/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "App/Application.h"
#include "Utils/Callback.h"

using namespace App;

int Application::run(int argc, char **argv)
{
	ui_app_lifecycle_callback_s cbs;
	cbs.create = makeCallback(&Application::onCreate);
	cbs.terminate = makeCallback(&Application::onTerminate);
	cbs.pause = makeCallback(&Application::onPause);
	cbs.resume = makeCallback(&Application::onResume);
	cbs.app_control = makeCallbackWithLastParam(&Application::onAppControl);

	return ui_app_main(argc, argv, &cbs, this);
}
