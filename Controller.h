#pragma once

#include <pad.h>
#include <cstring>

/* logic should be moved into .cpp file */
class Controller {
public:
	Controller();
	~Controller();

	void update_button_state();
	int32_t get_handle() const;
	bool is_connected() const;

	bool is_button_down(uint32_t button) const;
	bool is_button_pressed(uint32_t button) const;

	float analog_r2() const;
	float analog_l2() const;
private:
	ScePadData m_pad_data;
	int32_t m_handle;
	uint32_t m_curr_buttons;
	uint32_t m_prev_buttons;
};

inline int32_t Controller::get_handle() const {
	return m_handle;
}

inline bool Controller::is_connected() const {
	return m_pad_data.connected;
}

inline bool Controller::is_button_down(uint32_t button) const {
	return (m_curr_buttons & button) != 0;
}

inline bool Controller::is_button_pressed(uint32_t button) const {
	uint32_t pressed = m_curr_buttons & ~m_prev_buttons;
	return (pressed & button) != 0;
}

inline float Controller::analog_r2() const {
	return static_cast<float>(m_pad_data.analogButtons.r2) / 255.f;
}
inline float Controller::analog_l2() const {
	return static_cast<float>(m_pad_data.analogButtons.l2) / 255.f;
}

Controller::Controller() : m_handle(-1), m_curr_buttons(0), m_prev_buttons(0) {
	memset(&m_pad_data, 0, sizeof(m_pad_data));


	/* currently super slow, need a better way to do this */
	for(int i = 0; i < UINT32_MAX; i++) {
		ScePadData pad_data;
		if(scePadReadState(i, &pad_data) == 0) {
			m_handle = i;
			break;
		}
	}
}

Controller::~Controller() {}

void Controller::update_button_state() {
	scePadReadState(m_handle, &m_pad_data);
	m_prev_buttons = m_curr_buttons;
	m_curr_buttons = m_pad_data.buttons;
}