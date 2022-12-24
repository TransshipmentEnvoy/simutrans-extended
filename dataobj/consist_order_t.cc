/* This file is part of the Simutrans project under the artistic licence.
* (see licence.txt)
*
* @author: jamespetts, February 2018
*/

#include "consist_order_t.h"
#include "../simconvoi.h"

#include "../utils/cbuffer_t.h"

#include "loadsave.h"
#include "translator.h"

#include "../bauer/vehikelbauer.h"
#include "../vehicle/vehicle.h"


void consist_order_element_t::append_vehicle(const vehicle_desc_t *v, bool is_specific)
{
	vehicle_description_element v_elem;
	if (is_specific) {
		v_elem.specific_vehicle = v;
	}
	else {
		v_elem.set_vehicle_spec(v);
	}
	vehicle_description.append(v_elem);
}


void consist_order_element_t::increment_index(uint32 description_index)
{
	if( (description_index>=vehicle_description.get_count()-1)  ||  vehicle_description.get_count()<2 ) { return; }
	vehicle_description.insert_at(description_index+2, vehicle_description.get_element(description_index));
	vehicle_description.remove_at(description_index);
}


void consist_order_t::set_convoy_order(uint32 element_number, convoihandle_t cnv, bool specific)
{
	if( !cnv.is_bound() ) { return; }
	if( element_number >= orders.get_count() ) {
		dbg->warning("consist_order_t::set_convoy_order", "Invalid element_number. elem=%u total_orders=%u", element_number, orders.get_count()); return;
	}

	orders[element_number].clear_vehicles();
	for (uint8 i=0; i < cnv->get_vehicle_count(); i++) {
		orders[element_number].append_vehicle(cnv->get_vehicle(i)->get_desc(), specific);
	}
}


