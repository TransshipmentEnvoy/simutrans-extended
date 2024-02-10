/*
 * This file is part of the Simutrans-Extended project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef SIMFAB_H
#define SIMFAB_H


#include <algorithm>

#include "dataobj/koord3d.h"
#include "dataobj/translator.h"

#include "tpl/slist_tpl.h"
#include "tpl/vector_tpl.h"
#include "tpl/array_tpl.h"
#include "tpl/inthashtable_tpl.h"
#include "descriptor/factory_desc.h"
#include "halthandle_t.h"
#include "simworld.h"
#include "utils/plainstring.h"
#include "bauer/goods_manager.h"


class player_t;
class stadt_t;
class ware_t;


/**
 * Factory statistics
 */
#define MAX_MONTH                  (12)
#define FAB_PRODUCTION              (0) // total boost rate - it means productivity
#define FAB_POWER                   (1)
#define FAB_BOOST_ELECTRIC          (2)
#define FAB_BOOST_PAX               (3)
#define FAB_BOOST_MAIL              (4)
#define FAB_PAX_GENERATED           (5) // now unused, almost same as FAB_PAX_ARRIVED.
#define FAB_PAX_DEPARTED            (6) // almost same as FAB_PAX_ARRIVED
#define FAB_PAX_ARRIVED             (7)
#define FAB_MAIL_GENERATED          (8) // now unused, same as FAB_MAIL_DEPARTED.
#define FAB_MAIL_DEPARTED           (9)
#define FAB_MAIL_ARRIVED           (10)
#define FAB_CONSUMER_ARRIVED       (11)
#define MAX_FAB_STAT               (12)

// reference lines
#define FAB_REF_MAX_BOOST_ELECTRIC  (0)
#define FAB_REF_MAX_BOOST_PAX       (1)
#define FAB_REF_MAX_BOOST_MAIL      (2)
#define FAB_REF_DEMAND_ELECTRIC     (3)
#define FAB_REF_DEMAND_PAX          (4)
#define FAB_REF_DEMAND_MAIL         (5)
#define MAX_FAB_REF_LINE            (6)

// statistics for goods
#define MAX_FAB_GOODS_STAT          (4)
// common to both input and output goods
#define FAB_GOODS_STORAGE           (0)
// input goods
#define FAB_GOODS_RECEIVED          (1)
#define FAB_GOODS_CONSUMED          (2)
#define FAB_GOODS_TRANSIT           (3)
// output goods
#define FAB_GOODS_DELIVERED         (1)
#define FAB_GOODS_PRODUCED          (2)


// production happens in every second
#define PRODUCTION_DELTA_T_BITS (10)
#define PRODUCTION_DELTA_T (1 << PRODUCTION_DELTA_T_BITS)
// default production factor
#define DEFAULT_PRODUCTION_FACTOR_BITS (8)
#define DEFAULT_PRODUCTION_FACTOR (1 << DEFAULT_PRODUCTION_FACTOR_BITS)
// precision of apportioned demand (i.e. weights of factories at target cities)
#define DEMAND_BITS (4)


/**
 * Convert internal values to displayed values
 */
sint64 convert_goods(sint64 value);
sint64 convert_power(sint64 value);
sint64 convert_boost(sint64 value);

class ware_production_t
{
private:
	const goods_desc_t *type;
	// statistics for each goods
	sint64 statistics[MAX_MONTH][MAX_FAB_GOODS_STAT];

	///factory links to supplier or consumer
	vector_tpl<koord> links;
	vector_tpl<sint32> link_aux;

	sint32 total_contracts;

	/// clears statistics, transit, and weighted_sum_storage
	void init_stats();

	bool using_contracts;

public:
	ware_production_t() : type(NULL), total_contracts(0), using_contracts(0), menge(0), max(0)/*, transit(statistics[0][FAB_GOODS_TRANSIT])*/,
		max_transit(0), index_offset(0)
	{
		init_stats();
	}

	const goods_desc_t* get_typ() const { return type; }
	void set_typ(const goods_desc_t *t) { type=t; }

	// functions for manipulating goods statistics
	void roll_stats(uint32 factor);
	void rdwr(loadsave_t *file, uint8 sub_version=0);
	const sint64* get_stats() const { return *statistics; }
	void book_stat(sint64 value, int stat_type) { assert(stat_type<MAX_FAB_GOODS_STAT); statistics[0][stat_type] += value; }
	void book_stat_no_negative(sint64 value, int stat_type) { assert(stat_type < MAX_FAB_GOODS_STAT); statistics[0][stat_type] += (std::max(value, -statistics[0][stat_type])); }
	void set_stat(sint64 value, int stat_type) { assert(stat_type<MAX_FAB_GOODS_STAT); statistics[0][stat_type] = value; }
	sint64 get_stat(int month, int stat_type) const { assert(stat_type<MAX_FAB_GOODS_STAT); return statistics[month][stat_type]; }

