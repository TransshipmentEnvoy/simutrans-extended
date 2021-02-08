/*
 * This file is part of the Simutrans-Extended project under the Artistic License.
 * (see LICENSE.txt)
 */

#include "../dataobj/schedule.h"
#include "../dataobj/loadsave.h"
#include "minimap.h"
#include "../simline.h"
#include "simwin.h"
#include "../simtool.h"
#include "../utils/cbuffer_t.h"

#include "line_management_gui.h"

line_management_gui_t::line_management_gui_t(linehandle_t line, player_t* player_) :
	schedule_gui_t()
{
	this->line = line;
	if (line.is_bound() ) {
		schedule_gui_t::init(line->get_schedule()->copy(), player_, convoihandle_t() );

		title.printf("%s - %s", translator::translate("Fahrplan"), line->get_name());
		gui_frame_t::set_name(title);
		// has this line a single running convoi?
		if(  line->count_convoys() > 0  ) {
			minimap_t::get_instance()->set_selected_cnv(line->get_convoy(0));
			img_electric.set_visible(line->needs_electrification());
			min_range = min(line->get_min_range() ? line->get_min_range() : UINT16_MAX, min_range);
			if (min_range>0 && min_range<UINT16_MAX) {
				lb_min_range.buf().printf("%u km", min_range);
				lb_min_range.update();
			}
			img_refuel.set_visible(min_range>0 && min_range<UINT16_MAX);
			lb_min_range.set_visible(min_range>0 && min_range<UINT16_MAX);
			schedule_gui_t::set_min_range(min_range);
		}
	}
}


line_management_gui_t::~line_management_gui_t()
{
	delete old_schedule; // since we pass a *copy* of the line's schedule to the base class
	old_schedule = NULL;
}


bool line_management_gui_t::infowin_event(const event_t *ev)
{
	if(  player!=NULL  ) {
		// not "magic_line_schedule_rdwr_dummy" during loading of UI ...
		if(  !line.is_bound()  ) {
			destroy_win( this );
		}
		else  {
			if(  schedule_gui_t::infowin_event(ev)  ) {
				return true;
			}
			if(  ev->ev_class == INFOWIN  &&  ev->ev_code == WIN_CLOSE  ) {
				if (!schedule->matches(welt, line->get_schedule()))
				{
					FOR(minivec_tpl<schedule_entry_t>, &i, schedule->entries)
					{
						// It is necessary to re-set the reversing status here, as
						// the reversing status of one stop may well change depending on
						// what other stops are next to it in the schedule.
						i.reverse = -1;
					}

					// update all convoys of this line!
					// update line schedule via tool!
					tool_t *tool = create_tool( TOOL_CHANGE_LINE | SIMPLE_TOOL );
					cbuffer_t buf;
					buf.printf( "g,%i,", line.get_id() );
					schedule->sprintf_schedule( buf );
					tool->set_default_param(buf);
					welt->set_tool( tool, line->get_owner() );
					// since init always returns false, it is save to delete immediately
					delete tool;
				}
			}
		}
	}
	return false;
}



void line_management_gui_t::rdwr(loadsave_t *file)
{
	scr_size size = get_windowsize();
	size.rdwr( file );
	simline_t::rdwr_linehandle_t(file, line);
	// player that edits
	uint8 player_nr;
	if (file->is_saving()) {
		player_nr = player->get_player_nr();
	}
	file->rdwr_byte( player_nr );
	player = welt->get_player(player_nr);

	// save edited schedule
	if(  file->is_loading()  ) {
		// dummy types
		old_schedule = new truck_schedule_t();
		schedule = new truck_schedule_t();
	}
	schedule->rdwr(file);
	old_schedule->rdwr(file);

	if(  file->is_loading()  ) {
		player_t *player = welt->get_player(player_nr);
		assert(player); // since it was alive during saving, this should never happen
		if(  line.is_bound()  &&  old_schedule->matches( welt, line->get_schedule() )  ) {

			delete old_schedule;
			old_schedule = NULL;

			schedule_t *save_schedule = schedule->copy();

			init(line->get_schedule()->copy(), line->get_owner(), convoihandle_t() );
			title.printf("%s - %s", translator::translate("Fahrplan"), line->get_name());
			gui_frame_t::set_name(title);
			// init replaced schedule, restore
			schedule->copy_from(save_schedule);
			delete save_schedule;

			if(  line->count_convoys() > 0  ) {
				minimap_t::get_instance()->set_selected_cnv(line->get_convoy(0));
				img_electric.set_visible(line->needs_electrification());
				min_range = min(line->get_min_range() ? line->get_min_range() : UINT16_MAX, min_range);
				if (min_range>0 && min_range<UINT16_MAX) {
					lb_min_range.buf().printf("%u km", min_range);
					lb_min_range.update();
				}
				img_refuel.set_visible(min_range>0 && min_range<UINT16_MAX);
				lb_min_range.set_visible(min_range>0 && min_range<UINT16_MAX);
				schedule_gui_t::set_min_range(min_range);
			}

			set_windowsize(size);

			win_set_magic(this, (ptrdiff_t)line.get_rep());
		}
		else {
			line = linehandle_t();
			player = NULL; // prevent destructor from updating
			destroy_win( this );
			dbg->error( "line_management_gui_t::rdwr", "Could not restore schedule window for line id %i", line.get_id() );
		}
	}
}
