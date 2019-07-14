#pragma once

#include <inttypes.h>

uint32_t G_FindConfigstringIndex(const char* p0, uint64_t p1, uint32_t p2, int32_t p3, const char* p4);

uint32_t G_LocalizedStringIndex(const char* text);

uint32_t G_MaterialIndex(const char* material);

namespace HudElements {
	union Color_s {
		uint32_t rgba;
		struct {
			uint8_t r, g, b, a;
		};

		Color_s(uint32_t rgba) : rgba(rgba) {}
		Color_s(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
	};

	struct HudElement_s {
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
		Color_s rgba; // 0x30
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

	uint32_t getLevelTime();

	void DestroyAll();

	class HudElement {
	protected:
		HudElement_s* m_hud;
	public:
		HudElement(int client = 0, int team = 0);
		HudElement* Hud();
		HudElement_s* Ptr();
		HudElement* SetVisible(bool visible = true);
		HudElement* SetColor(Color_s color);
		HudElement* SetPosition(float x, float y);

		HudElement* scaleOverTime(char seconds, uint16_t width, uint16_t height);
		HudElement* moveOverTime(char seconds, float x, float y);
	};

	class Text : public HudElement {
	public:
		Text();
		Text(int client, const char* text, float x = 1.0, float y = 1.0, Color_s rgba = 0xFFFFFFFF, uint32_t font = 0x07, float fontScale = 1.0, bool visible = true);
		Text* SetText(const char* text);
		Text* SetFont(uint32_t font);
		Text* SetFontScale(float fontScale);
	};
};