void consist_order_t::rdwr(loadsave_t* file)
{
	file->rdwr_short(tags_to_clear);

	uint32 orders_count = orders.get_count();
	file->rdwr_long(orders_count);

	if(file->is_saving())
	{
		for(auto order : orders)
		{
			file->rdwr_byte(order.catg_index);
			file->rdwr_bool(order.clear_all_tags);
			file->rdwr_short(order.tags_required);
			file->rdwr_short(order.tags_to_set);

			uint32 vehicle_description_count = order.vehicle_description.get_count();
			file->rdwr_long(vehicle_description_count);

			for(auto vehicle_description : order.vehicle_description)
			{
				// Save each element of the vehicle descriptors
				const char *s = vehicle_description.specific_vehicle ? vehicle_description.specific_vehicle->get_name() : "~BLANK~";
				file->rdwr_str(s);

				file->rdwr_bool(vehicle_description.empty);
				file->rdwr_byte(vehicle_description.engine_type);
				file->rdwr_byte(vehicle_description.min_catering);
				file->rdwr_byte(vehicle_description.max_catering);
				file->rdwr_byte(vehicle_description.must_carry_class);
				file->rdwr_long(vehicle_description.min_range);
				file->rdwr_long(vehicle_description.max_range);
				file->rdwr_short(vehicle_description.min_brake_force);
				file->rdwr_short(vehicle_description.max_brake_force);
				file->rdwr_long(vehicle_description.min_power);
				file->rdwr_long(vehicle_description.max_power);
				file->rdwr_long(vehicle_description.min_tractive_effort);
				file->rdwr_long(vehicle_description.max_tractive_effort);
				file->rdwr_long(vehicle_description.max_topspeed);
				file->rdwr_long(vehicle_description.min_topspeed);
				file->rdwr_long(vehicle_description.max_weight);
				file->rdwr_long(vehicle_description.min_weight);
				file->rdwr_long(vehicle_description.max_axle_load);
				file->rdwr_long(vehicle_description.min_axle_load);
				file->rdwr_short(vehicle_description.min_capacity);
				file->rdwr_short(vehicle_description.max_capacity);
				file->rdwr_long(vehicle_description.max_running_cost);
				file->rdwr_long(vehicle_description.min_running_cost);
				file->rdwr_long(vehicle_description.max_fixed_cost);
				file->rdwr_long(vehicle_description.min_fixed_cost);
				file->rdwr_long(vehicle_description.max_fuel_per_km);
				file->rdwr_long(vehicle_description.min_fuel_per_km);
				file->rdwr_long(vehicle_description.max_staff_hundredths);
				file->rdwr_long(vehicle_description.min_staff_hundredths);
				file->rdwr_long(vehicle_description.max_drivers);
				file->rdwr_long(vehicle_description.min_drivers);

				for(uint32 i = 0; i < vehicle_description_element::max_rule_flags; i ++)
				{
					file->rdwr_long(vehicle_description.rule_flags[i]);
				}
			}
		}
	}

	if (file->is_loading())
	{
		for (uint32 i = 0; i < orders_count; i++)
		{
			// Load each order
			consist_order_element_t order;

			file->rdwr_byte(order.catg_index);
			file->rdwr_bool(order.clear_all_tags);
			file->rdwr_short(order.tags_required);
			file->rdwr_short(order.tags_to_set);

			uint32 vehicle_description_count = 0;
			file->rdwr_long(vehicle_description_count);

			for(uint32 j = 0; j < vehicle_description_count; j ++)
			{
				vehicle_description_element vehicle_description;

				char vehicle_name[512];
				file->rdwr_str(vehicle_name, 512);
				const vehicle_desc_t* desc = vehicle_builder_t::get_info(vehicle_name);
				if(desc == nullptr)
				{
					desc = vehicle_builder_t::get_info(translator::compatibility_name(vehicle_name));
				}
				vehicle_description.specific_vehicle = desc;

				file->rdwr_bool(vehicle_description.empty);
				file->rdwr_byte(vehicle_description.engine_type);
				file->rdwr_byte(vehicle_description.min_catering);
				file->rdwr_byte(vehicle_description.max_catering);
				file->rdwr_byte(vehicle_description.must_carry_class);
				file->rdwr_long(vehicle_description.min_range);
				file->rdwr_long(vehicle_description.max_range);
				file->rdwr_short(vehicle_description.min_brake_force);
				file->rdwr_short(vehicle_description.max_brake_force);
				file->rdwr_long(vehicle_description.min_power);
				file->rdwr_long(vehicle_description.max_power);
				file->rdwr_long(vehicle_description.min_tractive_effort);
				file->rdwr_long(vehicle_description.max_tractive_effort);
				file->rdwr_long(vehicle_description.max_topspeed);
				file->rdwr_long(vehicle_description.min_topspeed);
				file->rdwr_long(vehicle_description.max_weight);
				file->rdwr_long(vehicle_description.min_weight);
				file->rdwr_long(vehicle_description.max_axle_load);
				file->rdwr_long(vehicle_description.min_axle_load);
				file->rdwr_short(vehicle_description.min_capacity);
				file->rdwr_short(vehicle_description.max_capacity);
				file->rdwr_long(vehicle_description.max_running_cost);
				file->rdwr_long(vehicle_description.min_running_cost);
				file->rdwr_long(vehicle_description.max_fixed_cost);
				file->rdwr_long(vehicle_description.min_fixed_cost);
				file->rdwr_long(vehicle_description.max_fuel_per_km);
				file->rdwr_long(vehicle_description.min_fuel_per_km);
				file->rdwr_long(vehicle_description.max_staff_hundredths);
				file->rdwr_long(vehicle_description.min_staff_hundredths);
				file->rdwr_long(vehicle_description.max_drivers);
				file->rdwr_long(vehicle_description.min_drivers);

				for(uint32 i = 0; i < vehicle_description_element::max_rule_flags; i ++)
				{
					file->rdwr_long(vehicle_description.rule_flags[i]);
				}

				order.vehicle_description.append(vehicle_description);
			}

			orders.append(order);
		}
	}
}

