/*
 * LogView.cpp
 *
 *  Created on: Dec 9, 2015
 *      Author: iryna_ferenchak
 */
#include "Logs/List/LogView.h"
#include "Logs/Model/LogGroup.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"
#include <algorithm>

using namespace Logs::Model;
using namespace Logs::List;

LogView::LogView()
: m_LogProvider(nullptr)
{

}

Evas_Object *LogView::onCreate(Evas_Object *parent)
{
	if (!m_LogProvider) {
		DBG("notifyLogWithChange");
		m_LogProvider = new Logs::Model::LogProvider(LogProvider::FilterAll);
	}
	return nullptr;
}

void LogView::getListType()
{
}
