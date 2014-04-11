#include <pebble.h>

/* Header stuff */
static void options_menu_select_offset_callback(int, void *);
static void options_menu_select_schedule_callback(int, void *);
static void update_options_menu_offset_subtitle();
static void update_options_menu_schedule_subtitle();
static void click_config_provider(void *); 
static uint16_t timetable_get_num_rows_callback(MenuLayer *, uint16_t, void *);
static void timetable_draw_row_callback(GContext *, const Layer *, MenuIndex *, void *);
static void main_timetable_offset_changed(ScrollLayer *, void *);

typedef struct {
  char *name; //Name of activity
  uint32_t duration; //duration in seconds
} __attribute__((__packed__)) ClassTimeEntry;

typedef struct {
  char *name; //Name of schedule
  int num_entries; //Number of entries
  ClassTimeEntry entries[20]; //Entries for that type
} __attribute__((__packed__)) ScheduleEntry;


ScheduleEntry schedules[] = {
  { .name = "Half Day (Late)", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 8*60*60 }, //8:00
    { .name = "Mods 1-2", .duration = 11*60 }, // + 0:11
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 24*60}, // + 0:24
    { .name = "End of Day", .duration = 12*60*60+5*60} // + 12:05
  }},
  { .name = "Half Day", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 7*60*60 + 45*60}, //7:45
    { .name = "Mods 1-2", .duration = 26*60 }, // + 0:26
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 24*60}, // + 0:24
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 24*60}, // + 0:24
    { .name = "End of Day", .duration = 12*60*60+5*60} // + 12:05
  }},
  { .name = "Full Day (Late)", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 8*60*60 + 20*60 }, //8:20
    { .name = "Mods 1-2", .duration = 11*60 }, // + 0:11
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 40*60}, // + 0:40
    { .name = "End of Day", .duration = 9*60*60+37*60} // + 9:37
  }},
  { .name = "Full Day", .num_entries = 20, .entries = {
    { .name = "Start of Day", .duration = 7*60*60 + 40*60 }, //7:40
    { .name = "Passing Time", .duration = 5*60 }, // + 0:05
    { .name = "Mods 1-2", .duration = 46*60 }, // + 0:46
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 40*60}, // + 0:40
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 40*60}, // + 0:40
    { .name = "End of Day", .duration = 9*60*60+37*60} // + 9:37
  }},
  { .name = "Delayed Opening (Late)", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 10*60*60 }, //10:00
    { .name = "Mods 1-2", .duration = 16*60 }, // + 0:16
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 27*60}, // + 0:27
    { .name = "End of Day", .duration = 9*60*60+37*60} // + 9:37
  }},
  { .name = "Delayed Opening", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 9*60*60 + 45*60 }, //9:45
    { .name = "Mods 1-2", .duration = 31*60 }, // + 0:31
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 27*60}, // + 0:27
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 27*60}, // + 0:27
    { .name = "End of Day", .duration = 9*60*60+37*60} // + 9:37
  }},
  { .name = "HSPA", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 10*60*60 + 45*60 }, //10:45
    { .name = "Mods 1-2", .duration = 28*60 }, // + 0:28
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 20*60}, // + 0:20
    { .name = "End of Day", .duration = 9*60*60 + 35*60} // + 24:00
  }},
  { .name = "HSPA (Late)", .num_entries = 19, .entries = {
    { .name = "Start of Day", .duration = 11*60*60 }, //11:00
    { .name = "Mods 1-2", .duration = 13*60 }, // + 0:13
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 3-4", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 5-6", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 7-8", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 9-10", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 11-12", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 13-14", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 15-16", .duration = 20*60}, // + 0:20
    { .name = "Passing Time", .duration = 4*60}, // + 0:04
    { .name = "Mods 17-18", .duration = 20*60}, // + 0:20
    { .name = "End of Day", .duration = 9*60*60 + 35*60} // + 24:00
  }},
  { .name = "RUPC", .num_entries = 11, .entries = {
    { .name = "Start of Day", .duration = 8*60*60 }, //8:00
    { .name = "Sign-in", .duration = 45*60 }, // + 0:45
    { .name = "Welcome", .duration = 30*60}, // + 0:30
    { .name = "Return to Labs", .duration = 15*60}, // + 0:15
    { .name = "Programming", .duration = 2*60*60 + 30*60}, // + 2:30
    { .name = "Submission", .duration = 15*60}, // + 0:15
    { .name = "Lunch", .duration = 60*60}, // + 1:00
    { .name = "Planetarium", .duration = 45*60}, // + 0:45
    { .name = "Return", .duration = 45*60}, // + 0:45
    { .name = "Presentation", .duration = 15*60}, // + 0:15
    { .name = "End of Day", .duration = 9*60*60}, // + 9:00
  }}
};


