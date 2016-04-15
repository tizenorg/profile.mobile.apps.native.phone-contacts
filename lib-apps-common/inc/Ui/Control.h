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
#include <tizen.h>

namespace Ui
{
	/**
	 * @brief Basic Control types.
	 */
	enum ControlType
	{
		TypeControl,
		TypeWindow,
		TypeView,
		TypeMax
	};

	/**
	 * @brief Evas_Object wrapper.
	 * @details Ties the lifetime of this object to the underlying Evas_Object.
	 * @remark Should always be allocated with operator "new".
	 */
	class EXPORT_API Control
	{
	public:
		Control(int type = TypeControl);
		Control(const Control &that) = delete;
		Control(Control &&that) = delete;
		virtual ~Control();

		Control & operator=(const Control &that) = delete;
		Control & operator=(Control &&that) = delete;

		/**
		 * @brief Create underlying Evas_Object by calling onCreate()
		 * @param[in]   parent  Parent Evas_Object
		 * @return Created Evas_Object on success, otherwise nullptr
		 */
		Evas_Object *create(Evas_Object *parent);

		/**
		 * @return Control type.
		 */
		int getType() const;

		/**
		 * @return Underlying Evas_Object.
		 */
		Evas_Object *getEvasObject() const;

		/**
		 * @brief Find closest parent Control by type.
		 * @param[in]   object  Child object
		 * @param[in]   type    Control type
		 * @return Control if found, otherwise nullptr.
		 */
		template <typename ControlType>
		ControlType *findParent() const;
		Control *findParent(int type) const;

		template <typename ControlType>
		static ControlType *findParent(Evas_Object *object);
		static Control *findParent(Evas_Object *object, int type);

		/**
		 * @brief Get Control object from Evas_Object
		 * @param[in]   object  Evas_Object that belongs to some Control
		 * @return Control tied to Evas_Object
		 */
		static Control *getControl(Evas_Object *object);

	protected:
		/**
		 * @brief Called after create() is called
		 * @param[in]   parent  Parent Evas_Object
		 * @return Created Evas_Object on success, otherwise nullptr
		 */
		virtual Evas_Object *onCreate(Evas_Object *parent) = 0;

		/**
		 * @brief Called after onCreate() has returned
		 */
		virtual void onCreated() { }

	private:
		void setEvasObject(Evas_Object *object);
		Evas_Object *resetEvasObject();
		void destroyEvasObject();

		void onDestroy(Evas *e, Evas_Object *obj, void *event_info);

		int m_Type;
		Evas_Object *m_Object;
	};

	template <typename ControlType>
	ControlType *Control::findParent() const
	{
		return findParent<ControlType>(m_Object);
	}
}

#endif /* UI_CONTROL_H */
