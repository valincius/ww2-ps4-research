#include <_pthread.h>
#include <string>
#include <functional>
#include <queue>

#include "Logger.h"
#include "Controller.h"
#include "HudElement.h"

#include "ModMenu.h"

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

void* mainThread(void*) {
	auto controllerHud = HudElements::Text(0, "Waiting to find controller...", 1, 80.0, { 255, 255, 255 }, 7, .75);

	Controller controller;
	Logger::log("Found controller ID %i\n", controller.get_handle());

	controllerHud.SetText("Found controller!");

	HudElements::DestroyAll();

	using ModMenu::Menu;

	Menu::Init({
		{
			"Sub Menu 1",
			{
				{ "Option 1", [] { Logger::log("Option 1 pressed\n"); } },
				{ "Option 2", [] { Logger::log("Option 2 pressed\n"); } },
				{ "Option 3", [] { Logger::log("Option 3 pressed\n"); } },
				{
					"Nested", {
						{ "Nested - Option 1" },
						{ "Nested - Option 2" },
						{ "Nested - Option 3" },
						{ "Nested - Option 4" },
						{ "Nested - Option 5" },
					}
				},
			},
		},
		{
			"Sub Menu 2",
			{
				{ "Option 1" },
				{ "Option 2" },
				{ "Option 3" },
				{ "Option 4" },
				{ "Option 5" },
			},
		},
		{
			"Sub Menu 3",
			{
				{ "Option 1" },
				{ "Option 2" },
				{ "Option 3" },
				{ "Option 4" },
				{ "Option 5" },
				{ "Option 6" },
				{ "Option 7" },
				{ "Option 8" },
				{ "Option 9" },
				{ "Option 10" },
			},
		},
	});

	Menu::SpawnHuds();

	SceKernelUseconds pressDelay = 250;
	while (true) {
		controller.update_button_state();

		if(Menu::IsOpen()) {
			if(controller.is_button_pressed(SCE_PAD_BUTTON_UP)) {
				Menu::ScrollPrev();

				sceKernelUsleep(pressDelay);
			}
			if(controller.is_button_pressed(SCE_PAD_BUTTON_DOWN)) {
				Menu::ScrollNext();

				sceKernelUsleep(pressDelay);
			}

			if(controller.is_button_pressed(SCE_PAD_BUTTON_CROSS)) {
				Menu::Invoke();

				sceKernelUsleep(pressDelay);
			}
			if(controller.is_button_pressed(SCE_PAD_BUTTON_SQUARE)) {
				Menu::GoBack();

				sceKernelUsleep(pressDelay);
			}
		}
		else {
			if(controller.is_button_pressed(SCE_PAD_BUTTON_UP)) {
				Menu::SetOpenState(true);

				sceKernelUsleep(pressDelay);
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