	/**
	 * convert internal units to displayed values
	 */
	sint64 get_stat_converted(int month, int stat_type) const {
		assert(stat_type<MAX_FAB_STAT);
		sint64 value = statistics[month][stat_type];
		switch (stat_type) {
		case FAB_POWER:
			value = convert_power(value);
			break;
		case FAB_BOOST_ELECTRIC:
		case FAB_BOOST_PAX:
		case FAB_BOOST_MAIL:
			value = convert_boost(value);
			break;
		default:;
		}
		return value;
	}

	void book_weighted_sum_storage(uint32 factor);

	sint32 menge; // in internal units shifted by precision_bits (see step)
	sint32 max;
	// returns goods chart value (convert internal value to display value)
	sint32 get_storage() const { return (sint32)convert_goods(statistics[0][FAB_GOODS_STORAGE]); }
	sint32 get_capacity(uint32 factor) const { return (sint32)convert_goods(max*factor); }
	/// Cargo currently in transit from/to this slot. Equivalent to statistics[0][FAB_GOODS_TRANSIT].
	sint32 get_in_transit() const { return (sint32)statistics[0][FAB_GOODS_TRANSIT]; }
	/// Current limit on cargo in transit, depending on suppliers mean distance.
	sint32 max_transit;

	uint32 index_offset; // used for haltlist and consumers searches in verteile_waren to produce round robin results

	const vector_tpl<koord>& get_links() const {return links;}

	void set_using_contracts(sint32 default_contracts=0){
		link_aux.set_count(links.get_count());
		for(uint32 i = 0; i < link_aux.get_count(); i++){
			link_aux[i]=default_contracts;
		}
		using_contracts=true;
	}

	void reset_using_contracts(){
		link_aux.clear();
		link_aux.resize(0);
		using_contracts=false;
	}

	bool get_using_contracts(){
		return using_contracts;
	}

	void verify_contracts(){
		assert(!using_contracts || link_aux.get_count()==links.get_count());
	}

	sint32 get_contract(uint32 idx){ return using_contracts ? link_aux[idx] : 0;}
	void set_contract(uint32 idx, sint32 contract){
		link_aux[idx]=contract;
	}
	void add_contract(uint32 idx, sint32 diff){
		link_aux[idx]+=diff;
	}
	void sub_contract(uint32 idx, sint32 diff){
		link_aux[idx]-=diff;
	}

	static void add_contracts(sint32 additon, ware_production_t& ware_in, ware_production_t& ware_out, uint32 output_index, sint32 max_output);

	void reset_total_contracts(){total_contracts=0;}
	void add_total_contracts(sint32 diff){
		total_contracts+=diff;
	}
	void sub_total_contracts(sint32 diff){
		total_contracts-=diff;
	}

	sint32 get_total_contracts() const {return total_contracts;}

	template<class StrictWeakOrdering>
	void link_add(koord pos, StrictWeakOrdering T, sint64 aux = 0 ){
		//Note this is simular to vector<>::insert_unique_ordered but for two vectors
		sint32 low = -1, high = links.get_count();
		while( high - low > 1){
			const sint32 mid = ((uint32)(low + high)) / 2;
			if( links[mid] == pos ){
				return;
			}else if( T(pos,links[mid])){
				high=mid;
			}else{
				low=mid;
			}
		}
		links.insert_at(high,pos);
		if(using_contracts){
			link_aux.insert_at(high,aux);
		}
	}

	bool link_contained(koord pos) const{
		return links.is_contained(pos);
	}

	bool link_remove(koord pos){
		for(uint32 i = 0; i < links.get_count(); i++){
			if(links[i]==pos){
				links.remove_at(i);
				if(using_contracts){
					link_aux.remove_at(i);
				}
				return true;
			}
		}
		return false;
	}

	bool link_remove(uint32 idx){
		links.remove_at(idx);
		if(using_contracts){
			link_aux.remove_at(idx);
		}
		return true;
	}

	uint32 link_index(koord pos){
		for(uint32 i = 0; i < links.get_count(); i++){
			if(links[i]==pos){
				return i;
			}
		}
		return UINT32_MAX_VALUE;
	}

	uint32 link_count() const{
		return links.get_count();
	}

	koord link_from_index(uint32 i) const{
		return links[i];
	}

	vector_tpl<koord>::iterator link_begin() {
		return links.begin();
	}

	vector_tpl<koord>::iterator link_end() {
		return links.end();
	}

