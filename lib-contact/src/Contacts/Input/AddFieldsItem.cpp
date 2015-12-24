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

#include "Contacts/Input/AddFieldsItem.h"
#include "Contacts/Common/Strings.h"

#include "App/Path.h"
#include "Ui/Menu.h"
#include "Utils/Callback.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	Elm_Genlist_Item_Class itc = Ui::GenlistItem::createItemClass(INPUT_ITEM_STYLE_BUTTONS);
}

AddFieldsItem::AddFieldsItem()
	: GenlistItem(&itc), m_Buttons{nullptr}
{
	memset(m_AddFieldStates, true, sizeof(m_AddFieldStates));
}

void AddFieldsItem::setAddFieldCallback(AddFieldCallback callback)
{
	m_OnAddField = std::move(callback);
}

void AddFieldsItem::setAddFieldState(ContactFieldId fieldId, bool isEnabled)
{
	m_AddFieldStates[fieldId] = isEnabled;
	elm_object_disabled_set(m_Buttons[fieldId], !isEnabled);
}

Evas_Object *AddFieldsItem::getContent(Evas_Object *parent, const char *part)
{
	Evas_Object *box = elm_box_add(parent);
	elm_box_horizontal_set(box, EINA_TRUE);

	elm_box_pack_end(box, createAddFieldButton(box, FieldNumber, INPUT_ICON_ADD_NUMBER));
	elm_box_pack_end(box, createAddFieldButton(box, FieldEmail, INPUT_ICON_ADD_EMAIL));
	elm_box_pack_end(box, createAddFieldButton(box, FieldNote, INPUT_ICON_ADD_NOTE));
	elm_box_pack_end(box, createButton(box, "IDS_PB_BUTTON_MORE", INPUT_ICON_ADD_MORE,
			makeCallback(&AddFieldsItem::onMoreButtonPressed), this));

	return box;
}

void AddFieldsItem::onUnrealized()
{
	memset(m_Buttons, 0, sizeof(m_Buttons));
}

Evas_Object *AddFieldsItem::createButton(Evas_Object *parent, const char *text,
		const char *iconPath, Evas_Smart_Cb callback, void *data)
{
	Evas_Object *image = elm_image_add(parent);
	elm_image_file_set(image, App::getResourcePath(iconPath).c_str(), nullptr);

	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "circle");
	elm_object_translatable_text_set(button, text);
	elm_object_part_content_set(button, "elm.swallow.content", image);
	evas_object_smart_callback_add(button, "clicked", callback, data);

	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(button);

	return button;
}

Evas_Object *AddFieldsItem::createAddFieldButton(Evas_Object *parent,
		ContactFieldId fieldId, const char *iconPath)
{
	Evas_Object *button = createButton(parent, Common::getContactFieldName(fieldId), iconPath,
			makeCallback(&AddFieldsItem::onAddButtonPressed), this);
	evas_object_smart_data_set(button, (void *) fieldId);
	elm_object_disabled_set(button, !m_AddFieldStates[fieldId]);

	m_Buttons[fieldId] = button;
	return button;
}

void AddFieldsItem::onAddField(ContactFieldId fieldId)
{
	if (m_OnAddField) {
		m_OnAddField(fieldId);
	}
}

void AddFieldsItem::onAddButtonPressed(Evas_Object *button, void *eventInfo)
{
	onAddField((ContactFieldId) (long) evas_object_smart_data_get(button));
}

void AddFieldsItem::onMoreButtonPressed(Evas_Object *button, void *eventInfo)
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(button);

	for (unsigned id = FieldBegin; id < FieldEnd; ++id) {
		ContactFieldId fieldId = ContactFieldId(id);
		if (!m_Buttons[fieldId] && m_AddFieldStates[fieldId]) {
			menu->addItem(Common::getContactFieldName(fieldId),
				std::bind(&AddFieldsItem::onAddField, this, fieldId));
		}
	}

	menu->show();
}
