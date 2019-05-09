#include <_pthread.h>
#include <string>
#include <functional>
#include <queue>

#include "logger.h"
#include "Controller.h"
#include "HudElement.h"

uint64_t g_entity(int client, int offset = 0x0) {
	return (0x0C628A10 + (client * 0x418) + offset);
}
uint64_t player_state(int client, int offset = 0x0) {
	return (0x0C9C82C0 + (client * 0x6180) + offset);
}

void PatchInJump(uint64_t address, void* destination) {
	if (!address || !destination)
		return;

	*(uint8_t*)(address) = 0xFF;
	*(uint8_t*)(address + 1) = 0x25;
	*(uint8_t*)(address + 2) = 0x00;
	*(uint8_t*)(address + 3) = 0x00;
	*(uint8_t*)(address + 4) = 0x00;
	*(uint8_t*)(address + 5) = 0x00;
	*(uint64_t*)(address + 6) = (uint64_t)destination;
}

/*HudElement *spawnShader(uint32_t client, float x, float y, uint32_t rgba, const char *material, uint16_t width, uint16_t height) {
	HudElement* hud = spawnHudElem(client, x, y, rgba);

	hud->type = 0x04;
	hud->width = width;
	hud->height = height;
	hud->material = G_MaterialIndex(material);

	return hud;
}*/

int wrap(int value, int min, int max) {
	return value < min ? max : value > max ? min : value;
}

namespace Menu {
	class Option {
		std::string m_text;
		HudElements::Text* m_hud;
		std::function<void(Option*)> m_function;

	public:
		Option(std::string text, std::function<void(Option*)> function) {
			m_text = text;
			m_function = function;

			m_hud = new HudElements::Text(0, m_text.c_str(), 0, 0, 0xFFFFFFFF, 1, .80);
			m_hud->Hud()->SetVisible(false);
		}
		void Call() {
			m_function(this);
		}
		HudElements::Text* Hud() {
			return m_hud;
		}
	};
	class SubMenu {
		int m_currentOptionIndex;
		HudElements::Text* m_titleHud;
		std::vector<Option> m_options;

	public:
		SubMenu(std::string name, std::vector<Option> options) {
			m_titleHud = new HudElements::Text(0, name.c_str(), 1, 0, { 255, 255, 0 }, 1, 1.0);
			m_titleHud->SetVisible(false);

			m_options = options;
			m_currentOptionIndex = 0;

			int i = 0;
			for(auto option : m_options) {
				if(i == 0) {
					option.Hud()->SetColor({ 255, 0, 0 });
				}
				option.Hud()->SetPosition(0, 30.0 + (i * 25.0));
				i++;
			}
		}
		HudElements::Text* TitleHud() {
			return m_titleHud;
		}
		int const OptionCount() {
			return m_options.size();
		}
		std::vector<Option>& AllOptions() {
			return m_options;
		}
		Option& CurrentOption() {
			return m_options[m_currentOptionIndex];
		}
		void ChangeOption(int index) {
			CurrentOption().Hud()->SetColor({ 255, 255, 255 });
			m_currentOptionIndex = wrap(index, 0, OptionCount());
			CurrentOption().Hud()->SetColor({ 255, 0, 0 });
		}
		void PrevOption() {
			ChangeOption(m_currentOptionIndex - 1);
		}
		void NextOption() {
			ChangeOption(m_currentOptionIndex + 1);
		}
	};
	class Menu {
		int m_currentMenuIndex;
		std::vector<SubMenu> m_submenus;
		bool m_open;

	public:
		Menu(std::vector<SubMenu> submenus) {
			m_submenus = submenus;
			m_currentMenuIndex = 0;
		}
		bool IsOpen() {
			return m_open;
		}
		int const MenuCount() {
			return m_submenus.size();
		}
		SubMenu& CurrentMenu() {
			return m_submenus[m_currentMenuIndex];
		}