void consist_order_t::sprintf_consist_order(cbuffer_t &buf) const
{
	buf.append_fixed(tags_to_clear);
	buf.append_fixed(orders.get_count());
	for (auto order : orders)
	{
		buf.append_fixed(order.catg_index);
		buf.append_fixed(order.clear_all_tags == true ? (uint8)1 : (uint8)0);
		buf.append_fixed(order.tags_required);
		buf.append_fixed(order.tags_to_set);
		buf.append_fixed(order.vehicle_description.get_count());
		for(auto desc : order.vehicle_description)
		{
			buf.append(desc.specific_vehicle ? desc.specific_vehicle->get_name() : "NULL");
			buf.append("|");
			buf.append_fixed(desc.empty == true ? (uint8)1 : (uint8)0);
			buf.append_fixed(desc.engine_type);
			buf.append_fixed(desc.min_catering);
			buf.append_fixed(desc.must_carry_class);
			buf.append_fixed(desc.min_range);
			buf.append_fixed(desc.max_range);
			buf.append_fixed(desc.min_brake_force);
			buf.append_fixed(desc.max_brake_force);
			buf.append_fixed(desc.min_power);
			buf.append_fixed(desc.max_power);
			buf.append_fixed(desc.min_tractive_effort);
			buf.append_fixed(desc.max_tractive_effort);
			buf.append_fixed(desc.min_topspeed);
			buf.append_fixed(desc.max_topspeed);
			buf.append_fixed(desc.max_weight);
			buf.append_fixed(desc.min_weight);
			buf.append_fixed(desc.max_axle_load);
			buf.append_fixed(desc.min_axle_load);
			buf.append_fixed(desc.min_capacity);
			buf.append_fixed(desc.max_capacity);
			buf.append_fixed(desc.max_running_cost);
			buf.append_fixed(desc.min_running_cost);
			buf.append_fixed(desc.max_fixed_cost);
			buf.append_fixed(desc.min_fixed_cost);
			buf.append_fixed(desc.max_fuel_per_km);
			buf.append_fixed(desc.min_fuel_per_km);
			buf.append_fixed(desc.max_staff_hundredths);
			buf.append_fixed(desc.min_staff_hundredths);
			buf.append_fixed(desc.max_drivers);
			buf.append_fixed(desc.min_drivers);

			for(uint32 i = 0; i < vehicle_description_element::max_rule_flags; i ++)
			{
				buf.append_fixed(desc.rule_flags[i]);
			}
		}
	}
}

const char* consist_order_t::sscanf_consist_order(const char* ptr)
{
	const char *p = ptr;
	orders.clear();

	tags_to_clear = cbuffer_t::decode_uint16(p);

	// Retrieve the count
	const uint32 order_count = cbuffer_t::decode_uint32(p);

	// Now use the count to determine the number of orders retrieved
	for(uint32 i = 0;  i < order_count; i ++)
	{
		consist_order_element_t element;

		element.catg_index = cbuffer_t::decode_uint8(p);
		element.clear_all_tags = cbuffer_t::decode_uint8(p);
		element.tags_required = cbuffer_t::decode_uint16(p);
		element.tags_to_set = cbuffer_t::decode_uint16(p);

		uint32 vehicle_description_count = cbuffer_t::decode_uint32(p);
		for(uint32 j = 0; j < vehicle_description_count; j ++)
		{
			vehicle_description_element desc;
			char vehicle_name[256];
			uint8 n = 0;
			while(*p!='|' && *p!='\0')
			{
				vehicle_name[n++] = *p++;
			}
			vehicle_name[n] = '\0';

			const vehicle_desc_t* vehicle_desc = vehicle_builder_t::get_info(vehicle_name);
			if (strcmp(vehicle_name, "NULL"))
			{
				if (vehicle_desc == NULL)
				{
					vehicle_desc = vehicle_builder_t::get_info(translator::compatibility_name(vehicle_name));
				}
				if (vehicle_desc == NULL)
				{
					dbg->warning("consist_order_t::sscanf_consist_order()", "no vehicle found when searching for '%s', but this was not intended to be a blank consist order slot", vehicle_name);
				}
				else
				{
					desc.specific_vehicle = vehicle_desc;
				}
			}
			else
			{
				desc.specific_vehicle = NULL;
			}
			p++;

			desc.empty = cbuffer_t::decode_uint8(p);
			desc.engine_type = cbuffer_t::decode_uint8(p);
			desc.min_catering = cbuffer_t::decode_uint8(p);
			desc.must_carry_class = cbuffer_t::decode_uint8(p);
			desc.min_range = cbuffer_t::decode_uint32(p);
			desc.max_range = cbuffer_t::decode_uint32(p);
			desc.min_brake_force = cbuffer_t::decode_uint16(p);
			desc.max_brake_force = cbuffer_t::decode_uint16(p);
			desc.min_power = cbuffer_t::decode_uint32(p);
			desc.max_power = cbuffer_t::decode_uint32(p);
			desc.min_tractive_effort = cbuffer_t::decode_uint32(p);
			desc.max_tractive_effort = cbuffer_t::decode_uint32(p);
			desc.min_topspeed = cbuffer_t::decode_uint32(p);
			desc.max_topspeed = cbuffer_t::decode_uint32(p);
			desc.max_weight = cbuffer_t::decode_uint32(p);
			desc.min_weight = cbuffer_t::decode_uint32(p);
			desc.max_axle_load = cbuffer_t::decode_uint32(p);
			desc.min_axle_load = cbuffer_t::decode_uint32(p);
			desc.min_capacity = cbuffer_t::decode_uint16(p);
			desc.max_capacity = cbuffer_t::decode_uint16(p);
			desc.max_running_cost = cbuffer_t::decode_uint32(p);
			desc.min_running_cost = cbuffer_t::decode_uint32(p);
			desc.max_fixed_cost = cbuffer_t::decode_uint32(p);
			desc.min_fixed_cost = cbuffer_t::decode_uint32(p);
			desc.max_fuel_per_km = cbuffer_t::decode_uint32(p);
			desc.min_fuel_per_km = cbuffer_t::decode_uint32(p);
			desc.max_staff_hundredths = cbuffer_t::decode_uint32(p);
			desc.min_staff_hundredths = cbuffer_t::decode_uint32(p);
			desc.max_drivers = cbuffer_t::decode_uint32(p);
			desc.min_drivers = cbuffer_t::decode_uint32(p);

			for (uint32 k = 0; k < vehicle_description_element::max_rule_flags; k++)
			{
				desc.rule_flags[k] = cbuffer_t::decode_uint32(p);
			}

			element.vehicle_description.append(desc);
		}
		orders.append(element);
	}

	return p;
}

