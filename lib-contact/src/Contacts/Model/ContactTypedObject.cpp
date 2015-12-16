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

#include "Contacts/Model/ContactTypedObject.h"
#include "Contacts/Model/ContactFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

void ContactTypedObject::reset()
{
	getTypeField()->reset();
	getLabelField()->reset();
	ContactObject::reset();
}

ContactFieldPtr ContactTypedObject::getTypeField() const
{
	return ContactFactory::createField(getRecord(), getTypedObjectMetadata().typeField);
}

ContactFieldPtr ContactTypedObject::getLabelField() const
{
	return ContactFactory::createField(getRecord(), getTypedObjectMetadata().labelField);
}

const ContactTypedObjectMetadata &ContactTypedObject::getTypedObjectMetadata() const
{
	return (const ContactTypedObjectMetadata &) getObjectMetadata();
}
