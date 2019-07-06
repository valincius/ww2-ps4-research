#pragma once

#include <vector>
#include <functional>

#include "HudElement.h"

namespace ModMenu {
	class Option {
		std::function<void()> m_function;

	public:
		Option(std::string text, std::function<void()> function = []() {});
		Option(std::string text, std::vector<Option> options);

		std::string Text;
		void Call();
	};

	class Menu {
		static bool m_Open;
		static int m_CurrentMenuIndex;
		static int m_InternalMenuIndex;
		static int m_TopMenuIndex;
	public:

		static std::vector<int> PrevMenuIndexes;
		static std::vector<int> MenuIndexes;
		static std::vector<std::vector<Option>> Options;
		static std::vector<HudElements::Text *> MenuTextHuds;

		static void Init(std::vector<Option> options);
		static bool IsOpen();
		static void SetOpenState(bool open);
		static void GoBack();

		static void SpawnHuds();

		static int AddMenu(std::vector<Option> options);
		static void ChangeMenu(int menuIndex);

		static void Scroll(int index);
		static void ScrollPrev();
		static void ScrollNext();

		static void Invoke();
	};
};