#define NUM_SCHEDULES sizeof(schedules) / sizeof(ScheduleEntry)


//Persistance keys
#define NUM_OFFSET_PKEY 0x3214
#define NUM_SCHED_PKEY 0x3215

//Default values if the persistance values don't exist
#define NUM_OFFSET_DEFAULT 0
#define NUM_SCHED_DEFAULT 0

static int num_offset = NUM_OFFSET_DEFAULT;
static int num_sched = NUM_SCHED_DEFAULT;

//Windows
static Window *window;
static Window *options_window;
static NumberWindow *offset_window;
static SimpleMenuLayer *options_menu_layer;

//Options menu items
static SimpleMenuItem options_menu_items[] = {
  { .title = "Offset", .subtitle = "-1 seconds", .callback = options_menu_select_offset_callback },
  { .title = "Schedule", .subtitle = "Half Day", .callback = options_menu_select_schedule_callback }
};
static SimpleMenuSection options_menu_sections[] = {
  { .num_items = 2, .items = options_menu_items }
};

//Layers for the main screen
static ScrollLayer *main_scroll_layer;
static MenuLayer *main_timetable_menu;

static Layer *event_layer;
static InverterLayer *event_layer_inverter;
static TextLayer *event_layer_name;
static TextLayer *event_layer_elapsed;
static TextLayer *event_layer_separator;
static TextLayer *event_layer_remaining;

static Layer *time_layer;
static TextLayer *time_layer_ampm;
static TextLayer *time_layer_hoursmins;
static TextLayer *time_layer_seconds;

static Layer *date_layer;
static InverterLayer *date_layer_inverter;
static TextLayer *date_layer_year;
static TextLayer *date_layer_day_name;
static TextLayer *date_layer_month_day;

static ScheduleEntry current_schedule;
static ClassTimeEntry current_entry;

static int current_index = -1;
static int previous_times = 0;

//Find the current entry in the schedule
static int findcurrent(int seconds) {
  int current = 0;
  seconds -= current_schedule.entries[current].duration;
  while(seconds >= 0 && current < current_schedule.num_entries) {
    current++;
    seconds -= current_schedule.entries[current].duration;
  }
  return current;
}

//Loop through the entries to get the time elapsed before the index
//index is exclusive
static int getprevioustimes(int index) {
  int t = 0;

  for(int i = 0; i < index && i < current_schedule.num_entries; i++) {
    t += current_schedule.entries[i].duration;
  }

  return t;
}

static void update_timetable_selected() {
    menu_layer_set_selected_index(main_timetable_menu, (MenuIndex) { .row = current_index, .section = 0 }, MenuRowAlignCenter, false);
}

