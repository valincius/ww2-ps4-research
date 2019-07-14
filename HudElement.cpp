#include "HudElement.h"

#include "HudElement.h"

uint32_t G_FindConfigstringIndex(const char* p0, uint64_t p1, uint32_t p2, int32_t p3, const char* p4) {
	return ((uint32_t(*)(const char*, uint64_t, uint32_t, int32_t, const char*))0x917430)(p0, p1, p2, p3, p4);
}

uint32_t G_LocalizedStringIndex(const char* text) {
	return G_FindConfigstringIndex(text, 570, 0x28A, 1, "localized string");
};

uint32_t G_MaterialIndex(const char* material) {
	return G_FindConfigstringIndex(material, 4472, 0x1FF, *(int32_t*)0xC472750, "material");
}

namespace HudElements {
	uint32_t getLevelTime() {
		return *(uint32_t*)(0xC472D60 + 0x04);
	}

	void DestroyAll() {
		((void(*)())0xD6E090)();
	}

	HudElement::HudElement(int client, int team) {
		m_hud = ((HudElement_s * (*)(uint32_t, uint32_t, uint32_t))0xD6DF30)(client, team, 0);
		m_hud->alignOrg = 0x04;
		m_hud->alignScreen = 0x98;
		SetVisible(true);
	}
	HudElement* HudElement::Hud() {
		return this;
	}
	HudElement_s* HudElement::Ptr() {
		return m_hud;
	}
	HudElement* HudElement::SetVisible(bool visible) {
		m_hud->visibility = visible;

		return this;
	}
	HudElement* HudElement::SetColor(Color_s color) {
		m_hud->rgba = color;

		return this;
	}
	HudElement* HudElement::SetPosition(float x, float y) {
		m_hud->x = x;
		m_hud->y = y;

		return this;
	}

	HudElement* HudElement::scaleOverTime(char seconds, uint16_t width, uint16_t height) {
		m_hud->fromHeight = m_hud->height;
		m_hud->fromWidth = m_hud->width;
		m_hud->scaleStartTime = getLevelTime();
		m_hud->scaleTime = seconds;
		m_hud->height = height;
		m_hud->width = width;

		return this;
	}
	HudElement* HudElement::moveOverTime(char seconds, float x, float y) {
		m_hud->fromX = m_hud->x;
		m_hud->fromY = m_hud->y;
		m_hud->moveStartTime = getLevelTime();
		m_hud->moveTime = seconds;
		m_hud->x = x;
		m_hud->y = y;

		return this;
	}

	Text::Text() {}
	Text::Text(int client, const char* text, float x, float y, Color_s rgba, uint32_t font, float fontScale, bool visible) {
		HudElement::HudElement(client, 0);

		m_hud->type = 0x01;
		m_hud->font = font;
		m_hud->fontScale = fontScale;

		SetPosition(x, y);
		SetText(text);
		SetColor({ rgba.r, rgba.g, rgba.b, rgba.a });
		SetVisible(visible);
	}
	Text* Text::SetText(const char* text) {
		m_hud->localizeStringIndex = G_LocalizedStringIndex(text);

		return this;
	}
	Text* Text::SetFont(uint32_t font) {
		m_hud->font = font;

		return this;
	}
	Text* Text::SetFontScale(float fontScale) {
		m_hud->fontScale = fontScale;

		return this;
	}
};