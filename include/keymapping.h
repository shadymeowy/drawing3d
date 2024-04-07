#ifndef KEYMAPPING_H
#define KEYMAPPING_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef uint16_t key_action_t;
// x, y, z, rx, ry, rz, distance
#define KEY_ACTION_LEN 6 + 6 + 2
#define KEY_ACTION_X_INC (1 << 0)
#define KEY_ACTION_X_DEC (1 << 1)
#define KEY_ACTION_Y_INC (1 << 2)
#define KEY_ACTION_Y_DEC (1 << 3)
#define KEY_ACTION_Z_INC (1 << 4)
#define KEY_ACTION_Z_DEC (1 << 5)
#define KEY_ACTION_RX_INC (1 << 6)
#define KEY_ACTION_RX_DEC (1 << 7)
#define KEY_ACTION_RY_INC (1 << 8)
#define KEY_ACTION_RY_DEC (1 << 9)
#define KEY_ACTION_RZ_INC (1 << 10)
#define KEY_ACTION_RZ_DEC (1 << 11)
#define KEY_ACTION_DISTANCE_INC (1 << 12)
#define KEY_ACTION_DISTANCE_DEC (1 << 13)

typedef struct {
	SDL_Keycode key;
	key_action_t action;
} keymapping_t;

static keymapping_t default_spherical_keymap[] __attribute__((unused)) = {
	{ SDL_SCANCODE_LEFT, KEY_ACTION_RZ_INC },
	{ SDL_SCANCODE_RIGHT, KEY_ACTION_RZ_DEC },
	{ SDL_SCANCODE_UP, KEY_ACTION_RY_DEC },
	{ SDL_SCANCODE_DOWN, KEY_ACTION_RY_INC },
	{ SDL_SCANCODE_W, KEY_ACTION_DISTANCE_DEC },
	{ SDL_SCANCODE_S, KEY_ACTION_DISTANCE_INC },
	/* { SDL_SCANCODE_A, KEY_ACTION_Y_DEC },
	{ SDL_SCANCODE_D, KEY_ACTION_Y_INC },
	{ SDL_SCANCODE_Q, KEY_ACTION_X_DEC },
	{ SDL_SCANCODE_E, KEY_ACTION_X_INC },
	{ SDL_SCANCODE_SPACE, KEY_ACTION_Z_INC},
	{ SDL_SCANCODE_LSHIFT, KEY_ACTION_Z_DEC}, */
	{ 0, 0 },
};

static keymapping_t default_keymap[] __attribute__((unused)) = {
	{ SDL_SCANCODE_LEFT, KEY_ACTION_RZ_DEC },
	{ SDL_SCANCODE_RIGHT, KEY_ACTION_RZ_INC },
	{ SDL_SCANCODE_UP, KEY_ACTION_RY_DEC },
	{ SDL_SCANCODE_DOWN, KEY_ACTION_RY_INC },
	{ SDL_SCANCODE_W, KEY_ACTION_X_INC },
	{ SDL_SCANCODE_S, KEY_ACTION_X_DEC },
	{ SDL_SCANCODE_A, KEY_ACTION_Y_DEC },
	{ SDL_SCANCODE_D, KEY_ACTION_Y_INC },
	{ SDL_SCANCODE_Q, KEY_ACTION_Z_DEC },
	{ SDL_SCANCODE_E, KEY_ACTION_Z_INC },
	{ SDL_SCANCODE_SPACE, KEY_ACTION_Z_INC },
	{ SDL_SCANCODE_LSHIFT, KEY_ACTION_Z_DEC },
	{ 0, 0 },
};

#endif