static void handle_second_tick(struct tm *tick_time_arg, TimeUnits units_changes) {

  struct tm tick_time;
  memcpy(&tick_time, tick_time_arg, sizeof(*tick_time_arg));

  int secs = tick_time.tm_sec + (tick_time.tm_min * 60) + (tick_time.tm_hour * 60 * 60) - num_offset;

  //Get the current event on new days
  //Will be triggered on app focus also
  if((units_changes & DAY_UNIT) == DAY_UNIT) {
    current_index = findcurrent(secs);
    current_entry = current_schedule.entries[current_index];
    previous_times = getprevioustimes(current_index);
    update_timetable_selected();
  }

  int remaining_diff = previous_times + current_entry.duration - secs;

  int elapsed_diff = secs - previous_times;

  text_layer_set_text(event_layer_name, current_entry.name);

  static char event_elapsed[] = "00:00:00";
  snprintf(event_elapsed, sizeof(event_elapsed), "%02d:%02d:%02d", elapsed_diff / 3600, (elapsed_diff % 3600) / 60, elapsed_diff % 60);
  text_layer_set_text(event_layer_elapsed, event_elapsed);

  static char event_remaining[] = "-00:00:00";
  snprintf(event_remaining, sizeof(event_remaining), "-%02d:%02d:%02d", remaining_diff / 3600, (remaining_diff % 3600) / 60, remaining_diff % 60);
  text_layer_set_text(event_layer_remaining, event_remaining);


  //Update event when the time remaining is 0
  if(remaining_diff == 0) {
    previous_times += current_entry.duration;
    current_index++;
    if(current_index >= current_schedule.num_entries) current_index = 0;
    current_entry = current_schedule.entries[current_index];
    update_timetable_selected();
  }

  if((units_changes & MINUTE_UNIT) == MINUTE_UNIT) {
    static char time_hoursmins[] = "00:00";
    strftime(time_hoursmins, sizeof(time_hoursmins), clock_is_24h_style() ? "%R" : "%I:%M", &tick_time);

    if (!clock_is_24h_style() && (time_hoursmins[0] == '0')) {
      memmove(time_hoursmins, &time_hoursmins[1], sizeof(time_hoursmins) - 1);
    }

    text_layer_set_text(time_layer_hoursmins, time_hoursmins);

    if((units_changes & HOUR_UNIT) == HOUR_UNIT && !clock_is_24h_style()) {
      static char time_ampm[] = "AA";
      strftime(time_ampm, sizeof(time_ampm), "%p", &tick_time);
      text_layer_set_text(time_layer_ampm, time_ampm);
    }
    
  }


  if((units_changes & YEAR_UNIT) == YEAR_UNIT) {
    static char date_year[] = "0000";
    strftime(date_year, sizeof(date_year), "%Y", &tick_time);
    text_layer_set_text(date_layer_year, date_year);
  }
  if((units_changes & DAY_UNIT) == DAY_UNIT) {
    static char date_day_name[] = "AAA";
    strftime(date_day_name, sizeof(date_day_name), "%a", &tick_time);
    text_layer_set_text(date_layer_day_name, date_day_name);
  }
  if((units_changes & MONTH_UNIT) == MONTH_UNIT || (units_changes & DAY_UNIT) == DAY_UNIT) {
    static char date_month_day[] = "00-00";
    strftime(date_month_day, sizeof(date_month_day), "%m-%d", &tick_time);
  text_layer_set_text(date_layer_month_day, date_month_day);

  }

  static char time_seconds[] = "00";
  strftime(time_seconds, sizeof(time_seconds), "%S", &tick_time);
  text_layer_set_text(time_layer_seconds, time_seconds);

}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //define the segment size (height / 3)
  int16_t segment = 42;
  
  main_scroll_layer = scroll_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, bounds.size.h } });
  
  scroll_layer_set_callbacks(main_scroll_layer, (ScrollLayerCallbacks) {
    .click_config_provider = click_config_provider,
    .content_offset_changed_handler = main_timetable_offset_changed
  });
  scroll_layer_set_click_config_onto_window(main_scroll_layer, window);
  scroll_layer_set_content_size(main_scroll_layer, GSize(bounds.size.w, 2000));
  scroll_layer_set_shadow_hidden(main_scroll_layer, true);

  //holds everything for the top
  event_layer = layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, segment } });

  /*
    Split into four sections
        Event Name
    Elapsed / Remaining

    Ex:
           Day Start
      04:54:23 / 01:03:37

  */
  
  event_layer_name = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, segment / 2 } });
  text_layer_set_text_alignment(event_layer_name, GTextAlignmentCenter);
  text_layer_set_background_color(event_layer_name, GColorClear);
  layer_add_child(event_layer, text_layer_get_layer(event_layer_name));

  event_layer_elapsed = text_layer_create((GRect) { .origin = { 0, segment / 2 }, .size = { bounds.size.w / 2 - 15, segment / 2 } });
  text_layer_set_text_alignment(event_layer_elapsed, GTextAlignmentRight);
  text_layer_set_background_color(event_layer_elapsed, GColorClear);
  layer_add_child(event_layer, text_layer_get_layer(event_layer_elapsed));

  event_layer_separator = text_layer_create((GRect) { .origin = { bounds.size.w / 2 - 15, segment / 2 }, .size = { 30, segment / 2 } });
  text_layer_set_text(event_layer_separator, " / ");
  text_layer_set_text_alignment(event_layer_separator, GTextAlignmentCenter);
  text_layer_set_background_color(event_layer_separator, GColorClear);
  layer_add_child(event_layer, text_layer_get_layer(event_layer_separator));

  event_layer_remaining = text_layer_create((GRect) { .origin = { bounds.size.w / 2 + 10, segment / 2 }, .size = { bounds.size.w / 2, segment / 2 } });
  text_layer_set_text_alignment(event_layer_remaining, GTextAlignmentLeft);
  text_layer_set_background_color(event_layer_remaining, GColorClear);
  layer_add_child(event_layer, text_layer_get_layer(event_layer_remaining));

  //have an inverter invert the elements
  event_layer_inverter = inverter_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, segment } });
  layer_add_child(event_layer, inverter_layer_get_layer(event_layer_inverter));


  scroll_layer_add_child(main_scroll_layer, event_layer);


  //hold everything for the middle
  time_layer = layer_create((GRect) { .origin = { 0, segment }, .size = { bounds.size.w, segment * 2 } });

  /*
    Split into three sections:
          AM/PM
      Time
          Seconds

    Ex:
          AM
      10:04
          53
  */

  time_layer_ampm = text_layer_create((GRect) { .origin = { bounds.size.w - 30, 15 }, .size = { 50, 50 } });
  text_layer_set_text_alignment(time_layer_ampm, GTextAlignmentLeft);
  text_layer_set_background_color(time_layer_ampm, GColorClear);
  layer_add_child(time_layer, text_layer_get_layer(time_layer_ampm));

  time_layer_hoursmins = text_layer_create((GRect) { .origin = { 0, 7 }, .size = { bounds.size.w - 35, segment * 2 } });
  text_layer_set_font(time_layer_hoursmins, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_FONT_48)));
  text_layer_set_text_alignment(time_layer_hoursmins, GTextAlignmentRight);
  text_layer_set_background_color(time_layer_hoursmins, GColorClear);
  layer_add_child(time_layer, text_layer_get_layer(time_layer_hoursmins));

  time_layer_seconds = text_layer_create((GRect) { .origin = { bounds.size.w - 30, segment }, .size = { bounds.size.w / 4, segment } });
  text_layer_set_font(time_layer_seconds, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(time_layer_seconds, GTextAlignmentLeft);
  text_layer_set_background_color(time_layer_seconds, GColorClear);
  layer_add_child(time_layer, text_layer_get_layer(time_layer_seconds));

  //hide the am/pm symbol if it's in 24 hour mode
  if(clock_is_24h_style()) {
    layer_set_hidden(text_layer_get_layer(time_layer_ampm), true);
  }

  scroll_layer_add_child(main_scroll_layer, time_layer);

  //hold everything for the bottom
  date_layer = layer_create((GRect) { .origin = { 0, segment * 3 }, .size = { bounds.size.w, segment } });
  /* 
     Split into three sections:
     | Year | Day | Month / Day

     Ex:
     2014 | Tue | 01-13

  */

  date_layer_year = text_layer_create((GRect) { .origin = { 0, 2 }, .size = { bounds.size.w / 3, segment } });
  text_layer_set_font(date_layer_year, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(date_layer_year, GTextAlignmentRight);
  text_layer_set_background_color(date_layer_year, GColorClear);
  layer_add_child(date_layer, text_layer_get_layer(date_layer_year));

  date_layer_day_name = text_layer_create((GRect) { .origin = { bounds.size.w / 3, 2 }, .size = { bounds.size.w / 3, segment } });
  text_layer_set_font(date_layer_day_name, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(date_layer_day_name, GTextAlignmentCenter);
  text_layer_set_background_color(date_layer_day_name, GColorClear);
  layer_add_child(date_layer, text_layer_get_layer(date_layer_day_name));

  date_layer_month_day = text_layer_create((GRect) { .origin = { 2 * bounds.size.w / 3, 2 }, .size = { bounds.size.w / 3, segment } });
  text_layer_set_font(date_layer_month_day, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(date_layer_month_day, GTextAlignmentLeft);
  text_layer_set_background_color(date_layer_month_day, GColorClear);
  layer_add_child(date_layer, text_layer_get_layer(date_layer_month_day));


  //invert these too
  date_layer_inverter = inverter_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, segment } });
  layer_add_child(date_layer, inverter_layer_get_layer(date_layer_inverter));

  scroll_layer_add_child(main_scroll_layer, date_layer);

  main_timetable_menu = menu_layer_create((GRect) { .origin = { 0, segment * 4 }, .size = { bounds.size.w, 1500 } });
  menu_layer_set_callbacks(main_timetable_menu, NULL, (MenuLayerCallbacks) {
    .get_num_rows = timetable_get_num_rows_callback,
    .draw_row = timetable_draw_row_callback
  });
  
  scroll_layer_add_child(main_scroll_layer, menu_layer_get_layer(main_timetable_menu));
  
  layer_add_child(window_layer, scroll_layer_get_layer(main_scroll_layer));


}

