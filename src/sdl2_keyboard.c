#include "sdl2_keyboard.h"
#include "sdl2_video.h"
#include "sdl2_rect.h"
#include "mruby/value.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include <SDL2/SDL_keycode.h>

static struct RClass *mod_Keyboard = NULL;
static struct RClass *class_Keysym = NULL;

typedef struct mrb_sdl2_keyboard_keysym_data_t {
  SDL_Keysym keysym;
} mrb_sdl2_keyboard_keysym_data_t;

static void
mrb_sdl2_keyboard_keysym_data_free(mrb_state *mrb, void *p)
{
  mrb_free(mrb, p);
}

static struct mrb_data_type const mrb_sdl2_keyboard_keysym_data_type = {
  "Keysym", mrb_sdl2_keyboard_keysym_data_free
};

SDL_Keysym *
mrb_sdl2_keyboard_keysym_get_ptr(mrb_state *mrb, mrb_value value)
{
  if (mrb_nil_p(value)) {
    return NULL;
  }
  return &((mrb_sdl2_keyboard_keysym_data_t*)mrb_data_get_ptr(mrb, value, &mrb_sdl2_keyboard_keysym_data_type))->keysym;
}

mrb_value
mrb_sdl2_keyboard_keysym(mrb_state *mrb, SDL_Keysym const *keysym)
{
  mrb_sdl2_keyboard_keysym_data_t *data =
    (mrb_sdl2_keyboard_keysym_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_keyboard_keysym_data_t));
  if (NULL == data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->keysym = *keysym;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Keysym, &mrb_sdl2_keyboard_keysym_data_type, data));
}

/***************************************************************************
*
* module SDL2::Input
*
***************************************************************************/

static mrb_value
mrb_sdl2_keyboard_key_from_name(mrb_state *mrb, mrb_value mod)
{
  mrb_value name;
  mrb_get_args(mrb, "S", &name);
  return mrb_fixnum_value(SDL_GetKeyFromName(RSTRING_PTR(name)));
}

static mrb_value
mrb_sdl2_keyboard_key_from_scancode(mrb_state *mrb, mrb_value mod)
{
  mrb_int code;
  mrb_get_args(mrb, "i", &code);
  return mrb_fixnum_value(SDL_GetKeyFromScancode((SDL_Scancode)code));
}

static mrb_value
mrb_sdl2_keyboard_key_name(mrb_state *mrb, mrb_value mod)
{
  mrb_int key;
  mrb_get_args(mrb, "i", &key);
  return mrb_str_new_cstr(mrb, SDL_GetKeyName((SDL_Keycode)key));
}

static mrb_value
mrb_sdl2_keyboard_keyboard_focus(mrb_state *mrb, mrb_value mod)
{
  return mrb_sdl2_video_associated_window(mrb, SDL_GetKeyboardFocus());
}

static mrb_value
mrb_sdl2_keyboard_get_mod_state(mrb_state *mrb, mrb_value mod)
{
  return mrb_fixnum_value(SDL_GetModState());
}

static mrb_value
mrb_sdl2_keyboard_set_mod_state(mrb_state *mrb, mrb_value mod)
{
  mrb_int kmod;
  mrb_get_args(mrb, "i", &kmod);
  SDL_SetModState((SDL_Keymod)kmod);
  return mod;
}

static mrb_value
mrb_sdl2_keyboard_scancode_from_key(mrb_state *mrb, mrb_value mod)
{
  mrb_int key;
  mrb_get_args(mrb, "i", &key);
  return mrb_fixnum_value(SDL_GetScancodeFromKey((SDL_Keycode)key));
}

static mrb_value
mrb_sdl2_keyboard_scancode_from_name(mrb_state *mrb, mrb_value mod)
{
  mrb_value name;
  mrb_get_args(mrb, "S", &name);
  return mrb_fixnum_value(SDL_GetScancodeFromName(RSTRING_PTR(name)));
}

static mrb_value
mrb_sdl2_keyboard_scancode_name(mrb_state *mrb, mrb_value mod)
{
  mrb_int code;
  mrb_get_args(mrb, "i", &code);
  return mrb_str_new_cstr(mrb, SDL_GetScancodeName((SDL_Scancode)code));
}

static mrb_value
mrb_sdl2_keyboard_text_input_rect(mrb_state *mrb, mrb_value mod)
{
  SDL_Rect * r;
  SDL_Rect tmp;
  mrb_value rect;
  mrb_get_args(mrb, "o", &rect);
  r = mrb_sdl2_rect_get_ptr(mrb, rect);
  if (NULL == r) {
    return mrb_nil_value();
  }
  tmp = *r;
  SDL_SetTextInputRect(&tmp);
  return mod;
}

static mrb_value
mrb_sdl2_keyboard_text_input_start(mrb_state *mrb, mrb_value mod)
{
  SDL_StartTextInput();
  return mod;
}

static mrb_value
mrb_sdl2_keyboard_text_input_stop(mrb_state *mrb, mrb_value mod)
{
  SDL_StopTextInput();
  return mod;
}

/***************************************************************************
*
* class SDL2::Input::Keysym
*
***************************************************************************/

static mrb_value
mrb_sdl2_keyboard_keysym_get_scancode(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_keyboard_keysym_data_t *data =
    (mrb_sdl2_keyboard_keysym_data_t*)mrb_sdl2_keyboard_keysym_get_ptr(mrb, self);
  return mrb_fixnum_value(data->keysym.scancode);
}

