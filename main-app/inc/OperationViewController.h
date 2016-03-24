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

#ifndef OPERATION_VIEW_CONTROLLER_H
#define OPERATION_VIEW_CONTROLLER_H

#include "OperationController.h"
#include "Contacts/SelectTypes.h"

namespace Contacts
{
	namespace List
	{
		class ListView;
	}
	namespace Settings
	{
		class ImportController;
	}
}

class OperationViewController : public OperationController
{
public:
	OperationViewController();

private:
	virtual void onRequest(Operation operation, app_control_h request);
	bool onSelectResult(Contacts::SelectResults results,
			Contacts::List::ListView *view, std::string uri);
	void onImportFinish(Contacts::Settings::ImportController *importer,
			Contacts::List::ListView *view);
};

#endif /* _OPERATION_VIEW_CONTROLLER_H_ */