static void window_appear(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  int content_length = 45 * current_schedule.num_entries + bounds.size.h;
  scroll_layer_set_content_size(main_scroll_layer, (GSize) { .w = bounds.size.w, .h = content_length });

  //make a time struct to init the times without delay
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);

  //update every text layer on the first load
  handle_second_tick(t, SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT | MONTH_UNIT | YEAR_UNIT);
}

static uint16_t timetable_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return current_schedule.num_entries;
}
static void timetable_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    static char subt[] = "00:00:00 -> 00:00:00";
    int prev = getprevioustimes(cell_index->row);
    int next = prev + current_schedule.entries[cell_index->row].duration;
    snprintf(subt, sizeof(subt), "%02d:%02d:%02d -> %02d:%02d:%02d", prev / 3600, (prev % 3600) / 60, prev % 60, next / 3600, (next % 3600) / 60, next % 60);
    menu_cell_basic_draw(ctx, cell_layer, current_schedule.entries[cell_index->row].name, subt, NULL);
}

static void select_long_click_handler(ClickRecognizerRef recongnizer, void *context) {
  vibes_short_pulse();

  //Show the options menu
  window_stack_push(options_window, true /* Animated */);
}

static void click_config_provider(void *context) {
  const uint16_t long_click_delay = 1000;
  window_long_click_subscribe(BUTTON_ID_SELECT, long_click_delay, select_long_click_handler, NULL);
}

