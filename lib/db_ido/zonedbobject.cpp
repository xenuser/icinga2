/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2015 Icinga Development Team (http://www.icinga.org)    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "db_ido/zonedbobject.hpp"
#include "db_ido/dbtype.hpp"
#include "db_ido/dbvalue.hpp"
#include "icinga/icingaapplication.hpp"
#include "base/objectlock.hpp"
#include "base/initialize.hpp"
#include "base/dynamictype.hpp"
#include "base/utility.hpp"
#include "base/convert.hpp"
#include "base/logger.hpp"
#include <boost/foreach.hpp>

using namespace icinga;


REGISTER_DBTYPE(Zone, "zone", DbObjectTypeZone, "zone_object_id", ZoneDbObject);

INITIALIZE_ONCE(&ZoneDbObject::StaticInitialize);

void ZoneDbObject::StaticInitialize(void)
{
	/* TODO: find a way to signal zone connection updates from lib/remote/zones.cpp */
/*
	Zone::OnConnected.connect(boost::bind(&ZoneDbObject::UpdateConnectedStatus, _1));
	Zone::OnDisconnected.connect(boost::bind(&ZoneDbObject::UpdateConnectedStatus, _1));
*/
}

ZoneDbObject::ZoneDbObject(const DbType::Ptr& type, const String& name1, const String& name2)
	: DbObject(type, name1, name2)
{ }

Dictionary::Ptr ZoneDbObject::GetConfigFields(void) const
{
	Dictionary::Ptr fields = new Dictionary();
	Zone::Ptr zone = static_pointer_cast<Zone>(GetObject());

	fields->Set("is_global", zone->IsGlobal() ? 1 : 0);
	fields->Set("parent_zone_object_id", zone->GetParent());

	return fields;
}

Dictionary::Ptr ZoneDbObject::GetStatusFields(void) const
{
	Zone::Ptr zone = static_pointer_cast<Zone>(GetObject());

	Log(LogDebug, "ZoneDbObject")
	    << "update status for zone '" << zone->GetName() << "'";

	std::pair<bool, double> bag = GetConnectionLag(zone);

	Dictionary::Ptr fields = new Dictionary();
	fields->Set("is_connected", (bag.first ? 1 : 0));
	fields->Set("log_lag", bag.second);
	fields->Set("parent_zone_object_id", zone->GetParent());

	return fields;
}

void ZoneDbObject::UpdateConnectedStatus(const Zone::Ptr& zone)
{
	std::pair<bool, double> bag = GetConnectionLag(zone);

	DbQuery query1;
	query1.Table = "zonestatus";
	query1.Type = DbQueryUpdate;
	query1.Category = DbCatState;

	Dictionary::Ptr fields1 = new Dictionary();
	fields1->Set("is_connected", (bag.first ? 1 : 0));
	fields1->Set("log_lag", bag.second);
	fields1->Set("status_update_time", DbValue::FromTimestamp(Utility::GetTime()));
	fields1->Set("parent_zone_object_id", zone->GetParent());
	query1.Fields = fields1;

	query1.WhereCriteria = new Dictionary();
	query1.WhereCriteria->Set("zone_object_id", zone);
	query1.WhereCriteria->Set("instance_id", 0); /* DbConnection class fills in real ID */

	OnQuery(query1);
}

void ZoneDbObject::OnConfigUpdate(void)
{
	/* update current status on config dump once */
	Zone::Ptr zone = static_pointer_cast<Zone>(GetObject());

	std::pair<bool, double> bag = GetConnectionLag(zone);

	DbQuery query1;
	query1.Table = "zonestatus";
	query1.Type = DbQueryInsert;
	query1.Category = DbCatConfig;

	Dictionary::Ptr fields1 = new Dictionary();
	fields1->Set("is_connected", (bag.first ? 1 : 0));
	fields1->Set("log_lag", bag.second);
	fields1->Set("status_update_time", DbValue::FromTimestamp(Utility::GetTime()));
	fields1->Set("parent_zone_object_id", zone->GetParent());
	fields1->Set("instance_id", 0); /* DbConnection class fills in real ID */
	query1.Fields = fields1;

	OnQuery(query1);
}

std::pair<bool, double> ZoneDbObject::GetConnectionLag(const Zone::Ptr& zone)
{
	bool connected = false;
	double lag = 0;

        BOOST_FOREACH(const Endpoint::Ptr& endpoint, zone->GetEndpoints()) {
                double eplag = Utility::GetTime() - endpoint->GetRemoteLogPosition();

                if (endpoint->IsConnected())
                        connected = true;

                if ((endpoint->GetSyncing() || !endpoint->IsConnected()) && eplag > lag)
                        lag = eplag;
        }

	return std::make_pair(connected, lag);
}
