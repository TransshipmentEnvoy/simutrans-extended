/*
 * This file is part of the Simutrans-Extended project under the Artistic License.
 * (see LICENSE.txt)
 */

#include "trafficlight_info.h"
#include "components/gui_label.h"
#include "components/gui_colorbox.h"
#include "../obj/roadsign.h" // The rest of the dialog

#include "../simmenu.h"
#include "../simworld.h"

trafficlight_info_t::trafficlight_info_t(roadsign_t* s) :
	obj_infowin_t(s),
	roadsign(s)
{
	add_table(5,3)->set_alignment(ALIGN_CENTER_H);
	{
		new_component<gui_empty_t>();
		new_component<gui_colorbox_t>(15911)->set_size(scr_size(LINESPACE-2, LINESPACE-2));
		new_component<gui_colorbox_t>(65024)->set_size(scr_size(LINESPACE-2, LINESPACE-2));
		new_component<gui_margin_t>(LINESPACE);
		new_component<gui_label_t>("shift");

		new_component<gui_label_t>("east_and_west");
		ns.set_limits( 1, 255 );
		ns.set_value( s->get_ticks_ns() );
		ns.wrap_mode( false );
		ns.add_listener( this );
		add_component( &ns );

		amber_ns.set_limits(1, 255);
		amber_ns.set_value(s->get_ticks_amber_ns());
		amber_ns.wrap_mode(false);
		amber_ns.add_listener(this);
		add_component(&amber_ns);

		new_component<gui_empty_t>();

		offset.set_limits( 0, 255 );
		offset.set_value( s->get_ticks_offset() );
		offset.wrap_mode( false );
		offset.add_listener( this );
		add_component( &offset );


		new_component<gui_label_t>("north_and_south");
		ow.set_limits( 1, 255 );
		ow.set_value( s->get_ticks_ow() );
		ow.wrap_mode( false );
		ow.add_listener( this );
		add_component( &ow );

		amber_ow.set_limits( 1, 255 );
		amber_ow.set_value( s->get_ticks_amber_ow() );
		amber_ow.wrap_mode( false );
		amber_ow.add_listener( this );
		add_component( &amber_ow );
	}
	end_table();

	// show author below the settings
	if (char const* const maker = roadsign->get_desc()->get_copyright()) {
		gui_label_buf_t* lb = new_component<gui_label_buf_t>();
		lb->buf().printf(translator::translate("Constructed by %s"), maker);
		lb->update();
	}

	recalc_size();
}


/**
 * This method is called if an action is triggered
 *
 * Returns true, if action is done and no more
 * components should be triggered.
 */
bool trafficlight_info_t::action_triggered( gui_action_creator_t *comp, value_t v)
{
	char param[256];
	if(comp == &ns) {
		sprintf( param, "%s,1,%i", roadsign->get_pos().get_str(), (int)v.i );
		tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT]->set_default_param( param );
		welt->set_tool( tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT], welt->get_active_player() );
	}
	else if(comp == &ow) {
		sprintf( param, "%s,0,%i", roadsign->get_pos().get_str(), (int)v.i );
		tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT]->set_default_param( param );
		welt->set_tool( tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT], welt->get_active_player() );
	}
 	else if(comp == &offset) {
		sprintf( param, "%s,2,%i", roadsign->get_pos().get_str(), (int)v.i );
		tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT]->set_default_param( param );
		welt->set_tool( tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT], welt->get_active_player() );
	}
 	else if(comp == &amber_ns) {
		sprintf( param, "%s,4,%i", roadsign->get_pos().get_str(), (int)v.i );
		tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT]->set_default_param( param );
		welt->set_tool( tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT], welt->get_active_player() );
	}
 	else if(comp == &amber_ow) {
		sprintf( param, "%s,3,%i", roadsign->get_pos().get_str(), (int)v.i );
		tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT]->set_default_param( param );
		welt->set_tool( tool_t::simple_tool[TOOL_CHANGE_TRAFFIC_LIGHT], welt->get_active_player() );
	}
	return true;
}


// notify for an external update
void trafficlight_info_t::update_data()
{
	ns.set_value( roadsign->get_ticks_ns() );
	ow.set_value( roadsign->get_ticks_ow() );
	offset.set_value( roadsign->get_ticks_offset() );
	amber_ns.set_value( roadsign->get_ticks_amber_ns() );
	amber_ow.set_value( roadsign->get_ticks_amber_ow() );
}