bool consist_order_t::operator== (const consist_order_t& other) const
{
	if (tags_to_clear == other.tags_to_clear && orders.get_count() == other.orders.get_count())
	{
		// Check each individual element
		for(uint32 i = 0; i < orders.get_count(); i ++)
		{
			const consist_order_element_t this_element = orders.get_element(i);
			const consist_order_element_t other_element = other.orders.get_element(i);
			if (this_element != other_element)
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool consist_order_element_t::operator!= (const consist_order_element_t& other) const
{
	if (catg_index == other.catg_index && clear_all_tags == other.clear_all_tags && tags_required == other.tags_required && tags_to_set == other.tags_to_set && vehicle_description.get_count() == other.vehicle_description.get_count())
	{
		// Check each individual vehicle description element
		// ...
		for (uint32 i = 0; i < vehicle_description.get_count(); i++)
		{
			const vehicle_description_element this_element = vehicle_description.get_element(i);
			const vehicle_description_element other_element = other.vehicle_description.get_element(i);
			if (this_element != other_element)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return true;
	}

}

bool vehicle_description_element::operator!= (const vehicle_description_element& other) const
{
	bool return_value =
		specific_vehicle != other.specific_vehicle ||
		empty != other.empty ||
		engine_type != other.engine_type ||
		min_catering != other.min_catering ||
		max_catering != other.max_catering ||
		must_carry_class != other.must_carry_class ||
		min_range != other.min_range ||
		max_range != other.max_range ||
		min_brake_force != other.min_brake_force ||
		max_brake_force != other.max_brake_force ||
		min_power != other.min_power ||
		max_power != other.max_power ||
		min_tractive_effort != other.min_tractive_effort ||
		max_tractive_effort != other.max_tractive_effort ||
		max_weight != other.max_weight ||
		min_weight != other.min_weight ||
		max_axle_load != other.max_axle_load ||
		min_axle_load != other.min_axle_load ||
		min_capacity != other.min_capacity ||
		max_capacity != other.max_capacity ||
		max_running_cost != other.max_running_cost ||
		min_running_cost != other.min_running_cost ||
		max_fixed_cost != other.max_fixed_cost ||
		min_fixed_cost != other.min_fixed_cost ||
		max_fuel_per_km != other.max_fuel_per_km ||
		min_fuel_per_km != other.min_fuel_per_km ||
		max_staff_hundredths != other.max_staff_hundredths ||
		min_staff_hundredths != other.min_staff_hundredths ||
		max_drivers != other.max_drivers ||
		min_drivers != other.min_drivers;

	for (uint32 i = 0; i < max_rule_flags; i++)
	{
		if (rule_flags[i] != other.rule_flags[i])
		{
			return true;
		}
	}

	return return_value;
}
