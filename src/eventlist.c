#include "eventlist.h"

struct event_list_s {
	size_t length;
	size_t capacity;
	SDL_Event *events;
};

event_list_t *event_list_create()
{
	event_list_t *event_list = malloc(sizeof(event_list_t));
	if (event_list == NULL)
		return NULL;
	event_list->length = 0;
	event_list->capacity = 4;
	event_list->events = calloc(sizeof(SDL_Event), event_list->capacity);
	if (event_list->events == NULL) {
		free(event_list);
		return NULL;
	}
	return event_list;
}

int event_list_destroy(event_list_t *event_list)
{
	free(event_list->events);
	free(event_list);
	return 0;
}

int event_list_reset(event_list_t *event_list)
{
	event_list->length = 0;
	return 0;
}

int event_list_append(event_list_t *event_list, SDL_Event *event)
{
	if (event_list->length == event_list->capacity) {
		event_list->capacity *= 2;
		event_list->events =
			realloc(event_list->events,
				sizeof(SDL_Event) * event_list->capacity);
	}
	event_list->events[event_list->length] = *event;
	event_list->length++;
	return 0;
}

int event_list_length(event_list_t *event_list)
{
	return event_list->length;
}

int event_list_get(event_list_t *event_list, int index, SDL_Event *event)
{
	if (index < 0 || index >= (int)event_list->length)
		return 1;
	*event = event_list->events[index];
	return 0;
}

int event_list_poll(event_list_t *event_list)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		event_list_append(event_list, &event);
		if (event.type == SDL_QUIT)
			return 1;
	}
	return 0;
}