	vector_tpl<koord>::const_iterator link_begin() const{
		return links.begin();
	}

	vector_tpl<koord>::const_iterator link_end() const{
		return links.end();
	}
};

class ware_link_iteratable_t{
private:
	array_tpl<ware_production_t> *ware_list;
public:
	ware_link_iteratable_t(array_tpl<ware_production_t> *list=0){
		ware_list=list;
	}
	class iterator{
	public:
		typedef std::forward_iterator_tag	iterator_catagory;
		typedef koord						value_type;
		typedef ptrdiff_t					difference_type;
		typedef value_type*					pointer;
		typedef value_type&					reference;

		iterator() {
			ware_iterator=array_tpl<ware_production_t>::iterator();
			koord_iterator=vector_tpl<koord>::iterator();
		}

		pointer operator ->() const {return &*koord_iterator;}
		reference operator *() const {return *koord_iterator;}

		iterator& operator ++(){
			if(++koord_iterator == ware_iterator->link_end()){
				for(;;){
					if(++ware_iterator == ware_end){
						ware_iterator=array_tpl<ware_production_t>::iterator();
						koord_iterator=vector_tpl<koord>::iterator();
						break;
					}
					if(ware_iterator->link_count()){
						koord_iterator = ware_iterator->link_begin();
						break;
					}
				}
			}
			return *this;
		}

		bool operator ==(iterator const& o) const {return ware_iterator==o.ware_iterator && koord_iterator==o.koord_iterator;}
		bool operator !=(iterator const& o) const {return ware_iterator!=o.ware_iterator || koord_iterator!=o.koord_iterator;}

		ware_production_t& get_ware() {return *ware_iterator;}
		uint32 get_ware_index(){return koord_iterator - ware_iterator->link_begin();}

	private:
		array_tpl<ware_production_t>::iterator ware_iterator;
		array_tpl<ware_production_t>::iterator ware_end;
		vector_tpl<koord>::iterator koord_iterator;

		iterator(array_tpl<ware_production_t>::iterator _ware_iterator, array_tpl<ware_production_t>::iterator _ware_end, vector_tpl<koord>::iterator _koord_iterator) : ware_iterator(_ware_iterator), ware_end(_ware_end), koord_iterator(_koord_iterator) {};

		friend class ware_link_iteratable_t;
	};

	iterator begin() {
		for(array_tpl<ware_production_t>::iterator ware_begin = ware_list->begin(); ware_begin != ware_list->end(); ++ware_begin){;
			if(ware_begin->link_count()){
				return iterator(ware_begin,ware_list->end(),ware_begin->link_begin());
			}
		}
		return end();
	}

	iterator end(){
		return iterator();
	}

	const koord operator[](uint32 i) const {
		for(auto ware : *ware_list){
			if(i < ware.link_count()){
				return ware.link_from_index(i);
			}
			i-= ware.link_count();
		}

		return koord::invalid;
	}

	uint32 index_of(koord k) const {
		uint32 i=0;
		for(auto ware : *ware_list){
			for(uint32 j=0; j < ware.link_count(); j++){
				if(ware.link_from_index(j)==k){
					return i;
				}
				i++;
			}
		}
		return 0xFFFFFFFF;
	}

	uint32 get_count() const{
		if(!ware_list) return 0;
		uint32 cnt=0;
		for(auto ware : *ware_list){
			cnt+=ware.link_count();
		}
		return cnt;
	}

	bool empty() const {
		if(!ware_list) return true;
		for(auto ware : *ware_list){
			if(ware.link_count()){
				return false;
			}
		}
		return true;
	}
};

class ware_link_const_iteratable_t{
private:
	const array_tpl<ware_production_t> *ware_list;
public:
	ware_link_const_iteratable_t(const array_tpl<ware_production_t> *list=0){
		ware_list=list;
	}

	class const_iterator{
	public:
		typedef std::forward_iterator_tag	iterator_catagory;
		typedef koord						value_type;
		typedef ptrdiff_t					difference_type;
		typedef value_type const*					pointer;
		typedef value_type const&					reference;

		const_iterator() {
			ware_iterator=array_tpl<ware_production_t>::iterator();
			koord_iterator=vector_tpl<koord>::iterator();
		}

		pointer operator ->() const {return &*koord_iterator;}
		reference operator *() const {return *koord_iterator;}

		const_iterator& operator ++(){
			if(++koord_iterator == ware_iterator->link_end()){
				for(;;){
					if(++ware_iterator == ware_end){
						ware_iterator=array_tpl<ware_production_t>::iterator();
						koord_iterator=vector_tpl<koord>::iterator();
						break;
					}
					if(ware_iterator->link_count()){
						koord_iterator = ware_iterator->link_begin();
						break;
					}
				}
			}
			return *this;
		}

