//
//  TestScene.hpp
//  test_proj
//
//  Created by Aleksey on 21.11.2020.
//

#pragma once

#include <2d/CCScene.h>

namespace cocos2d
{
    class Label;
    class ProgressBar;
}

namespace custom_ui
{
    class Button;
    class LongPressButton;
}


namespace test_env
{
    class TestScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(TestScene);
        
        virtual bool init() override;
        
    private:
        bool setupUI();
        
    private:
        cocos2d::Label* m_eventLabel;
        
        custom_ui::Button* m_button;
        custom_ui::LongPressButton* m_longButton;
    };
}
