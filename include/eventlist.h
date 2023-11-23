#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <SDL2/SDL.h>

struct event_list_s;
typedef struct event_list_s event_list_t;

event_list_t *event_list_create();
int event_list_destroy(event_list_t *event_list);
int event_list_reset(event_list_t *event_list);
int event_list_append(event_list_t *event_list, SDL_Event *event);
int event_list_length(event_list_t *event_list);
int event_list_get(event_list_t *event_list, int index, SDL_Event *event);
int event_list_poll(event_list_t *event_list);

#endif
