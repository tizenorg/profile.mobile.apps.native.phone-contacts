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

#ifndef UI_CONTROL_H
#define UI_CONTROL_H

#include <Elementary.h>

namespace Ui
{
	/**
	 * @brief Evas_Object wrapper.
	 * @details Ties the lifetime of this object to the underlying Evas_Object.
	 * @remark Should always be allocated with operator "new".
	 */
	class Control
	{
	public:
		/**
		 * @brief Create underlying Evas_Object by calling onCreate()
		 * @param[in]   parent  Parent Evas_Object
		 * @return Created Evas_Object on success, otherwise nullptr
		 */
		Evas_Object *create(Evas_Object *parent);

		/**
		 * @return Underlying Evas_Object.
		 */
		Evas_Object *getEvasObject() const;

	protected:
		Control();
		virtual ~Control();

		/**
		 * @brief Called after create() is called
		 * @param[in]   parent  Parent Evas_Object
		 * @return Created Evas_Object on success, otherwise nullptr
		 */
		virtual Evas_Object *onCreate(Evas_Object *parent) = 0;

		/**
		 * @brief Untie this object from underlying Evas_Object
		 * @return Untied Evas_Object
		 */
		Evas_Object *reset();

	private:
		void onDestroy(Evas *e, Evas_Object *obj, void *event_info);

		Evas_Object *m_Object;
	};
}

#endif /* UI_CONTROL_H */