		bool operator ==(const_iterator const& o) const {return ware_iterator==o.ware_iterator && koord_iterator==o.koord_iterator;}
		bool operator !=(const_iterator const& o) const {return ware_iterator!=o.ware_iterator || koord_iterator!=o.koord_iterator;}


	private:
		array_tpl<ware_production_t>::const_iterator ware_iterator;
		array_tpl<ware_production_t>::const_iterator ware_end;
		vector_tpl<koord>::const_iterator koord_iterator;

		const_iterator(array_tpl<ware_production_t>::const_iterator _ware_iterator, array_tpl<ware_production_t>::const_iterator _ware_end, vector_tpl<koord>::const_iterator _koord_iterator) : ware_iterator(_ware_iterator), ware_end(_ware_end), koord_iterator(_koord_iterator) {};

		friend class ware_link_const_iteratable_t;
	};

	const_iterator begin() const {
		for(auto ware_begin = ware_list->begin(); ware_begin != ware_list->end(); ++ware_begin){;
			if(ware_begin->link_count()){
				return const_iterator(ware_begin,ware_list->end(),ware_begin->link_begin());
			}
		}
		return end();
	}

	const_iterator end() const{
		return const_iterator();
	}

	const koord operator[](uint32 i) const{
		for(auto ware : *ware_list){
			if(i < ware.link_count()){
				return ware.link_from_index(i);
			}
			i-= ware.link_count();
		}

		return koord::invalid;
	}

	uint32 index_of(koord k) const {
		uint32 i=0;
		for(auto ware : *ware_list){
			for(uint32 j=0; j < ware.link_count(); j++){
				if(ware.link_from_index(j)==k){
					return i;
				}
				i++;
			}
		}
		return 0xFFFFFFFF;
	}

	uint32 get_count() const{
		uint32 cnt=0;
		for(auto ware : *ware_list){
			cnt+=ware.link_count();
		}
		return cnt;
	}

	bool empty() const {
		for(auto ware : *ware_list){
			if(ware.link_count()){
				return false;
			}
		}
		return true;
	}

};

/**
 * Factories produce and consume goods (ware_t) and supply nearby halts.
 *
 * A class of factories in Simutrans.
 *  Factories produce and consume goods and supplies near bus stops.
 * The query functions return -1 if a product is never produced or consumed,
 * 0 when nothing is manufactured or consumed and> 0 otherwise (equivalent to stocks / consumption).
 * @see haltestelle_t
 */
class fabrik_t
{
public:
	/**
	 * Constants
	 */
	enum {
		old_precision_bits = 10,
		precision_bits     = 10,
		precision_mask     = (1 << precision_bits) - 1
	};

private:

	gebaeude_t* building;

	/**
	 * Factory statistics
	 */
	sint64 statistics[MAX_MONTH][MAX_FAB_STAT];

	// Functions for manipulating factory statistics
	void init_stats();
	void set_stat(sint64 value, int stat_type) { assert(stat_type<MAX_FAB_STAT); statistics[0][stat_type] = value; }

	// For accumulating weighted sums for average statistics
	void book_weighted_sums();

	/// Possible destinations for produced goods
	/// Deprecated, only used for read/write
	vector_tpl <koord> consumers;
	vector_tpl <sint32> contracts;
	vector_tpl<uint32> consumers_active_this_month;

	///number of months without any production
	uint32 months_unproductive;
	///number of months where ware total contract(s) are zero
	uint32 months_missing_contracts;

	/**
	 * fields of this factory (only for farms etc.)
	 */
	struct field_data_t
	{
		koord location;
		uint16 field_class_index;

		field_data_t() : field_class_index(0) {}
		explicit field_data_t(const koord loc) : location(loc), field_class_index(0) {}
		field_data_t(const koord loc, const uint16 class_index) : location(loc), field_class_index(class_index) {}

		bool operator==(const field_data_t &other) const { return location==other.location; }
	};
	vector_tpl <field_data_t> fields;

	/**
	 * The produced were distributed at the stops
	 */
	void verteile_waren(const uint32 product);

	player_t *owner;
	static karte_ptr_t welt;

	const factory_desc_t *desc;

protected:

	/**
	 * Freight halts within range
	 */
	vector_tpl<nearby_halt_t> nearby_freight_halts;

private:
	/**
	 * Passenger halts within range
	 */
	vector_tpl<nearby_halt_t> nearby_passenger_halts;
	/**
	 * Mail halts within range
	 */
	vector_tpl<nearby_halt_t> nearby_mail_halts;

	/**
	 * Is construction site rotated?
	 */
	uint8 rotate;