static void main_timetable_offset_changed(ScrollLayer *scroll_layer, void *ctx) {
  scroll_layer_set_shadow_hidden(scroll_layer, scroll_layer_get_content_offset(scroll_layer).y == 0);
}

static void window_unload(Window *window) {
  text_layer_destroy(event_layer_name);
  text_layer_destroy(event_layer_elapsed);
  text_layer_destroy(event_layer_separator);
  text_layer_destroy(event_layer_remaining);
  layer_destroy(event_layer);

  text_layer_destroy(time_layer_ampm);
  text_layer_destroy(time_layer_hoursmins);
  text_layer_destroy(time_layer_seconds);
  layer_destroy(time_layer);

  text_layer_destroy(date_layer_year);
  text_layer_destroy(date_layer_day_name);
  text_layer_destroy(date_layer_month_day);
  layer_destroy(date_layer);
  
  menu_layer_destroy(main_timetable_menu);
  scroll_layer_destroy(main_scroll_layer);
}

static void options_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(options_window);
  GRect bounds = layer_get_frame(window_layer);

  options_menu_layer = simple_menu_layer_create(bounds, options_window, options_menu_sections, 1, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(options_menu_layer));

  update_options_menu_offset_subtitle();
  update_options_menu_schedule_subtitle();
}

