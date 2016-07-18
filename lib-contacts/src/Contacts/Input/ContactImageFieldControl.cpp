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

#include "Contacts/Input/ContactImageFieldControl.h"
#include "Contacts/Model/ContactTextField.h"

#include "App/AppControlRequest.h"
#include "Ui/ListPopup.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactImageFieldControl::ContactImageFieldControl(ContactTextField &field)
	: Thumbnail(Thumbnail::SizeLarge), m_Field(field)
{
}

void ContactImageFieldControl::update()
{
	setImagePath(m_Field.getValue());
}

void ContactImageFieldControl::onCreated()
{
	setSizeHint(true);
	evas_object_smart_callback_add(getImage(), "clicked",
			makeCallback(&ContactImageFieldControl::onImagePressed), this);

	update();
}

void ContactImageFieldControl::onImageResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	std::string value = App::getSingleExtraData(reply, APP_CONTROL_DATA_SELECTED);
	if (!value.empty()) {
		m_Field.setValue(value.c_str());
		update();
	}
}

void ContactImageFieldControl::onImagePressed(Evas_Object *image, void *eventInfo)
{
	Ui::ListPopup *popup = new Ui::ListPopup();
	popup->create(getEvasObject());
	popup->setTitle("IDS_PB_HEADER_SET_CONTACT_PICTURE_ABB");
	popup->addItem("IDS_PB_OPT_FROM_GALLERY_ABB2", [this] {
		m_AppControl = App::requestGalleryImage();
		m_AppControl.launch(makeCallbackWithLastParam(&ContactImageFieldControl::onImageResult), this);
	});
	popup->addItem("IDS_PB_OPT_TAKE_PICTURE_ABB", [this] {
		m_AppControl = App::requestCameraImage();
		m_AppControl.launch(makeCallbackWithLastParam(&ContactImageFieldControl::onImageResult), this);
	});

	if (!m_Field.isEmpty()) {
		popup->addItem("IDS_PB_OPT_REMOVE", [this] {
			m_Field.reset();
			update();
		});
	}
}