	/**
	 * production base amount
	 */
	sint32 prodbase;

	/**
	 * multipliers for the production base amount
	 */
	sint32 prodfactor_electric;
	sint32 prodfactor_pax;
	sint32 prodfactor_mail;

	array_tpl<ware_production_t> input; /// array for input/consumed goods
	array_tpl<ware_production_t> output; /// array for output/produced goods

	/// Accumulated time since last production
	sint32 delta_t_sum;
	uint32 delta_amount;

	// production remainder when scaled to PRODUCTION_DELTA_T. added back next step to eliminate cumulative error
	uint32 delta_amount_remainder;

	// number of rounds where there is active production or consumption
	uint8 activity_count;

	// true if the factory has a transformer adjacent
	leitung_t* transformer_connected;

	// true, if the factory did produce enough in the last step to require power
	bool currently_producing;

	uint32 last_sound_ms;

	// power that can be currently drawn from this station (or the amount delivered)
	uint32 power;

	// power requested for next step
	uint32 power_demand;

	uint32 total_input, total_transit, total_output;
	uint8 status;

	uint8 sector = unknown;
	void set_sector();

	/// Position of a building of the factory.
	koord3d pos;

	/// Position of the nw-corner tile of the factory.
	koord3d pos_origin;

	/**
	 * Number of times the factory has expanded so far
	 * Only for factories without fields
	 */
	uint16 times_expanded;

	/**
	 * Electricity amount scaled with prodbase
	 */
	uint32 scaled_electric_demand;

	/**
	 * Pax/mail demand scaled with prodbase and month length
	 */
	uint32 scaled_pax_demand;
	uint32 scaled_mail_demand;

	/**
	 * Update scaled electricity amount
	 */
	void update_scaled_electric_demand();

	/**
	 * Update scaled pax/mail demand
	 */
public:
	void update_scaled_pax_demand(bool is_from_saved_game = false);
	void update_scaled_mail_demand(bool is_from_saved_game = false);
private:

	/**
	 * Update production multipliers for pax and mail
	 */
	void update_prodfactor_pax();
	void update_prodfactor_mail();

	/**
	 * Recalculate storage capacities based on prodbase or capacities contributed by fields
	 */
	void recalc_storage_capacities();

	/**
	 * Class for collecting arrival data and calculating pax/mail boost with fixed period length
	 */
	#define PERIOD_BITS   (18)              // determines period length on which boost calculation is based
	#define SLOT_BITS     (6)               // determines the number of time slots available
	#define SLOT_COUNT    (1<<SLOT_BITS)    // number of time slots for accumulating arrived pax/mail

	class arrival_statistics_t
	{
	private:
		uint16 slots[SLOT_COUNT];
		uint16 current_slot;
		uint16 active_slots;      // number of slots covered since aggregate arrival last increased from 0 to +ve
		uint32 aggregate_arrival;
		uint32 scaled_demand;
	public:
		void init();
		void rdwr(loadsave_t *file);
		void set_scaled_demand(const uint32 demand) { scaled_demand = demand; }
		#define ARRIVALS_CHANGED (1)
		#define ACTIVE_SLOTS_INCREASED (2)
		sint32 advance_slot();
		void book_arrival(const uint16 amount);
		uint16 get_active_slots() const { return active_slots; }
		uint32 get_aggregate_arrival() const { return aggregate_arrival; }
		uint32 get_scaled_demand() const { return scaled_demand; }
	};

	void update_transit_intern( const ware_t& ware, bool add );

	/**
	 * Arrival data for calculating pax/mail boost
	 */
	arrival_statistics_t arrival_stats_pax;
	arrival_statistics_t arrival_stats_mail;

	plainstring name;

	/**
	 * For advancement of slots for boost calculation
	 */
	sint32 delta_slot;

	void recalc_factory_status();

	// create some smoke on the map
	void smoke() const;

	// scales the amount of production based on the amount already in storage
	uint32 scale_output_production(const uint32 product, uint32 menge) const;

	// This is the city within whose city limits the factory is located.
	// NULL if it is outside a city. This is re-checked monthly.
	// @author: jamespetts
	stadt_t* city;

	// Check whether this factory is in a city: return NULL if not, or the city that it is in if so.
	stadt_t* check_local_city();

	bool has_calculated_intransit_percentages;

	void adjust_production_for_fields(bool is_from_saved_game = false);

protected:

	void delete_all_fields(bool destructor=false);

public:
	fabrik_t(loadsave_t *file);
	fabrik_t(koord3d pos, player_t* owner, const factory_desc_t* desc, sint32 initial_prod_base);
	~fabrik_t();

	gebaeude_t* get_building();

