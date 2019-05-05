#include <_pthread.h>
#include <string>

#include "logger.h"

uint64_t g_entity(int client, int offset = 0x0) {
	return (0x0C628A10 + (client * 0x418) + offset);
}
uint64_t player_state(int client, int offset = 0x0) {
	return (0x0C9C82C0 + (client * 0x6180) + offset);
}

void dump_memory(void *address, size_t count) {
	size_t line_count = 16;
	for (int x = 0; x < count / line_count; x++) {
		Logger::log("%08X    ", (uint32_t)(size_t)address + (x * line_count));
		for (int i = 0; i < line_count; i++) {
			unsigned char character = *(unsigned char*)((uint32_t)(size_t)address + ((x * line_count) + i));
			Logger::log("%02X ", character);
		}
		Logger::log("    ");
		for (int i = 0; i < line_count; i++) {
			unsigned char character = *(unsigned char*)((uint32_t)(size_t)address + ((x * line_count) + i));
			if (character <= 0x20 || character >= 0x7F) {
				character = '.';
			}
			Logger::log("%c ", character);
		}
		Logger::log("\n");
	}
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

#pragma pack(push, 1)
struct HudElement {
	char padding_0[0x04]; // 0x0
	uint32_t font; // 0x4
	uint32_t alignOrg; // 0x8
	uint32_t alignScreen; // 0xc
	float x; // 10
	float y; // 14
	float z; // 18
	uint32_t type; // 0x1c
	float fontScale; // 0x20
	char padding_1[0x0C]; // 0x24
	union {
		uint32_t argb;
		struct { uint8_t a, r, g, b; };
	} argb; // 0x30
	char padding_2[0x10]; // 0x34
	uint32_t width; // 0x44
	uint32_t height; // 0x48
	uint32_t material; // 0x4c
	char padding_3[0x10]; // 0x50
	uint32_t fromWidth; // 0x60
	uint32_t fromHeight; // 0x64
	uint32_t scaleStartTime; // 0x68
	uint32_t scaleTime; // 0x6c
	float fromX; // 0x70
	float fromY; // 0x74
	uint32_t fromAlignOrg; // 0x78
	uint32_t fromAlignScreen; // 0x7c
	uint32_t moveStartTime; // 0x80
	uint32_t moveTime; // 0x84
	uint32_t time; // 0x88
	uint32_t duration; // 0x8c
	float value; // 0x90
	uint32_t localizeStringIndex; // 0x94
	char padding_5[0x24]; // 0x98
	uint32_t index; // 0xbc
	char padding_6[0x01]; // 0xc0
	char visibility; // 0xc1
	char padding_7[0x0E]; // 0xc2
};
#pragma pack(pop)

HudElement* HudElemAlloc(uint32_t client, uint32_t team) {
	return ((HudElement * (*)(uint32_t, uint32_t, uint32_t))0xD6DF30)(client, team, 0);
}

uint32_t sub_917430(const char *p0, uint64_t p1, uint32_t p2, int32_t p3, const char *p4) {
	return ((uint32_t(*)(const char *, uint64_t, uint32_t, int32_t, const char *))0x917430)(p0, p1, p2, p3, p4);
}

uint32_t G_LocalizedStringIndex(const char* text) {
	return sub_917430(text, 570, 0x28A, 1, "localized string");
};

uint32_t G_MaterialIndex(const char* material) {
	return sub_917430(material, 4472, 0x1FF, *(int32_t *)0xC472750, "material");
}

HudElement* spawnHudElem(uint32_t client, float x, float y, uint32_t rgba) {
	HudElement* hud = HudElemAlloc(client, 0);

	hud->alignOrg = 0x04;
	hud->alignScreen = 0x98;
	hud->x = x;
	hud->y = y;
	hud->argb.argb = __builtin_bswap32(rgba);
	hud->visibility = 0x01;

	return hud;
}

HudElement *spawnText(uint32_t client, float x, float y, uint32_t rgba, const char *text, uint32_t font, float fontScale) {
	HudElement* hud = spawnHudElem(client, x, y, rgba);

	hud->type = 0x01;
	hud->font = font;
	hud->fontScale = fontScale;
	hud->localizeStringIndex = G_LocalizedStringIndex(text);

	return hud;
}

HudElement *spawnShader(uint32_t client, float x, float y, uint32_t rgba, const char *material, uint16_t width, uint16_t height) {
	HudElement* hud = spawnHudElem(client, x, y, rgba);

	hud->type = 0x04;
	hud->width = width;
	hud->height = height;
	hud->material = G_MaterialIndex(material);

	return hud;
}

uint32_t getLevelTime() {
	return *(uint32_t *)(0xC472D60 + 0x04);
}

void scaleOverTime(HudElement* hud, char seconds, uint16_t width, uint16_t height) {
	hud->fromHeight = hud->height;
	hud->fromWidth = hud->width;
	hud->scaleStartTime = getLevelTime();
	hud->scaleTime = seconds;
	hud->height = height;
	hud->width = width;
}
void moveOverTime(HudElement* hud, char seconds, float x, float y) {
	hud->fromX = hud->x;
	hud->fromY = hud->y;
	hud->moveStartTime = getLevelTime();
	hud->moveTime = seconds;
	hud->x = x;
	hud->y = y;
}

void* mainThread(void*) {
	sceKernelSleep(1);
	Logger::log("Thread has started...\n");

	HudElement* shader = spawnShader(0, 0, 100, 0x00FF00FF, "white", 100, 100);
	scaleOverTime(shader, 5000, 500, 500);

	HudElement* text = spawnText(0, 0, 100, 0xFF0000FF, "Hello", 0, 3.0);
	moveOverTime(text, 5000, 50, 150);

	while (true) {
		sceKernelSleep(1);
	}

	Logger::log("Done running!\n");
}

ScePthread main_thread;
extern "C" {
	int module_start(size_t args, const void* argp) {
		Logger::initialize();
		Logger::log("Called module_start()\n");

		scePthreadSetcancelstate(PTHREAD_CANCEL_ENABLE, 0);

		scePthreadCreate(&main_thread, NULL, mainThread, NULL, "Main Thread");

		return 0;
	}
	int module_stop(size_t args, const void* argp) {
		Logger::log("Called module_stop()\n");
		
		Logger::destory();

		scePthreadCancel(main_thread);

		return 0;
	}
}
