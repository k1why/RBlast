//
//  LongPressButton.hpp
//
//  Created by Aleksey on 19.11.2020.
//

#pragma once

#include "Button.hpp"

namespace cocos2d
{
    class Action;
}

namespace custom_ui
{
    constexpr auto long_press_event_name = "button_long_pressed_event";
    constexpr float default_duration = 2.f;

    class LongPressButton : public Button
    {
    public:
        static LongPressButton* create(float duration = default_duration);
        static LongPressButton* create(const std::string& normal, const std::string& pressed, const std::string& dragout, float duration);
        static LongPressButton* create(const std::string& normal, const std::string& pressed, const std::string& dragout, const buttonCallback& callback, float duration);
        
        void  setPressDuration(float duration);
        float getPressDuration() const;
        
    protected:
        explicit LongPressButton(float duration) noexcept;
        
        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
        
    private:
        float m_pressDuration;
        bool  m_isLongTouch;
        
        cocos2d::Action* m_longPressAction;
    };
}
