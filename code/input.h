
#ifndef INPUT_H
#define INPUT_H

namespace input { 

enum Key {
	Key_None,
	
	Key_0, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6, Key_7, Key_8, Key_9,
	
	Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, Key_H, Key_I, Key_J, Key_K,
	Key_L, Key_M, Key_N, Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V,
	Key_W, Key_X, Key_Y, Key_Z,
	
	Key_Space, Key_Tab, Key_Escape, Key_Tilde, Key_Backspace, Key_Enter,
	
	Key_Minus, Key_Equals, 
	
	Key_Up, Key_Down, Key_Left, Key_Right,
	
	Key_Count
};

enum Mouse_Button {
	Mouse_Button_Left,
	Mouse_Button_Right,
	Mouse_Button_Middle,
	Mouse_Button_Count,
};

struct Game_Input {
	b32 keys[Key_Count];
	Key latest_key_down;
	
	b32 buttons[Mouse_Button_Count];
	s32 mouse_x, mouse_y;
	s32 relative_mouse_x, relative_mouse_y;
};

Key get_latest_key_down();
b32 is_key_down(Key key);
b32 is_key_pressed(Key key);
b32 is_key_released(Key key);

b32 is_shift_key_down();
b32 is_caps_lock_on();

b32 is_mouse_button_down(Mouse_Button button);
b32 is_mouse_button_pressed(Mouse_Button button);
b32 is_mouse_button_released(Mouse_Button button);

s32 get_mouse_x();
s32 get_mouse_y();
s32 get_relative_mouse_x();
s32 get_relative_mouse_y();

}

#endif