	/**
	 * Return/book statistics
	 */
	const sint64* get_stats() const { return *statistics; }
	sint64 get_stat(int month, int stat_type) const { assert(stat_type<MAX_FAB_STAT); return statistics[month][stat_type]; }
	void book_stat(sint64 value, int stat_type) { assert(stat_type<MAX_FAB_STAT); statistics[0][stat_type] += value; }


	static void update_transit( const ware_t& ware, bool add );

	/**
	 * convert internal units to displayed values
	 */
	sint64 get_stat_converted(int month, int stat_type) const {
		assert(stat_type<MAX_FAB_STAT);
		sint64 value = statistics[month][stat_type];
		switch(stat_type) {
			case FAB_POWER:
				value = convert_power(value);
				break;
			case FAB_BOOST_ELECTRIC:
			case FAB_BOOST_PAX:
			case FAB_BOOST_MAIL:
				value = convert_boost(value);
				break;
			default: ;
		}
		return value;
	}

	static fabrik_t * get_fab(const koord &pos);

	/**
	 * @return vehicle description object
	 */
	const factory_desc_t *get_desc() const {return desc; }

	void finish_rd();

	/**
	* gets position of a building belonging to factory
	*/
	koord3d get_pos() const { return pos; }

	void rotate90( const sint16 y_size );

	static const vector_tpl<koord> null_vector;

	ware_link_iteratable_t get_consumers() { return ware_link_iteratable_t(&output); } // "Delivery destinations" (Google)
	ware_link_const_iteratable_t get_consumers() const {return ware_link_const_iteratable_t(&output);}
	const vector_tpl<koord>& get_consumers(const goods_desc_t* ware_type) const {
		for(auto& ware : get_output()){
			if(ware.get_typ()==ware_type){
				return ware.get_links();
			}
		}
		return null_vector;
	}
	bool is_consumer_active_at(koord consumer_pos ) const;
	void set_consumer_active_at(koord consumer_pos );
	void reset_consumer_active();

	ware_link_iteratable_t get_suppliers() { return ware_link_iteratable_t(&input); }
	ware_link_const_iteratable_t get_suppliers() const {return ware_link_const_iteratable_t(&input);}
	const vector_tpl<koord>& get_suppliers(const goods_desc_t* ware_type) const {
		for(auto& ware : get_input()){
			if(ware.get_typ()==ware_type){
				return ware.get_links();
			}
		}
		return null_vector;
	}

	void init_contracts();
	void remove_contracts();

	void negotiate_contracts();

	const vector_tpl<nearby_halt_t>& get_nearby_freight_halts() const { return nearby_freight_halts; }

	/**
	 * Recalculate nearby halts
	 * These are stashed, so must be recalced
	 * when halts are built or destroyed
	 * @author neroden
	 */
	void recalc_nearby_halts();

	/**
	 * Re-mark nearby roads.
	 * Needs to be called by factory_builder_t (otherwise private).
	 */
	void mark_connected_roads(bool del);

	/**
	 * Functions for manipulating the list of connected cities
	 */
	/*void add_target_city(stadt_t *const city);
	void remove_target_city(stadt_t *const city);
	void clear_target_cities();
	const vector_tpl<stadt_t *>& get_target_cities() const { return target_cities; }*/

	/**
	 * Adds a new delivery goal
	 */
	void add_consumer(koord ziel, const goods_desc_t *desc=0, const sint64 contract=0);
	void remove_consumer(koord consumer_pos);

	bool disconnect_consumer(koord consumer_pos);
	bool disconnect_supplier(koord supplier_pos, fabrik_t *supplier=0);

	/**
	 * adds a supplier
	 */
	void  add_supplier(koord pos, const goods_desc_t* desc=0);
	void  remove_supplier(koord supplier_pos, fabrik_t *supplier=0);

	/**
	 * @return counts amount of ware of typ
	 *   -1 not produced/used here
	 *   0>= actual amount
	 */
	sint32 get_input_stock(const goods_desc_t *ware);
	sint32 get_output_stock(const goods_desc_t *ware);

	/**
	* returns all power and consume it to prevent multiple pumpes
	*/
	uint32 get_power() { uint32 p=power; power=0; return p; }

	/**
	* returns power wanted by the factory for next step and sets to 0 to prevent multiple senkes on same powernet
	*/
	uint32 step_power_demand() { uint32 p=power_demand; power_demand=0; return p; }
	uint32 get_power_demand() const { return power_demand; }

	/**
	*give power to the factory to consume ...
	*/
	void add_power(uint32 p) { power += p; }

