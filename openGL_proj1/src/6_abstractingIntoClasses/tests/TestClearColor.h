#pragma once

#include "Test.h"

namespace test
{
	class TestClearColor : public Test
	{
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ImGuiTimesColor() override;
		float m_TimesColor[4];

	private:
		float m_ClearColor[4];
		
	};
}