static void update_options_menu_offset_subtitle() {
  char *subtitle_pattern = num_offset == 1 ? "%d second " : "%d seconds";
  static char stp[] = "           ";
  snprintf(stp, sizeof(stp), subtitle_pattern, num_offset);
  options_menu_items[0].subtitle = stp;
  layer_mark_dirty(simple_menu_layer_get_layer(options_menu_layer));
}

static void update_options_menu_schedule_subtitle() {
  options_menu_items[1].subtitle = current_schedule.name;
  layer_mark_dirty(simple_menu_layer_get_layer(options_menu_layer));
}

static void options_menu_select_offset_callback(int index, void *ctx) {
  vibes_short_pulse();
  window_stack_push((Window*)offset_window, true /* Animated */);
}

static void options_menu_select_schedule_callback(int index, void *ctx) {
  vibes_short_pulse();
  num_sched++;
  if(num_sched >= (int)(NUM_SCHEDULES)) num_sched = 0;
  persist_write_int(NUM_SCHED_PKEY, num_sched);
  current_schedule = schedules[num_sched];
  update_options_menu_schedule_subtitle();
}


static void options_window_unload(Window *window) {
  simple_menu_layer_destroy(options_menu_layer);
}

static void offset_window_selected(struct NumberWindow *number_window, void *context) {
  num_offset = number_window_get_value(number_window);
  persist_write_int(NUM_OFFSET_PKEY, num_offset);
  update_options_menu_offset_subtitle();
  update_options_menu_schedule_subtitle();
  window_stack_pop(true /* Animated */);
}


static void init(void) {

  //get the values from the persist service
  num_offset = persist_exists(NUM_OFFSET_PKEY) ? persist_read_int(NUM_OFFSET_PKEY) : NUM_OFFSET_DEFAULT;
  num_sched = persist_exists(NUM_SCHED_PKEY) ? persist_read_int(NUM_SCHED_PKEY) : NUM_SCHED_DEFAULT;

  current_schedule = schedules[num_sched];

  //Create the main window
  window = window_create();

  //window_set_click_config_provider(window, click_config_provider);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear
  });

  window_set_fullscreen(window, true);
  window_stack_push(window, true /* Animated */);


  //Create the options window
  options_window = window_create();
  window_set_window_handlers(options_window, (WindowHandlers) {
    .load = options_window_load,
    .unload = options_window_unload
  });

  //Create the offset number window
  offset_window = number_window_create("Offset", (NumberWindowCallbacks) {
    .decremented = NULL,
    .incremented = NULL,
    .selected = offset_window_selected
  }, NULL);
  number_window_set_min(offset_window, -99); //-99 is used because it's 3 characters
  number_window_set_max(offset_window, 999);
  number_window_set_value(offset_window, num_offset);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {

  //store the values on exit also
  persist_write_int(NUM_OFFSET_PKEY, num_offset);
  persist_write_int(NUM_SCHED_PKEY, num_sched);

  window_destroy(window);
  window_destroy(options_window);
  number_window_destroy(offset_window);


  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
