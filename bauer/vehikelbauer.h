/*
 * This file is part of the Simutrans-Extended project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef BAUER_VEHIKELBAUER_H
#define BAUER_VEHIKELBAUER_H


#include "../dataobj/koord3d.h"
#include "../simtypes.h"
#include <string>

class vehicle_t;
class player_t;
class convoi_t;
class vehicle_desc_t;
class goods_desc_t;
template <class T> class slist_tpl;


/**
 * Builds vehicles. Vehicles should not be instantiated directly,
 * but always from a vehicle_builder_t produced. (Google)
 */
class vehicle_builder_t
{
public:
	// sorting categories
	enum sort_mode_t {
		best,
		sb_name,
		sb_value,
		sb_running_cost,
		sb_capacity,
		sb_speed,
		sb_range,
		sb_power,
		sb_tractive_force,
		sb_weight,
		sb_axle_load,
		sb_intro_date,
		sb_retire_date,
		sb_min_comfort,
		sb_length, // MAX for depot
		// -- Below are vehicle list only --
		sb_freight,
		sb_enigine_type,
		sb_role
	};
	static const char *vehicle_sort_by[sb_length];

	// default compare function
	static bool compare_vehicles(const vehicle_desc_t* a, const vehicle_desc_t* b, sort_mode_t mode);

	static const char *engine_type_names[11];

	// Retained for compatibility only
	static void rdwr_speedbonus(loadsave_t *file);

	static bool register_desc(vehicle_desc_t *desc);
	static bool successfully_loaded();

	static vehicle_t* build(koord3d k, player_t* player, convoi_t* cnv, const vehicle_desc_t* vb )
	{
		return build(k, player, cnv, vb, false);
	}

	static vehicle_t* build(koord3d k, player_t* player, convoi_t* cnv, const vehicle_desc_t* vb, bool upgrade, uint16 livery_scheme_index = 0 );

	static const vehicle_desc_t * get_info(const char *name);
	static slist_tpl<vehicle_desc_t*> const& get_info(waytype_t, uint8 sortkey = best);

	/** extended search for vehicles for AI
	*/
	static const vehicle_desc_t *vehicle_search(waytype_t typ,const uint16 month_now,const uint32 target_power,const sint32 target_speed, const goods_desc_t * target_freight, bool include_electric, bool not_obsolete );

	/* for replacement during load time
	 * prev_veh==NULL equals leading of convoi
	 */
	static const vehicle_desc_t *get_best_matching( waytype_t wt, const uint16 month_now, const uint32 target_weight, const uint32 target_power, const sint32 target_speed, const goods_desc_t * target_freight, bool not_obsolete, const vehicle_desc_t *prev_veh, bool is_last );
};

#endif
