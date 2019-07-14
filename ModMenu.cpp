#include "logger.h"

#include "ModMenu.h"

using namespace ModMenu;

const char* MAIN_MENU_TEXT = "Main Menu";

Option::Option(std::string text, std::function<void()> function) {
	Text = text;
	m_function = function;
}
Option::Option(std::string text, std::vector<Option> options) {
	Text = text;
	int menuIndex = Menu::AddMenu(options);

	m_function = [menuIndex]() { Menu::ChangeMenu(menuIndex); };
}
void Option::Call() {
	m_function();
}

void Menu::Init(std::vector<Option> options) {
	AddMenu(options);
	m_TopMenuIndex = Options.size() - 1;
}
int Menu::AddMenu(std::vector<Option> options) {
	Options.push_back(options);
	MenuIndexes.push_back(0);
	PrevMenuIndexes.push_back(m_TopMenuIndex);

	return m_InternalMenuIndex++;
}

void Menu::SpawnHuds() {
	Menu::HudElements["title"] = new HudElements::Text(0, MAIN_MENU_TEXT, 1, 100.0, { 255, 255, 255 }, 7, 1.0, false);

	for(int i = 0; i < 25; i++) {
		auto hud = new HudElements::Text(0, "", 1, (i * 25.0) + 125.0, { 255, 255, 255 }, 7);
		hud->SetVisible(false);
		OptionsTextHuds.push_back(hud);
	}
}

bool Menu::IsOpen() {
	return m_Open;
}
void Menu::SetOpenState(bool state) {
	m_Open = state;
	if(state) {
		ChangeMenu(m_TopMenuIndex);
		((HudElements::Text *)Menu::HudElements["title"])
			->SetVisible();
	}
	else {
		Menu::HudElements["title"]->SetVisible(false);

		for(auto hud : OptionsTextHuds) {
			hud->SetVisible(false);
		}
	}
}
void Menu::GoBack() {
	if(m_CurrentMenuIndex != m_TopMenuIndex) {
		ChangeMenu(PrevMenuIndexes[m_CurrentMenuIndex]);
	}
	else {
		SetOpenState(false);
	}
}

void Menu::ChangeMenu(int menuIndex) {
	if(menuIndex != PrevMenuIndexes[m_CurrentMenuIndex]) {
		PrevMenuIndexes[menuIndex] = m_CurrentMenuIndex;
	}
	m_CurrentMenuIndex = menuIndex;

	((HudElements::Text*)Menu::HudElements["title"])
		->SetText(CurrentMenuTitle().c_str());

	int i = 0;
	for(auto hud : OptionsTextHuds) {
		auto options = Options[m_CurrentMenuIndex];
		if(i < options.size()) {
			OptionsTextHuds[i]
				->SetText(options[i].Text.c_str())
				->SetColor({ 255, 255, 255 })
				->SetVisible();
		}
		else {
			hud->SetVisible(false);
		}
		i++;
	}

	OptionsTextHuds[MenuIndexes[m_CurrentMenuIndex]]->SetColor({ 255, 0, 0 });
}

int wrap(int value, int min, int max) {
	return value < min ? max : value > max ? min : value;
}

void Menu::Scroll(int index) {
	index = wrap(index, 0, Options[m_CurrentMenuIndex].size()-1);
	MenuIndexes[m_CurrentMenuIndex] = index;
	for(int i = 0; i < Options[m_CurrentMenuIndex].size(); i++) {
		OptionsTextHuds[i]->SetColor({ 255, 255, 255 });
	}
	OptionsTextHuds[MenuIndexes[m_CurrentMenuIndex]]->SetColor({ 255, 0, 0 });
}
void Menu::ScrollPrev() {
	Scroll(MenuIndexes[m_CurrentMenuIndex] - 1);
}
void Menu::ScrollNext() {
	Scroll(MenuIndexes[m_CurrentMenuIndex] + 1);
}

void Menu::Invoke() {
	Options[m_CurrentMenuIndex][MenuIndexes[m_CurrentMenuIndex]].Call();
}

std::string ModMenu::Menu::CurrentMenuTitle() {
	if(m_CurrentMenuIndex != m_TopMenuIndex) {
		const int prev = PrevMenuIndexes[m_CurrentMenuIndex];
		return Options[prev][MenuIndexes[prev]].Text;
	}
	return MAIN_MENU_TEXT;
}

bool Menu::m_Open = false;
int Menu::m_CurrentMenuIndex = 0;
int Menu::m_InternalMenuIndex = 0;
int Menu::m_TopMenuIndex = 0;
std::vector<int> Menu::PrevMenuIndexes;
std::vector<int> Menu::MenuIndexes;
std::vector<std::vector<Option>> Menu::Options;
std::vector<HudElements::Text*> Menu::OptionsTextHuds;
std::map<std::string, HudElements::HudElement *> Menu::HudElements;
