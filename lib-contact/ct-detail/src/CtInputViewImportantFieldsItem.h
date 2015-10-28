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

#ifndef _CT_INPUT_VIEW_IMPORTANT_FIELDS_ITEM_H_
#define _CT_INPUT_VIEW_IMPORTANT_FIELDS_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtData.h"

class CtInputViewImportantFieldsItem : public CtInputViewGenlistItem
{
public:
	CtInputViewImportantFieldsItem(const std::function<void ()>& addNumberCb,
		const std::function<void ()>& addEmailCb,
		const std::function<void ()>& addNoteCb,
		const std::function<void ()>& addMoreCb,
		bool isNoteInput)
	{
		__addNumberCb = addNumberCb;
		__addEmailCb = addEmailCb;
		__addNoteCb = addNoteCb;
		__addMoreCb = addMoreCb;
		__isNoteInput = isNoteInput;
	};
	virtual ~CtInputViewImportantFieldsItem() { WHIT(); }
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;

		itc.item_style = "input.4icon";
		itc.func.content_get = __getContentImportantFields;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual bool getIsEmpty()
	{
		return true;
	}

	void updateNoteBtn(bool isNoteInput)
	{
		__isNoteInput = isNoteInput;
		Evas_Object *btn = elm_object_item_part_content_get(getElmObjectItem(), "elm.icon.3");
		elm_object_disabled_set(btn, __isNoteInput);
	}

private:
	static Evas_Object* __getContentImportantFields(void *data, Evas_Object *obj, const char *part)
	{
		CtInputViewImportantFieldsItem* item = (CtInputViewImportantFieldsItem*)data;
		Evas_Object *btn = NULL;
		Evas_Object *img = NULL;

		btn = elm_button_add(obj);
		elm_object_style_set(btn, "circle");

		if (0 == strcmp(part, "elm.icon.1")) {
			elm_object_text_set(btn, V_("IDS_PB_BUTTON_PHONE"));
			evas_object_smart_callback_add(btn, "clicked", __addNumberClickedCb, item);

			img = elm_image_add(btn);
			elm_image_file_set(img, ContactsCommon::getResourcePath(CT_IMG_ICON_INPUT_CALL).c_str(), NULL);
			elm_image_resizable_set(img, EINA_TRUE, EINA_TRUE);
			elm_object_part_content_set(btn, "icon", img);
		}
		else if (0 == strcmp(part, "elm.icon.2")) {
			elm_object_text_set(btn, V_("IDS_PB_BUTTON_EMAIL_ABB3"));
			evas_object_smart_callback_add(btn, "clicked", __addEmailClickedCb, item);

			img = elm_image_add(btn);
			elm_image_file_set(img, ContactsCommon::getResourcePath(CT_IMG_ICON_INPUT_EMAIL).c_str(), NULL);
			elm_image_resizable_set(img, EINA_TRUE, EINA_TRUE);
			elm_object_part_content_set(btn, "icon", img);
		}
		else if (0 == strcmp(part, "elm.icon.3")) {
			elm_object_text_set(btn, V_("IDS_PB_BUTTON_NOTES"));
			evas_object_smart_callback_add(btn, "clicked", __addNoteClickedCb, item);

			img = elm_image_add(btn);
			elm_image_file_set(img, ContactsCommon::getResourcePath(CT_IMG_ICON_INPUT_NOTE).c_str(), NULL);
			elm_image_resizable_set(img, EINA_TRUE, EINA_TRUE);
			elm_object_part_content_set(btn, "icon", img);
			elm_object_disabled_set(btn, item->__isNoteInput);
		}
		else if (0 == strcmp(part, "elm.icon.4")) {
			elm_object_text_set(btn, V_("IDS_PB_BUTTON_MORE"));
			evas_object_smart_callback_add(btn, "clicked", __addMoreClickedCb, item);

			img = elm_image_add(btn);
			elm_image_file_set(img, ContactsCommon::getResourcePath(CT_IMG_ICON_INPUT_MORE).c_str(), NULL);
			elm_image_resizable_set(img, EINA_TRUE, EINA_TRUE);
			elm_object_part_content_set(btn, "icon", img);
		}

		return btn;
	}

	static void __addNumberClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewImportantFieldsItem* item = (CtInputViewImportantFieldsItem*)data;
		item->__addNumberCb();
	}

	static void __addEmailClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewImportantFieldsItem* item = (CtInputViewImportantFieldsItem*)data;
		item->__addEmailCb();
	}

	static void __addNoteClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewImportantFieldsItem* item = (CtInputViewImportantFieldsItem*)data;
		elm_object_disabled_set(obj, EINA_TRUE);
		item->__addNoteCb();
	}

	static void __addMoreClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewImportantFieldsItem* item = (CtInputViewImportantFieldsItem*)data;
		elm_object_focus_set(obj, EINA_TRUE);
		item->__addMoreCb();
	}
private:
	CtInputViewImportantFieldsItem();

	std::function<void ()> __addNumberCb;
	std::function<void ()> __addEmailCb;
	std::function<void ()> __addNoteCb;
	std::function<void ()> __addMoreCb;
	bool __isNoteInput;
};

#endif /* _CT_INPUT_VIEW_IMPORTANT_FIELDS_ITEM_H_ */