		void Open() {
			SubMenu currentMenu = CurrentMenu();
			currentMenu.TitleHud()->SetVisible(true);
			for(auto option : currentMenu.AllOptions()) { //abstract this
				option.Hud()->SetVisible(true);
			}
			
			m_open = true;
		}
		void Close() {
			SubMenu currentMenu = CurrentMenu();
			currentMenu.TitleHud()->SetVisible(false);
			for(auto option : currentMenu.AllOptions()) { //abstract this
				option.Hud()->SetVisible(false);
			}

			m_open = false;
		}
		void ChangeMenu(int index) {
			Close();
			m_currentMenuIndex = wrap(index, 0, MenuCount());
			Open();
		}
		void PrevMenu() {
			ChangeMenu(m_currentMenuIndex - 1);
		}
		void NextMenu() {
			ChangeMenu(m_currentMenuIndex + 1);
		}
	};
};


/*
TODO
	Make a common class for Menu::SubMenu & Menu::Menu since they're both really similar
	Add some animations for transitioning between options/menus
	Make some standard colors so we don't have to repeat everything every time
	Create a configure kinda thing for the menus
	Make menu design a little more appealing
	Have some different variants for the function types accepted for the options
	Seperate out Controller.h and HudElement.h to .cpp & .h
	Normalize coding style throughout (VS keeps changing how I write everything out)
*/

void* mainThread(void*) {
	Controller controller;
	Logger::log("Found controller ID %i\n", controller.get_handle());

	HudElements::DestroyAll();

	Menu::Menu menu({
		{
			"Sub Menu 1",
			{
				Menu::Option("Option 1", [](Menu::Option*) {}),
				Menu::Option("Option 2", [](Menu::Option*) {}),
				Menu::Option("Option 3", [](Menu::Option*) {}),
			},
		},
		{
			"Sub Menu 2",
			{
				Menu::Option("Option 1", [](Menu::Option*) {}),
				Menu::Option("Option 2", [](Menu::Option*) {}),
				Menu::Option("Option 3", [](Menu::Option*) {}),
				Menu::Option("Option 4", [](Menu::Option*) {}),
				Menu::Option("Option 5", [](Menu::Option*) {}),
			},
		},
		{
			"Sub Menu 3",
			{
				Menu::Option("Option 1", [](Menu::Option*) {}),
				Menu::Option("Option 2", [](Menu::Option*) {}),
				Menu::Option("Option 3", [](Menu::Option*) {}),
				Menu::Option("Option 4", [](Menu::Option*) {}),
				Menu::Option("Option 5", [](Menu::Option*) {}),
				Menu::Option("Option 6", [](Menu::Option*) {}),
				Menu::Option("Option 7", [](Menu::Option*) {}),
				Menu::Option("Option 8", [](Menu::Option*) {}),
				Menu::Option("Option 9", [](Menu::Option*) {}),
				Menu::Option("Option 10", [](Menu::Option*) {}),
			},
		},
	});

	while (true) {
		controller.update_button_state();

		if(menu.IsOpen()) {
			if(controller.is_button_pressed(SCE_PAD_BUTTON_UP)) {
				menu.CurrentMenu().PrevOption();

				sceKernelUsleep(200);
			}
			if(controller.is_button_pressed(SCE_PAD_BUTTON_DOWN)) {
				menu.CurrentMenu().NextOption();

				sceKernelUsleep(200);
			}

			if(controller.is_button_pressed(SCE_PAD_BUTTON_LEFT)) {
				menu.PrevMenu();

				sceKernelUsleep(200);
			}
			if(controller.is_button_pressed(SCE_PAD_BUTTON_RIGHT)) {
				menu.NextMenu();

				sceKernelUsleep(200);
			}

			if(controller.is_button_pressed(SCE_PAD_BUTTON_CROSS)) {
				menu.CurrentMenu().CurrentOption().Call();

				sceKernelUsleep(200);
			}
			if(controller.is_button_pressed(SCE_PAD_BUTTON_SQUARE)) {
				menu.Close();

				sceKernelUsleep(200);
			}
		}
		else {
			if(controller.is_button_pressed(SCE_PAD_BUTTON_UP)) {
				menu.Open();

				sceKernelUsleep(200);
			}
		}
	}
}

ScePthread main_thread;
extern "C" {
	int module_start(size_t args, const void* argp) {
		Logger::initialize();

		scePthreadSetcancelstate(PTHREAD_CANCEL_ENABLE, 0);

		scePthreadCreate(&main_thread, NULL, mainThread, NULL, "Main Thread");

		return 0;
	}
	int module_stop(size_t args, const void* argp) {
		Logger::destory();

		scePthreadCancel(main_thread);

		return 0;
	}
}