	/**
	* senkes give back wanted power they can't supply such that a senke on a different powernet can try suppling
	* WARNING: senke stepping order can vary between ingame construction and savegame loading => different results after saving/loading the game
	*/
	void add_power_demand(uint32 p) { power_demand +=p; }

	/**
	* True if there was production requiring power in the last step
	*/
	bool is_currently_producing() const { return currently_producing; }

	/**
	* Used to limit transformers to one per factory
	*/
	bool is_transformer_connected() const { return (bool)transformer_connected || ((bool)city && !desc->is_electricity_producer()); }
	leitung_t* get_transformer_connected() { return transformer_connected; }
	void set_transformer_connected(leitung_t* connected) { transformer_connected = connected; }

	/**
	 * Return the number of goods needed
	 * in internal units
	 */

	sint32 goods_needed(const goods_desc_t *) const;

	sint32 liefere_an(const goods_desc_t *, sint32 menge);

	/*
	* This method is used when visiting passengers arrive at an
	* end consumer industry. This logs their consumption of
	* products sold at this end-consumer industry.
	*/
	void add_consuming_passengers(sint32 number_of_passengers);

	/*
	* Returns true if this industry has no stock left.
	* If the industry has some types of stock left but not
	* others, whether true or false is returned is random,
	* weighted by the proportions of each.
	* This is for use in determining whether passengers may
	* travel to this destination.
	*/
	bool out_of_stock_selective();

	void step(uint32 delta_t);                  // factory muss auch arbeiten ("factory must also work")
	void step_contracts(uint32 delta_t);

	void distribute_contracts(uint32 delta_t);

	void rescale_delta();
	void advance_slot(uint32 delta_t);

	void new_month();

	char const* get_name() const;
	void set_name( const char *name );

	PIXVAL get_color() const { return desc->get_color(); }

	player_t *get_owner() const
	{
		grund_t const* const p = welt->lookup(pos);
		return p ? p->first_obj()->get_owner() : 0;
	}

	void show_info();

	/**
	 * infostring on production
	 */
	void info_prod(cbuffer_t& buf) const;

	void rdwr(loadsave_t *file);

	/*
	 * Fills the vector with the koords of the tiles.
	 */
	void get_tile_list( vector_tpl<koord> &tile_list ) const;

	/// @returns a vector of factories within a rectangle
	static vector_tpl<fabrik_t *> & sind_da_welche(koord min, koord max);

	/**
	 * "gives true back if factory in the field is"
	 */
	//static bool ist_da_eine(karte_t *welt, koord min, koord max);
	//static bool check_construction_site(karte_t *welt, koord pos, koord size, bool water, climate_bits cl);

	// hier die methoden zum parametrisieren der Fabrik
	// "here the methods to parameterize the factory"

	/// Builds buildings (gebaeude_t) for the factory.
	void build(sint32 rotate, bool build_fields, bool force_initial_prodbase, bool from_saved = false);

	uint8 get_rotate() const { return rotate; }
	void set_rotate( uint8 r ) { rotate = r; }

	/* field generation code
	 * spawns a field for sure if probability>=1000
	 */
	bool add_random_field(uint16 probability);

	void remove_field_at(koord pos);

	uint32 get_field_count() const { return fields.get_count(); }

	/**
	 * total and current procduction/storage values
	 */
	const array_tpl<ware_production_t>& get_input() const { return input; }
	array_tpl<ware_production_t>& get_input() { return input; }
	const array_tpl<ware_production_t>& get_output() const { return output; }
	array_tpl<ware_production_t>& get_output() { return output; }

	ware_production_t* get_input(const goods_desc_t* ware){
		for(uint32 i = 0; i < input.get_count(); i++){
			if(input[i].get_typ()==ware){
				return &(input[i]);
			}
		}
		return NULL;
	}

	const ware_production_t* get_input(const goods_desc_t* ware) const{
		for(uint32 i = 0; i < input.get_count(); i++){
			if(input[i].get_typ()==ware){
				return &(input[i]);
			}
		}
		return NULL;
	}

	ware_production_t* get_output(const goods_desc_t* ware){
		for(uint32 i = 0; i < output.get_count(); i++){
			if(output[i].get_typ()==ware){
				return &(output[i]);
			}
		}
		return NULL;
	}

	/**
	 * Production multipliers
	 */
	sint32 get_prodfactor_electric() const { return prodfactor_electric; }
	sint32 get_prodfactor_pax() const { return get_sector() == fabrik_t::end_consumer ? 0 : prodfactor_pax; }
	sint32 get_prodfactor_mail() const { return get_sector() == fabrik_t::end_consumer ? 0 : prodfactor_mail; }
	sint32 get_prodfactor() const { return DEFAULT_PRODUCTION_FACTOR + prodfactor_electric + get_prodfactor_pax() + get_prodfactor_mail(); }

