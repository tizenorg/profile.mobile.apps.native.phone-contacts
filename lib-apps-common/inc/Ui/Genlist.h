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

#ifndef UI_GENLIST_H
#define UI_GENLIST_H

#include "Ui/GenContainer.h"

namespace Ui
{
	class EXPORT_API Genlist : public GenContainer
	{
	public:
		Genlist();

	protected:
		virtual void onCreated() override;

	private:
		static void onItemExpanded(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
		static void onItemContracted(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
	};
}

#endif /* UI_GENLIST_H */
