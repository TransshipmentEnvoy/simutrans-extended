target_sources(simutrans-extended PRIVATE
	bauer/brueckenbauer.cc
	bauer/fabrikbauer.cc
	bauer/goods_manager.cc
	bauer/hausbauer.cc
	bauer/pier_builder.cc
	bauer/tunnelbauer.cc
	bauer/tree_builder.cc
	bauer/vehikelbauer.cc
	bauer/wegbauer.cc
	boden/boden.cc
	boden/brueckenboden.cc
	boden/fundament.cc
	boden/grund.cc
	boden/monorailboden.cc
	boden/pier_deck.cc
	boden/tunnelboden.cc
	boden/wasser.cc
	boden/wege/kanal.cc
	boden/wege/maglev.cc
	boden/wege/monorail.cc
	boden/wege/narrowgauge.cc
	boden/wege/runway.cc
	boden/wege/schiene.cc
	boden/wege/strasse.cc
	boden/wege/weg.cc
	convoy.cc
	dataobj/consist_order_t.cc
	dataobj/crossing_logic.cc
	dataobj/environment.cc
	dataobj/freelist.cc
	dataobj/gameinfo.cc
	dataobj/height_map_loader.cc
	dataobj/koord3d.cc
	dataobj/koord.cc
	dataobj/livery_scheme.cc
	dataobj/loadsave.cc
	dataobj/marker.cc
	dataobj/objlist.cc
	dataobj/powernet.cc
	dataobj/rect.cc
	dataobj/replace_data.cc
	dataobj/ribi.cc
	dataobj/route.cc
	dataobj/scenario.cc
	dataobj/schedule.cc
	dataobj/settings.cc
	dataobj/tabfile.cc
	dataobj/translator.cc
	descriptor/bridge_desc.cc
	descriptor/building_desc.cc
	descriptor/factory_desc.cc
	descriptor/goods_desc.cc
	descriptor/ground_desc.cc
	descriptor/image.cc
	descriptor/obj_base_desc.cc
	descriptor/pier_desc.cc
	descriptor/reader/bridge_reader.cc
	descriptor/reader/building_reader.cc
	descriptor/reader/citycar_reader.cc
	descriptor/reader/crossing_reader.cc
	descriptor/reader/factory_reader.cc
	descriptor/reader/good_reader.cc
	descriptor/reader/groundobj_reader.cc
	descriptor/reader/ground_reader.cc
	descriptor/reader/imagelist2d_reader.cc
	descriptor/reader/imagelist3d_reader.cc
	descriptor/reader/imagelist_reader.cc
	descriptor/reader/image_reader.cc
	descriptor/reader/obj_reader.cc
	descriptor/reader/pedestrian_reader.cc
	descriptor/reader/pier_reader.cc
	descriptor/reader/roadsign_reader.cc
	descriptor/reader/root_reader.cc
	descriptor/reader/sim_reader.cc
	descriptor/reader/skin_reader.cc
	descriptor/reader/sound_reader.cc
	descriptor/reader/text_reader.cc
	descriptor/reader/tree_reader.cc
	descriptor/reader/tunnel_reader.cc
	descriptor/reader/vehicle_reader.cc
	descriptor/reader/way_obj_reader.cc
	descriptor/reader/way_reader.cc
	descriptor/reader/xref_reader.cc
	descriptor/sound_desc.cc
	descriptor/tunnel_desc.cc
	descriptor/vehicle_desc.cc
	descriptor/way_desc.cc
	display/font.cc
	display/simview.cc
	display/viewport.cc
	finder/placefinder.cc
	freight_list_sorter.cc
	gui/ai_option_t.cc
	gui/banner.cc
	gui/base_info.cc
	gui/baum_edit.cc
	gui/building_info.cc
	gui/citybuilding_edit.cc
	gui/city_info.cc
	gui/citylist_frame_t.cc
	gui/citylist_stats_t.cc
	gui/climates.cc
	gui/components/gui_aligned_container.cc
	gui/components/gui_building.cc
	gui/components/gui_button.cc
	gui/components/gui_button_to_chart.cc
	gui/components/gui_chart.cc
	gui/components/gui_colorbox.cc
	gui/components/gui_combobox.cc
	gui/components/gui_component.cc
	gui/components/gui_container.cc
	gui/components/gui_convoiinfo.cc
	gui/components/gui_convoy_assembler.cc
	gui/components/gui_convoy_formation.cc
	gui/components/gui_convoy_label.cc
	gui/components/gui_convoy_payloadinfo.cc
	gui/components/gui_divider.cc
	gui/components/gui_factory_storage_info.cc
	gui/components/gui_fixedwidth_textarea.cc
	gui/components/gui_flowtext.cc
	gui/components/gui_halthandled_lines.cc
	gui/components/gui_image.cc
	gui/components/gui_image_list.cc
	gui/components/gui_label.cc
	gui/components/gui_line_lettercode.cc
	gui/components/gui_map_preview.cc
	gui/components/gui_numberinput.cc
	gui/components/gui_obj_view_t.cc
	gui/components/gui_schedule_item.cc
	gui/components/gui_scrollbar.cc
	gui/components/gui_scrolled_list.cc
	gui/components/gui_scrollpane.cc
	gui/components/gui_speedbar.cc
	gui/components/gui_table.cc
	gui/components/gui_tab_panel.cc
	gui/components/gui_textarea.cc
	gui/components/gui_textinput.cc
	gui/components/gui_vehicle_capacitybar.cc
	gui/components/gui_vehicle_cargoinfo.cc
	gui/components/gui_waytype_tab_panel.cc
	gui/components/gui_world_view_t.cc
	gui/convoi_detail_t.cc
	gui/convoi_filter_frame.cc
	gui/convoi_frame.cc
	gui/convoi_info_t.cc
	gui/convoy_item.cc
	gui/curiosity_edit.cc
	gui/curiositylist_frame_t.cc
	gui/curiositylist_stats_t.cc
	gui/depot_frame.cc
	gui/depotlist_frame.cc
	gui/display_settings.cc
	gui/enlarge_map_frame_t.cc
	gui/extend_edit.cc
	gui/fabrik_info.cc
	gui/factory_chart.cc
	gui/factory_edit.cc
	gui/factory_legend.cc
	gui/factorylist_frame_t.cc
	gui/factorylist_stats_t.cc
	gui/goods_frame_t.cc
	gui/goods_stats_t.cc
	gui/ground_info.cc
	gui/groundobj_edit.cc
	gui/gui_frame.cc
	gui/gui_theme.cc
	gui/halt_detail.cc
	gui/halt_info.cc
	gui/halt_list_filter_frame.cc
	gui/halt_list_frame.cc
	gui/halt_list_stats.cc
	gui/help_frame.cc
	gui/headquarter_info.cc
	gui/jump_frame.cc
	gui/kennfarbe.cc
	gui/label_info.cc
	gui/labellist_frame_t.cc
	gui/labellist_stats_t.cc
	gui/line_color_gui.cc
	gui/line_item.cc
	gui/line_management_gui.cc
	gui/loadfont_frame.cc
	gui/load_relief_frame.cc
	gui/loadsave_frame.cc
	gui/map_frame.cc
	gui/messagebox.cc
	gui/message_frame_t.cc
	gui/message_option_t.cc
	gui/message_stats_t.cc
	gui/minimap.cc
	gui/money_frame.cc
	gui/obj_info.cc
	gui/onewaysign_info.cc
	gui/optionen.cc
	gui/overtaking_mode.cc
	gui/pakselector.cc
	gui/password_frame.cc
	gui/pier_rotation_select.cc
	gui/player_frame_t.cc
	gui/privatesign_info.cc
	gui/replace_frame.cc
	gui/savegame_frame.cc
	gui/scenario_frame.cc
	gui/scenario_info.cc
	gui/schedule_gui.cc
	gui/schedule_list.cc
	gui/server_frame.cc
	gui/settings_frame.cc
	gui/settings_stats.cc
	gui/signal_connector_gui.cc
	gui/signal_info.cc
	gui/signal_spacing.cc
	gui/signalboxlist_frame.cc
	gui/simwin.cc
	gui/slim_obj_info.cc
	gui/sound_frame.cc
	gui/sprachen.cc
	gui/station_building_select.cc
	gui/themeselector.cc
	gui/times_history_container.cc
	gui/tool_selector.cc
	gui/trafficlight_info.cc
	gui/vehiclelist_frame.cc
	gui/vehicle_class_manager.cc
	gui/water_info.cc
	gui/way_info.cc
	gui/welt.cc
	io/raw_image.cc
	io/raw_image_bmp.cc
	io/raw_image_png.cc
	io/raw_image_ppm.cc
	io/rdwr/adler32_stream.cc
	io/rdwr/bzip2_file_rdwr_stream.cc
	io/rdwr/compare_file_rd_stream.cc
	io/rdwr/raw_file_rdwr_stream.cc
	io/rdwr/rdwr_stream.cc
	io/rdwr/zlib_file_rdwr_stream.cc
	io/classify_file.cc
	network/checksum.cc
	network/memory_rw.cc
	network/network_address.cc
	network/network.cc
	network/network_cmd.cc
	network/network_cmd_ingame.cc
	network/network_cmd_scenario.cc
	network/network_cmp_pakset.cc
	network/network_file_transfer.cc
	network/network_packet.cc
	network/network_socket_list.cc
	network/pakset_info.cc
	network/pwd_hash.cc
	obj/baum.cc
	obj/bruecke.cc
	obj/crossing.cc
	obj/field.cc
	obj/gebaeude.cc
	obj/groundobj.cc
	obj/label.cc
	obj/leitung2.cc
	obj/pier.cc
	obj/pillar.cc
	obj/roadsign.cc
	obj/signal.cc
	obj/simobj.cc
	obj/tunnel.cc
	obj/wayobj.cc
	obj/wolke.cc
	obj/zeiger.cc
	old_blockmanager.cc
	path_explorer.cc
	player/ai.cc
	player/ai_goods.cc
	player/ai_passenger.cc
	player/finance.cc
	player/simplay.cc
	script/api/api_city.cc
	script/api/api_const.cc
	script/api/api_control.cc
	script/api/api_convoy.cc
	script/api/api_factory.cc
	script/api/api_gui.cc
	script/api/api_halt.cc
	script/api/api_include.cc
	script/api/api_line.cc
	script/api/api_map_objects.cc
	script/api/api_obj_desc_base.cc
	script/api/api_obj_desc.cc
	script/api/api_player.cc
	script/api/api_scenario.cc
	script/api/api_schedule.cc
	script/api/api_settings.cc
	script/api/api_simple.cc
	script/api/api_tiles.cc
	script/api/api_world.cc
	script/api_class.cc
	script/api/export_desc.cc
	script/api_function.cc
	script/api/get_next.cc
	script/api_param.cc
	script/dynamic_string.cc
	script/export_objs.cc
	script/script.cc
	simcity.cc
	simconvoi.cc
	simdebug.cc
	simdepot.cc
	simevent.cc
	simfab.cc
	simhalt.cc
	siminteraction.cc
	simintr.cc
	simio.cc
	simline.cc
	simlinemgmt.cc
	simloadingscreen.cc
	simmain.cc
	simmem.cc
	simmenu.cc
	simmesg.cc
	simplan.cc
	simsignalbox.cc
	simskin.cc
	simsound.cc
	sys/simsys.cc
	simticker.cc
	simtool.cc
	simunits.cc
	simware.cc
	simworld.cc
	squirrel/sq_extensions.cc
	squirrel/sqstdlib/sqstdaux.cc
	squirrel/sqstdlib/sqstdblob.cc
	squirrel/sqstdlib/sqstdio.cc
	squirrel/sqstdlib/sqstdmath.cc
	squirrel/sqstdlib/sqstdrex.cc
	squirrel/sqstdlib/sqstdstream.cc
	squirrel/sqstdlib/sqstdstring.cc
	squirrel/sqstdlib/sqstdsystem.cc
	squirrel/squirrel/sqapi.cc
	squirrel/squirrel/sqbaselib.cc
	squirrel/squirrel/sqclass.cc
	squirrel/squirrel/sqcompiler.cc
	squirrel/squirrel/sqdebug.cc
	squirrel/squirrel/sqfuncstate.cc
	squirrel/squirrel/sqlexer.cc
	squirrel/squirrel/sqmem.cc
	squirrel/squirrel/sqobject.cc
	squirrel/squirrel/sqstate.cc
	squirrel/squirrel/sqtable.cc
	squirrel/squirrel/sqvm.cc
	unicode.cc
	utils/cbuffer_t.cc
	utils/checklist.cc
	utils/csv.cc
	utils/float32e8_t.cc
	utils/log.cc
	utils/searchfolder.cc
	utils/sha1.cc
	utils/simrandom.cc
	utils/simstring.cc
	utils/simthread.cc
	vehicle/movingobj.cc
	vehicle/pedestrian.cc
	vehicle/simroadtraffic.cc
	vehicle/vehicle.cc
	vehicle/air_vehicle.cc
	vehicle/rail_vehicle.cc
	vehicle/road_vehicle.cc
	vehicle/water_vehicle.cc
)