	/* does not takes month length into account */
	sint32 get_base_production() const { return prodbase; }
	void set_base_production(sint32 p, bool is_from_saved_game = false);

	// This is done this way rather than reusing get_prodfactor() because the latter causes a lack of precision (everything being rounded to the nearest 16).
	sint32 get_current_production() const { return (sint32)(welt->calc_adjusted_monthly_figure((sint64)prodbase * (sint64)get_prodfactor())) >> 8l; }

	// returns the current productivity relative to 100
	sint32 get_current_productivity() const { return welt->calc_adjusted_monthly_figure(prodbase) ? get_current_production() * 100 / welt->calc_adjusted_monthly_figure(prodbase) : 0; }
	// returns the current productivity including the effect of staff shortage
	sint32 get_actual_productivity() const { return status == inactive ? 0 : is_staff_shortage() ? get_current_productivity() * get_staffing_level_percentage() / 100 : get_current_productivity(); }

	//TODO fix rounding
	sint32 get_monthly_production(const sint32 pfactor=256) const {
		return (uint32)(get_current_production()*pfactor << (fabrik_t::precision_bits - DEFAULT_PRODUCTION_FACTOR_BITS));
	}

	/* returns the status of the current factory, as well as output */
	enum {
		nothing, good, water_resource, medium, water_resource_full, storage_full,
		inactive, shipment_stuck, material_shortage, no_material, bad,
		mat_overstocked, stuck, missing_connections, missing_consumer, material_not_available,
		MAX_FAB_STATUS
	};
	static uint8 status_to_color[MAX_FAB_STATUS];

	uint8  get_status() const { return status; }
	uint32 get_total_in() const { return total_input; }
	uint32 get_total_transit() const { return total_transit; }
	uint32 get_total_out() const { return total_output; }

	// return total storage occupancy for UI. should ignore the overflow of certain goods.
	uint32 get_total_input_capacity() const;
	uint32 get_total_output_capacity() const;

	/**
	 * Draws some nice colored bars giving some status information
	 */
	void display_status(sint16 xpos, sint16 ypos);

	/**
	 * Crossconnects all factories
	 */
	void add_all_suppliers();

	/* adds a new supplier to this factory
	 * fails if no matching goods are there
	 */
	bool add_supplier(fabrik_t* fab, const goods_desc_t* product=0);

	/* adds a new customer to this factory
	 * fails if no matching goods are accepted
	 */
	bool add_customer(fabrik_t* fab, const goods_desc_t* product=0);

	stadt_t* get_city() const { return city; }
	void clear_city() { city = NULL; }
	/**
	 * Return the scaled electricity amount and pax/mail demand
	 */
	uint32 get_scaled_electric_demand() const { return scaled_electric_demand; }
	uint32 get_scaled_pax_demand() const { return scaled_pax_demand; }
	uint32 get_monthly_pax_demand() const;
	uint32 get_scaled_mail_demand() const { return scaled_mail_demand; }

	//bool is_end_consumer() const { return (output.empty() && !desc->is_electricity_producer()); }
	enum ftype {
		marine_resource = 0,
		resource,
		resource_city,
		manufacturing,
		end_consumer,
		power_plant,
		unknown
	};

	// @returns industry type
	uint8 get_sector() const { return sector; }
	// Determine shortage of staff for each industry type
	bool is_staff_shortage() const;

	sint32 get_staffing_level_percentage() const;

	/**
	 * Returns a list of goods produced by this factory.
	 */
	slist_tpl<const goods_desc_t*> *get_produced_goods() const;

	void add_to_world_list();

	inline uint32 get_base_pax_demand() const { return arrival_stats_pax.get_scaled_demand(); }
	inline uint32 get_base_mail_demand() const { return arrival_stats_mail.get_scaled_demand(); }

	void calc_max_intransit_percentages();

	sint64 adjust_consumption_by_passenger_level(sint64 original_consumption);

	// Average journey time to delivery goods of this type
	uint32 get_lead_time (const goods_desc_t* wtype);

	int get_passenger_level_jobs() const;
	int get_passenger_level_visitors() const;
	int get_mail_level() const;

	bool is_input_empty() const;

	// check connected to public or current player stop
    bool is_connected_to_network(player_t *player) const;

	// Returns whether this factory has potential demand for passed goods category
	// 0=check input and output demand, 1=cehck only input, 2=check only output
	bool has_goods_catg_demand(uint8 catg_index = goods_manager_t::INDEX_NONE, uint8 check_option = 0) const;


	// Returns the operating rate to basic production. (x 10)
	uint32 calc_operation_rate(sint8 month) const;
};

#endif