static mrb_value
mrb_sdl2_keyboard_keysym_get_symbol(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_keyboard_keysym_data_t *data =
    (mrb_sdl2_keyboard_keysym_data_t*)mrb_sdl2_keyboard_keysym_get_ptr(mrb, self);
  return mrb_fixnum_value(data->keysym.sym);
}

static mrb_value
mrb_sdl2_keyboard_keysym_get_modifier(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_keyboard_keysym_data_t *data =
    (mrb_sdl2_keyboard_keysym_data_t*)mrb_sdl2_keyboard_keysym_get_ptr(mrb, self);
  return mrb_fixnum_value(data->keysym.mod);
}


void
mruby_sdl2_keyboard_init(mrb_state *mrb)
{
  int arena_size;
  struct RClass *mod_Input = mrb_module_get_under(mrb, mod_SDL2, "Input");

  mod_Keyboard = mrb_define_module_under(mrb, mod_Input, "Keyboard");

  mrb_define_module_function(mrb, mod_Keyboard, "key_from_name",      mrb_sdl2_keyboard_key_from_name,      MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Keyboard, "key_from_scancode",  mrb_sdl2_keyboard_key_from_scancode,  MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Keyboard, "key_name",           mrb_sdl2_keyboard_key_name,           MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Keyboard, "keyboard_focus",     mrb_sdl2_keyboard_keyboard_focus,     MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "mod_state",          mrb_sdl2_keyboard_get_mod_state,      MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "mod_state=",         mrb_sdl2_keyboard_set_mod_state,      MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Keyboard, "scancode_from_key",  mrb_sdl2_keyboard_scancode_from_key,  MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "scancode_from_name", mrb_sdl2_keyboard_scancode_from_name, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "scancode_name",      mrb_sdl2_keyboard_scancode_name,      MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "text_input_rect",    mrb_sdl2_keyboard_text_input_rect,    MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Keyboard, "text_input_start",   mrb_sdl2_keyboard_text_input_start,   MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Keyboard, "text_input_stop",    mrb_sdl2_keyboard_text_input_stop,    MRB_ARGS_NONE());

  class_Keysym = mrb_define_class_under(mrb, mod_Keyboard, "Keysym", mrb->object_class);

  MRB_SET_INSTANCE_TT(class_Keysym, MRB_TT_DATA);

  mrb_define_method(mrb, class_Keysym, "scancode", mrb_sdl2_keyboard_keysym_get_scancode, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Keysym, "symbol",   mrb_sdl2_keyboard_keysym_get_symbol,   MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Keysym, "modifier", mrb_sdl2_keyboard_keysym_get_modifier, MRB_ARGS_NONE());

  arena_size = mrb_gc_arena_save(mrb);

  /* SDL_Scancode */
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_0", mrb_fixnum_value(SDL_SCANCODE_0));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_1", mrb_fixnum_value(SDL_SCANCODE_1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_2", mrb_fixnum_value(SDL_SCANCODE_2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_3", mrb_fixnum_value(SDL_SCANCODE_3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_4", mrb_fixnum_value(SDL_SCANCODE_4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_5", mrb_fixnum_value(SDL_SCANCODE_5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_6", mrb_fixnum_value(SDL_SCANCODE_6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_7", mrb_fixnum_value(SDL_SCANCODE_7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_8", mrb_fixnum_value(SDL_SCANCODE_8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_9", mrb_fixnum_value(SDL_SCANCODE_9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_A", mrb_fixnum_value(SDL_SCANCODE_A));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_B", mrb_fixnum_value(SDL_SCANCODE_B));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_C", mrb_fixnum_value(SDL_SCANCODE_C));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_D", mrb_fixnum_value(SDL_SCANCODE_D));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_E", mrb_fixnum_value(SDL_SCANCODE_E));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F", mrb_fixnum_value(SDL_SCANCODE_F));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_G", mrb_fixnum_value(SDL_SCANCODE_G));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_H", mrb_fixnum_value(SDL_SCANCODE_H));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_I", mrb_fixnum_value(SDL_SCANCODE_I));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_J", mrb_fixnum_value(SDL_SCANCODE_J));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_K", mrb_fixnum_value(SDL_SCANCODE_K));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_L", mrb_fixnum_value(SDL_SCANCODE_L));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_M", mrb_fixnum_value(SDL_SCANCODE_M));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_N", mrb_fixnum_value(SDL_SCANCODE_N));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_O", mrb_fixnum_value(SDL_SCANCODE_O));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_P", mrb_fixnum_value(SDL_SCANCODE_P));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Q", mrb_fixnum_value(SDL_SCANCODE_Q));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_R", mrb_fixnum_value(SDL_SCANCODE_R));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_S", mrb_fixnum_value(SDL_SCANCODE_S));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_T", mrb_fixnum_value(SDL_SCANCODE_T));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_U", mrb_fixnum_value(SDL_SCANCODE_U));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_V", mrb_fixnum_value(SDL_SCANCODE_V));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_W", mrb_fixnum_value(SDL_SCANCODE_W));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_X", mrb_fixnum_value(SDL_SCANCODE_X));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Y", mrb_fixnum_value(SDL_SCANCODE_Y));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Z", mrb_fixnum_value(SDL_SCANCODE_Z));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F1", mrb_fixnum_value(SDL_SCANCODE_F1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F2", mrb_fixnum_value(SDL_SCANCODE_F2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F3", mrb_fixnum_value(SDL_SCANCODE_F3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F4", mrb_fixnum_value(SDL_SCANCODE_F4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F5", mrb_fixnum_value(SDL_SCANCODE_F5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F6", mrb_fixnum_value(SDL_SCANCODE_F6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F7", mrb_fixnum_value(SDL_SCANCODE_F7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F8", mrb_fixnum_value(SDL_SCANCODE_F8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F9", mrb_fixnum_value(SDL_SCANCODE_F9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F10", mrb_fixnum_value(SDL_SCANCODE_F10));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F11", mrb_fixnum_value(SDL_SCANCODE_F11));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F12", mrb_fixnum_value(SDL_SCANCODE_F12));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F13", mrb_fixnum_value(SDL_SCANCODE_F13));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F14", mrb_fixnum_value(SDL_SCANCODE_F14));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F15", mrb_fixnum_value(SDL_SCANCODE_F15));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F16", mrb_fixnum_value(SDL_SCANCODE_F16));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F17", mrb_fixnum_value(SDL_SCANCODE_F17));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F18", mrb_fixnum_value(SDL_SCANCODE_F18));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F19", mrb_fixnum_value(SDL_SCANCODE_F19));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F20", mrb_fixnum_value(SDL_SCANCODE_F20));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F21", mrb_fixnum_value(SDL_SCANCODE_F21));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F22", mrb_fixnum_value(SDL_SCANCODE_F22));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F23", mrb_fixnum_value(SDL_SCANCODE_F23));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F24", mrb_fixnum_value(SDL_SCANCODE_F24));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_BACKSPACE",   mrb_fixnum_value(SDL_SCANCODE_BACKSPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CAPSLOCK",    mrb_fixnum_value(SDL_SCANCODE_CAPSLOCK));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_DELETE",      mrb_fixnum_value(SDL_SCANCODE_DELETE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_ESCAPE",      mrb_fixnum_value(SDL_SCANCODE_ESCAPE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_END",         mrb_fixnum_value(SDL_SCANCODE_END));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_HOME",        mrb_fixnum_value(SDL_SCANCODE_HOME));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INSERT",      mrb_fixnum_value(SDL_SCANCODE_INSERT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LALT",        mrb_fixnum_value(SDL_SCANCODE_LALT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LCTRL",       mrb_fixnum_value(SDL_SCANCODE_LCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LEFT",        mrb_fixnum_value(SDL_SCANCODE_LEFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LSHIFT",      mrb_fixnum_value(SDL_SCANCODE_LSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PAGEDOWN",    mrb_fixnum_value(SDL_SCANCODE_PAGEDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PAGEUP",      mrb_fixnum_value(SDL_SCANCODE_PAGEUP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PRINTSCREEN", mrb_fixnum_value(SDL_SCANCODE_PRINTSCREEN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RALT",        mrb_fixnum_value(SDL_SCANCODE_RALT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RCTRL",       mrb_fixnum_value(SDL_SCANCODE_RCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RETURN",      mrb_fixnum_value(SDL_SCANCODE_RETURN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RIGHT",       mrb_fixnum_value(SDL_SCANCODE_RIGHT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RSHIFT",      mrb_fixnum_value(SDL_SCANCODE_RSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_UNKNOWN",     mrb_fixnum_value(SDL_SCANCODE_UNKNOWN));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  /* SDL_Keycode */
  mrb_define_const(mrb, mod_Keyboard, "SDLK_0", mrb_fixnum_value(SDLK_0));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_1", mrb_fixnum_value(SDLK_1));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_2", mrb_fixnum_value(SDLK_2));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_3", mrb_fixnum_value(SDLK_3));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_4", mrb_fixnum_value(SDLK_4));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_5", mrb_fixnum_value(SDLK_5));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_6", mrb_fixnum_value(SDLK_6));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_7", mrb_fixnum_value(SDLK_7));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_8", mrb_fixnum_value(SDLK_8));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_9", mrb_fixnum_value(SDLK_9));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_A", mrb_fixnum_value(SDLK_a));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_B", mrb_fixnum_value(SDLK_b));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_C", mrb_fixnum_value(SDLK_c));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_D", mrb_fixnum_value(SDLK_d));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_E", mrb_fixnum_value(SDLK_e));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F", mrb_fixnum_value(SDLK_f));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_G", mrb_fixnum_value(SDLK_g));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_H", mrb_fixnum_value(SDLK_h));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_I", mrb_fixnum_value(SDLK_i));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_J", mrb_fixnum_value(SDLK_j));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_K", mrb_fixnum_value(SDLK_k));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_L", mrb_fixnum_value(SDLK_l));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_M", mrb_fixnum_value(SDLK_m));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_N", mrb_fixnum_value(SDLK_n));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_O", mrb_fixnum_value(SDLK_o));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_P", mrb_fixnum_value(SDLK_p));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_Q", mrb_fixnum_value(SDLK_q));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_R", mrb_fixnum_value(SDLK_r));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_S", mrb_fixnum_value(SDLK_s));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_T", mrb_fixnum_value(SDLK_t));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_U", mrb_fixnum_value(SDLK_u));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_V", mrb_fixnum_value(SDLK_v));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_W", mrb_fixnum_value(SDLK_w));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_X", mrb_fixnum_value(SDLK_x));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_Y", mrb_fixnum_value(SDLK_y));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_Z", mrb_fixnum_value(SDLK_z));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  mrb_define_const(mrb, mod_Keyboard, "SDLK_F1", mrb_fixnum_value(SDLK_F1));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F2", mrb_fixnum_value(SDLK_F2));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F3", mrb_fixnum_value(SDLK_F3));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F4", mrb_fixnum_value(SDLK_F4));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F5", mrb_fixnum_value(SDLK_F5));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F6", mrb_fixnum_value(SDLK_F6));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F7", mrb_fixnum_value(SDLK_F7));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F8", mrb_fixnum_value(SDLK_F8));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F9", mrb_fixnum_value(SDLK_F9));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F10", mrb_fixnum_value(SDLK_F10));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F11", mrb_fixnum_value(SDLK_F11));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F12", mrb_fixnum_value(SDLK_F12));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F13", mrb_fixnum_value(SDLK_F13));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F14", mrb_fixnum_value(SDLK_F14));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F15", mrb_fixnum_value(SDLK_F15));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F16", mrb_fixnum_value(SDLK_F16));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F17", mrb_fixnum_value(SDLK_F17));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F18", mrb_fixnum_value(SDLK_F18));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F19", mrb_fixnum_value(SDLK_F19));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F20", mrb_fixnum_value(SDLK_F20));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F21", mrb_fixnum_value(SDLK_F21));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F22", mrb_fixnum_value(SDLK_F22));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F23", mrb_fixnum_value(SDLK_F23));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_F24", mrb_fixnum_value(SDLK_F24));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  mrb_define_const(mrb, mod_Keyboard, "SDLK_BACKSPACE",   mrb_fixnum_value(SDLK_BACKSPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_CAPSLOCK",    mrb_fixnum_value(SDLK_CAPSLOCK));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_DELETE",      mrb_fixnum_value(SDLK_DELETE));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_DOWN",        mrb_fixnum_value(SDLK_DOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_ESCAPE",      mrb_fixnum_value(SDLK_ESCAPE));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_END",         mrb_fixnum_value(SDLK_END));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_HOME",        mrb_fixnum_value(SDLK_HOME));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_INSERT",      mrb_fixnum_value(SDLK_INSERT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LALT",        mrb_fixnum_value(SDLK_LALT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LCTRL",       mrb_fixnum_value(SDLK_LCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LEFT",        mrb_fixnum_value(SDLK_LEFT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LSHIFT",      mrb_fixnum_value(SDLK_LSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_PAGEDOWN",    mrb_fixnum_value(SDLK_PAGEDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_PAGEUP",      mrb_fixnum_value(SDLK_PAGEUP));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_PRINTSCREEN", mrb_fixnum_value(SDLK_PRINTSCREEN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RALT",        mrb_fixnum_value(SDLK_RALT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RCTRL",       mrb_fixnum_value(SDLK_RCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RETURN",      mrb_fixnum_value(SDLK_RETURN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RIGHT",       mrb_fixnum_value(SDLK_RIGHT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RSHIFT",      mrb_fixnum_value(SDLK_RSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_AMPERSAND",   mrb_fixnum_value(SDLK_AMPERSAND));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_ASTERISK",    mrb_fixnum_value(SDLK_ASTERISK));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_AT",          mrb_fixnum_value(SDLK_AT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_CARET",       mrb_fixnum_value(SDLK_CARET));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_COLON",       mrb_fixnum_value(SDLK_COLON));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_DOLLAR",      mrb_fixnum_value(SDLK_DOLLAR));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_EXCLAIM",     mrb_fixnum_value(SDLK_EXCLAIM));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_GREATER",     mrb_fixnum_value(SDLK_GREATER));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_HASH",        mrb_fixnum_value(SDLK_HASH));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LEFTPAREN",   mrb_fixnum_value(SDLK_LEFTPAREN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_LESS",        mrb_fixnum_value(SDLK_LESS));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_PERCENT",     mrb_fixnum_value(SDLK_PERCENT));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_PLUS",        mrb_fixnum_value(SDLK_PLUS));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_QUESTION",    mrb_fixnum_value(SDLK_QUESTION));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_QUOTEDBL",    mrb_fixnum_value(SDLK_QUOTEDBL));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_RIGHTPAREN",  mrb_fixnum_value(SDLK_RIGHTPAREN));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_UP",          mrb_fixnum_value(SDLK_UP));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_UNDERSCORE",  mrb_fixnum_value(SDLK_UNDERSCORE));
  mrb_define_const(mrb, mod_Keyboard, "SDLK_UNKNOWN",     mrb_fixnum_value(SDLK_UNKNOWN));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  /* SDL_Keymod */
  mrb_define_const(mrb, mod_Keyboard, "KMOD_NONE",   mrb_fixnum_value(KMOD_NONE));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_LSHIFT", mrb_fixnum_value(KMOD_LSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_RSHIFT", mrb_fixnum_value(KMOD_RSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_LCTRL",  mrb_fixnum_value(KMOD_LCTRL));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_RCTRL",  mrb_fixnum_value(KMOD_RCTRL));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_LALT",   mrb_fixnum_value(KMOD_LALT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_RALT",   mrb_fixnum_value(KMOD_RALT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_LGUI",   mrb_fixnum_value(KMOD_LGUI));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_RGUI",   mrb_fixnum_value(KMOD_RGUI));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_NUM",    mrb_fixnum_value(KMOD_NUM));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_CAPS",   mrb_fixnum_value(KMOD_CAPS));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_MODE",   mrb_fixnum_value(KMOD_MODE));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_CTRL",   mrb_fixnum_value(KMOD_CTRL));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_SHIFT",  mrb_fixnum_value(KMOD_SHIFT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_ALT",    mrb_fixnum_value(KMOD_ALT));
  mrb_define_const(mrb, mod_Keyboard, "KMOD_GUI",    mrb_fixnum_value(KMOD_GUI));
  mrb_gc_arena_restore(mrb, arena_size);


  arena_size = mrb_gc_arena_save(mrb);
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_UNKNOWN", mrb_fixnum_value(SDL_SCANCODE_UNKNOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_A", mrb_fixnum_value(SDL_SCANCODE_A));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_B", mrb_fixnum_value(SDL_SCANCODE_B));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_C", mrb_fixnum_value(SDL_SCANCODE_C));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_D", mrb_fixnum_value(SDL_SCANCODE_D));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_E", mrb_fixnum_value(SDL_SCANCODE_E));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F", mrb_fixnum_value(SDL_SCANCODE_F));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_G", mrb_fixnum_value(SDL_SCANCODE_G));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_H", mrb_fixnum_value(SDL_SCANCODE_H));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_I", mrb_fixnum_value(SDL_SCANCODE_I));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_J", mrb_fixnum_value(SDL_SCANCODE_J));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_K", mrb_fixnum_value(SDL_SCANCODE_K));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_L", mrb_fixnum_value(SDL_SCANCODE_L));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_M", mrb_fixnum_value(SDL_SCANCODE_M));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_N", mrb_fixnum_value(SDL_SCANCODE_N));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_O", mrb_fixnum_value(SDL_SCANCODE_O));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_P", mrb_fixnum_value(SDL_SCANCODE_P));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Q", mrb_fixnum_value(SDL_SCANCODE_Q));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_R", mrb_fixnum_value(SDL_SCANCODE_R));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_S", mrb_fixnum_value(SDL_SCANCODE_S));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_T", mrb_fixnum_value(SDL_SCANCODE_T));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_U", mrb_fixnum_value(SDL_SCANCODE_U));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_V", mrb_fixnum_value(SDL_SCANCODE_V));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_W", mrb_fixnum_value(SDL_SCANCODE_W));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_X", mrb_fixnum_value(SDL_SCANCODE_X));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Y", mrb_fixnum_value(SDL_SCANCODE_Y));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_Z", mrb_fixnum_value(SDL_SCANCODE_Z));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_1", mrb_fixnum_value(SDL_SCANCODE_1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_2", mrb_fixnum_value(SDL_SCANCODE_2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_3", mrb_fixnum_value(SDL_SCANCODE_3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_4", mrb_fixnum_value(SDL_SCANCODE_4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_5", mrb_fixnum_value(SDL_SCANCODE_5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_6", mrb_fixnum_value(SDL_SCANCODE_6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_7", mrb_fixnum_value(SDL_SCANCODE_7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_8", mrb_fixnum_value(SDL_SCANCODE_8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_9", mrb_fixnum_value(SDL_SCANCODE_9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_0", mrb_fixnum_value(SDL_SCANCODE_0));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RETURN", mrb_fixnum_value(SDL_SCANCODE_RETURN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_ESCAPE", mrb_fixnum_value(SDL_SCANCODE_ESCAPE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_BACKSPACE", mrb_fixnum_value(SDL_SCANCODE_BACKSPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_TAB", mrb_fixnum_value(SDL_SCANCODE_TAB));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SPACE", mrb_fixnum_value(SDL_SCANCODE_SPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MINUS", mrb_fixnum_value(SDL_SCANCODE_MINUS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_EQUALS", mrb_fixnum_value(SDL_SCANCODE_EQUALS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LEFTBRACKET", mrb_fixnum_value(SDL_SCANCODE_LEFTBRACKET));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RIGHTBRACKET", mrb_fixnum_value(SDL_SCANCODE_RIGHTBRACKET));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_BACKSLASH", mrb_fixnum_value(SDL_SCANCODE_BACKSLASH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_NONUSHASH", mrb_fixnum_value(SDL_SCANCODE_NONUSHASH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SEMICOLON", mrb_fixnum_value(SDL_SCANCODE_SEMICOLON));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_APOSTROPHE", mrb_fixnum_value(SDL_SCANCODE_APOSTROPHE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_GRAVE", mrb_fixnum_value(SDL_SCANCODE_GRAVE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_COMMA", mrb_fixnum_value(SDL_SCANCODE_COMMA));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PERIOD", mrb_fixnum_value(SDL_SCANCODE_PERIOD));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SLASH", mrb_fixnum_value(SDL_SCANCODE_SLASH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CAPSLOCK", mrb_fixnum_value(SDL_SCANCODE_CAPSLOCK));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F1", mrb_fixnum_value(SDL_SCANCODE_F1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F2", mrb_fixnum_value(SDL_SCANCODE_F2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F3", mrb_fixnum_value(SDL_SCANCODE_F3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F4", mrb_fixnum_value(SDL_SCANCODE_F4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F5", mrb_fixnum_value(SDL_SCANCODE_F5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F6", mrb_fixnum_value(SDL_SCANCODE_F6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F7", mrb_fixnum_value(SDL_SCANCODE_F7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F8", mrb_fixnum_value(SDL_SCANCODE_F8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F9", mrb_fixnum_value(SDL_SCANCODE_F9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F10", mrb_fixnum_value(SDL_SCANCODE_F10));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F11", mrb_fixnum_value(SDL_SCANCODE_F11));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F12", mrb_fixnum_value(SDL_SCANCODE_F12));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PRINTSCREEN", mrb_fixnum_value(SDL_SCANCODE_PRINTSCREEN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SCROLLLOCK", mrb_fixnum_value(SDL_SCANCODE_SCROLLLOCK));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PAUSE", mrb_fixnum_value(SDL_SCANCODE_PAUSE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INSERT", mrb_fixnum_value(SDL_SCANCODE_INSERT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_HOME", mrb_fixnum_value(SDL_SCANCODE_HOME));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PAGEUP", mrb_fixnum_value(SDL_SCANCODE_PAGEUP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_DELETE", mrb_fixnum_value(SDL_SCANCODE_DELETE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_END", mrb_fixnum_value(SDL_SCANCODE_END));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PAGEDOWN", mrb_fixnum_value(SDL_SCANCODE_PAGEDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RIGHT", mrb_fixnum_value(SDL_SCANCODE_RIGHT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LEFT", mrb_fixnum_value(SDL_SCANCODE_LEFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_DOWN", mrb_fixnum_value(SDL_SCANCODE_DOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_UP", mrb_fixnum_value(SDL_SCANCODE_UP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_NUMLOCKCLEAR", mrb_fixnum_value(SDL_SCANCODE_NUMLOCKCLEAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_DIVIDE", mrb_fixnum_value(SDL_SCANCODE_KP_DIVIDE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MULTIPLY", mrb_fixnum_value(SDL_SCANCODE_KP_MULTIPLY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MINUS", mrb_fixnum_value(SDL_SCANCODE_KP_MINUS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_PLUS", mrb_fixnum_value(SDL_SCANCODE_KP_PLUS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_ENTER", mrb_fixnum_value(SDL_SCANCODE_KP_ENTER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_1", mrb_fixnum_value(SDL_SCANCODE_KP_1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_2", mrb_fixnum_value(SDL_SCANCODE_KP_2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_3", mrb_fixnum_value(SDL_SCANCODE_KP_3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_4", mrb_fixnum_value(SDL_SCANCODE_KP_4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_5", mrb_fixnum_value(SDL_SCANCODE_KP_5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_6", mrb_fixnum_value(SDL_SCANCODE_KP_6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_7", mrb_fixnum_value(SDL_SCANCODE_KP_7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_8", mrb_fixnum_value(SDL_SCANCODE_KP_8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_9", mrb_fixnum_value(SDL_SCANCODE_KP_9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_0", mrb_fixnum_value(SDL_SCANCODE_KP_0));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_PERIOD", mrb_fixnum_value(SDL_SCANCODE_KP_PERIOD));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_NONUSBACKSLASH", mrb_fixnum_value(SDL_SCANCODE_NONUSBACKSLASH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_APPLICATION", mrb_fixnum_value(SDL_SCANCODE_APPLICATION));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_POWER", mrb_fixnum_value(SDL_SCANCODE_POWER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_EQUALS", mrb_fixnum_value(SDL_SCANCODE_KP_EQUALS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F13", mrb_fixnum_value(SDL_SCANCODE_F13));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F14", mrb_fixnum_value(SDL_SCANCODE_F14));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F15", mrb_fixnum_value(SDL_SCANCODE_F15));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F16", mrb_fixnum_value(SDL_SCANCODE_F16));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F17", mrb_fixnum_value(SDL_SCANCODE_F17));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F18", mrb_fixnum_value(SDL_SCANCODE_F18));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F19", mrb_fixnum_value(SDL_SCANCODE_F19));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F20", mrb_fixnum_value(SDL_SCANCODE_F20));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F21", mrb_fixnum_value(SDL_SCANCODE_F21));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F22", mrb_fixnum_value(SDL_SCANCODE_F22));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F23", mrb_fixnum_value(SDL_SCANCODE_F23));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_F24", mrb_fixnum_value(SDL_SCANCODE_F24));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_EXECUTE", mrb_fixnum_value(SDL_SCANCODE_EXECUTE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_HELP", mrb_fixnum_value(SDL_SCANCODE_HELP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MENU", mrb_fixnum_value(SDL_SCANCODE_MENU));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SELECT", mrb_fixnum_value(SDL_SCANCODE_SELECT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_STOP", mrb_fixnum_value(SDL_SCANCODE_STOP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AGAIN", mrb_fixnum_value(SDL_SCANCODE_AGAIN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_UNDO", mrb_fixnum_value(SDL_SCANCODE_UNDO));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CUT", mrb_fixnum_value(SDL_SCANCODE_CUT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_COPY", mrb_fixnum_value(SDL_SCANCODE_COPY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PASTE", mrb_fixnum_value(SDL_SCANCODE_PASTE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_FIND", mrb_fixnum_value(SDL_SCANCODE_FIND));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MUTE", mrb_fixnum_value(SDL_SCANCODE_MUTE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_VOLUMEUP", mrb_fixnum_value(SDL_SCANCODE_VOLUMEUP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_VOLUMEDOWN", mrb_fixnum_value(SDL_SCANCODE_VOLUMEDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_COMMA", mrb_fixnum_value(SDL_SCANCODE_KP_COMMA));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_EQUALSAS400", mrb_fixnum_value(SDL_SCANCODE_KP_EQUALSAS400));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL1", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL2", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL3", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL4", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL5", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL6", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL7", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL8", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_INTERNATIONAL9", mrb_fixnum_value(SDL_SCANCODE_INTERNATIONAL9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG1", mrb_fixnum_value(SDL_SCANCODE_LANG1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG2", mrb_fixnum_value(SDL_SCANCODE_LANG2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG3", mrb_fixnum_value(SDL_SCANCODE_LANG3));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG4", mrb_fixnum_value(SDL_SCANCODE_LANG4));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG5", mrb_fixnum_value(SDL_SCANCODE_LANG5));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG6", mrb_fixnum_value(SDL_SCANCODE_LANG6));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG7", mrb_fixnum_value(SDL_SCANCODE_LANG7));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG8", mrb_fixnum_value(SDL_SCANCODE_LANG8));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LANG9", mrb_fixnum_value(SDL_SCANCODE_LANG9));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_ALTERASE", mrb_fixnum_value(SDL_SCANCODE_ALTERASE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SYSREQ", mrb_fixnum_value(SDL_SCANCODE_SYSREQ));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CANCEL", mrb_fixnum_value(SDL_SCANCODE_CANCEL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CLEAR", mrb_fixnum_value(SDL_SCANCODE_CLEAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_PRIOR", mrb_fixnum_value(SDL_SCANCODE_PRIOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RETURN2", mrb_fixnum_value(SDL_SCANCODE_RETURN2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SEPARATOR", mrb_fixnum_value(SDL_SCANCODE_SEPARATOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_OUT", mrb_fixnum_value(SDL_SCANCODE_OUT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_OPER", mrb_fixnum_value(SDL_SCANCODE_OPER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CLEARAGAIN", mrb_fixnum_value(SDL_SCANCODE_CLEARAGAIN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CRSEL", mrb_fixnum_value(SDL_SCANCODE_CRSEL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_EXSEL", mrb_fixnum_value(SDL_SCANCODE_EXSEL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_00", mrb_fixnum_value(SDL_SCANCODE_KP_00));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_000", mrb_fixnum_value(SDL_SCANCODE_KP_000));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_THOUSANDSSEPARATOR", mrb_fixnum_value(SDL_SCANCODE_THOUSANDSSEPARATOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_DECIMALSEPARATOR", mrb_fixnum_value(SDL_SCANCODE_DECIMALSEPARATOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CURRENCYUNIT", mrb_fixnum_value(SDL_SCANCODE_CURRENCYUNIT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CURRENCYSUBUNIT", mrb_fixnum_value(SDL_SCANCODE_CURRENCYSUBUNIT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_LEFTPAREN", mrb_fixnum_value(SDL_SCANCODE_KP_LEFTPAREN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_RIGHTPAREN", mrb_fixnum_value(SDL_SCANCODE_KP_RIGHTPAREN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_LEFTBRACE", mrb_fixnum_value(SDL_SCANCODE_KP_LEFTBRACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_RIGHTBRACE", mrb_fixnum_value(SDL_SCANCODE_KP_RIGHTBRACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_TAB", mrb_fixnum_value(SDL_SCANCODE_KP_TAB));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_BACKSPACE", mrb_fixnum_value(SDL_SCANCODE_KP_BACKSPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_A", mrb_fixnum_value(SDL_SCANCODE_KP_A));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_B", mrb_fixnum_value(SDL_SCANCODE_KP_B));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_C", mrb_fixnum_value(SDL_SCANCODE_KP_C));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_D", mrb_fixnum_value(SDL_SCANCODE_KP_D));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_E", mrb_fixnum_value(SDL_SCANCODE_KP_E));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_F", mrb_fixnum_value(SDL_SCANCODE_KP_F));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_XOR", mrb_fixnum_value(SDL_SCANCODE_KP_XOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_POWER", mrb_fixnum_value(SDL_SCANCODE_KP_POWER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_PERCENT", mrb_fixnum_value(SDL_SCANCODE_KP_PERCENT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_LESS", mrb_fixnum_value(SDL_SCANCODE_KP_LESS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_GREATER", mrb_fixnum_value(SDL_SCANCODE_KP_GREATER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_AMPERSAND", mrb_fixnum_value(SDL_SCANCODE_KP_AMPERSAND));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_DBLAMPERSAND", mrb_fixnum_value(SDL_SCANCODE_KP_DBLAMPERSAND));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_VERTICALBAR", mrb_fixnum_value(SDL_SCANCODE_KP_VERTICALBAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_DBLVERTICALBAR", mrb_fixnum_value(SDL_SCANCODE_KP_DBLVERTICALBAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_COLON", mrb_fixnum_value(SDL_SCANCODE_KP_COLON));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_HASH", mrb_fixnum_value(SDL_SCANCODE_KP_HASH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_SPACE", mrb_fixnum_value(SDL_SCANCODE_KP_SPACE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_AT", mrb_fixnum_value(SDL_SCANCODE_KP_AT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_EXCLAM", mrb_fixnum_value(SDL_SCANCODE_KP_EXCLAM));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMSTORE", mrb_fixnum_value(SDL_SCANCODE_KP_MEMSTORE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMRECALL", mrb_fixnum_value(SDL_SCANCODE_KP_MEMRECALL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMCLEAR", mrb_fixnum_value(SDL_SCANCODE_KP_MEMCLEAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMADD", mrb_fixnum_value(SDL_SCANCODE_KP_MEMADD));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMSUBTRACT", mrb_fixnum_value(SDL_SCANCODE_KP_MEMSUBTRACT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMMULTIPLY", mrb_fixnum_value(SDL_SCANCODE_KP_MEMMULTIPLY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_MEMDIVIDE", mrb_fixnum_value(SDL_SCANCODE_KP_MEMDIVIDE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_PLUSMINUS", mrb_fixnum_value(SDL_SCANCODE_KP_PLUSMINUS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_CLEAR", mrb_fixnum_value(SDL_SCANCODE_KP_CLEAR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_CLEARENTRY", mrb_fixnum_value(SDL_SCANCODE_KP_CLEARENTRY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_BINARY", mrb_fixnum_value(SDL_SCANCODE_KP_BINARY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_OCTAL", mrb_fixnum_value(SDL_SCANCODE_KP_OCTAL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_DECIMAL", mrb_fixnum_value(SDL_SCANCODE_KP_DECIMAL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KP_HEXADECIMAL", mrb_fixnum_value(SDL_SCANCODE_KP_HEXADECIMAL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LCTRL", mrb_fixnum_value(SDL_SCANCODE_LCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LSHIFT", mrb_fixnum_value(SDL_SCANCODE_LSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LALT", mrb_fixnum_value(SDL_SCANCODE_LALT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_LGUI", mrb_fixnum_value(SDL_SCANCODE_LGUI));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RCTRL", mrb_fixnum_value(SDL_SCANCODE_RCTRL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RSHIFT", mrb_fixnum_value(SDL_SCANCODE_RSHIFT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RALT", mrb_fixnum_value(SDL_SCANCODE_RALT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_RGUI", mrb_fixnum_value(SDL_SCANCODE_RGUI));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MODE", mrb_fixnum_value(SDL_SCANCODE_MODE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AUDIONEXT", mrb_fixnum_value(SDL_SCANCODE_AUDIONEXT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AUDIOPREV", mrb_fixnum_value(SDL_SCANCODE_AUDIOPREV));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AUDIOSTOP", mrb_fixnum_value(SDL_SCANCODE_AUDIOSTOP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AUDIOPLAY", mrb_fixnum_value(SDL_SCANCODE_AUDIOPLAY));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AUDIOMUTE", mrb_fixnum_value(SDL_SCANCODE_AUDIOMUTE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MEDIASELECT", mrb_fixnum_value(SDL_SCANCODE_MEDIASELECT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_WWW", mrb_fixnum_value(SDL_SCANCODE_WWW));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_MAIL", mrb_fixnum_value(SDL_SCANCODE_MAIL));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_CALCULATOR", mrb_fixnum_value(SDL_SCANCODE_CALCULATOR));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_COMPUTER", mrb_fixnum_value(SDL_SCANCODE_COMPUTER));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_SEARCH", mrb_fixnum_value(SDL_SCANCODE_AC_SEARCH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_HOME", mrb_fixnum_value(SDL_SCANCODE_AC_HOME));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_BACK", mrb_fixnum_value(SDL_SCANCODE_AC_BACK));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_FORWARD", mrb_fixnum_value(SDL_SCANCODE_AC_FORWARD));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_STOP", mrb_fixnum_value(SDL_SCANCODE_AC_STOP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_REFRESH", mrb_fixnum_value(SDL_SCANCODE_AC_REFRESH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_AC_BOOKMARKS", mrb_fixnum_value(SDL_SCANCODE_AC_BOOKMARKS));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_BRIGHTNESSDOWN", mrb_fixnum_value(SDL_SCANCODE_BRIGHTNESSDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_BRIGHTNESSUP", mrb_fixnum_value(SDL_SCANCODE_BRIGHTNESSUP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_DISPLAYSWITCH", mrb_fixnum_value(SDL_SCANCODE_DISPLAYSWITCH));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KBDILLUMTOGGLE", mrb_fixnum_value(SDL_SCANCODE_KBDILLUMTOGGLE));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KBDILLUMDOWN", mrb_fixnum_value(SDL_SCANCODE_KBDILLUMDOWN));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_KBDILLUMUP", mrb_fixnum_value(SDL_SCANCODE_KBDILLUMUP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_EJECT", mrb_fixnum_value(SDL_SCANCODE_EJECT));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_SLEEP", mrb_fixnum_value(SDL_SCANCODE_SLEEP));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_APP1", mrb_fixnum_value(SDL_SCANCODE_APP1));
  mrb_define_const(mrb, mod_Keyboard, "SDL_SCANCODE_APP2", mrb_fixnum_value(SDL_SCANCODE_APP2));
  mrb_define_const(mrb, mod_Keyboard, "SDL_NUM_SCANCODES", mrb_fixnum_value(SDL_NUM_SCANCODES));
  mrb_gc_arena_restore(mrb, arena_size);
}

void
mruby_sdl2_keyboard_final(mrb_state *mrb)
{
}

