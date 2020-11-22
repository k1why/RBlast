//
//  LongPressButton.cpp
//
//  Created by Aleksey on 19.11.2020.
//

#include "LongPressButton.hpp"

#include <base/CCDirector.h>
#include <base/CCEventCustom.h>
#include <base/CCEventDispatcher.h>

#include <2d/CCActionInterval.h>
#include <2d/CCActionInstant.h>


using namespace custom_ui;

LongPressButton* LongPressButton::create(float duration)
{
    if (auto button = new (std::nothrow) LongPressButton(duration))
    {
        button->autorelease();
        return button;
    }

    return nullptr;
}

LongPressButton* LongPressButton::create(const std::string& normal, const std::string& pressed, const std::string& dragout, float duration)
{
    auto button = LongPressButton::create(duration);
    
    if (button != nullptr)
        button->setNormalImage(normal)->setPressedImage(pressed)->setDragoutImage(dragout);

    return button;
}

LongPressButton* LongPressButton::create(const std::string& normal, const std::string& pressed, const std::string& dragout, const buttonCallback& callback, float duration)
{
    auto button = LongPressButton::create(normal, pressed, dragout, duration);
    
    if (button != nullptr)
        button->setPressedCallback(callback);
    
    return button;
}

void LongPressButton::setPressDuration(float duration) {
    m_pressDuration = duration;
}

float LongPressButton::getPressDuration() const {
    return m_pressDuration;
}

LongPressButton::LongPressButton(float duration) noexcept
    : Button()
    , m_pressDuration(duration)
    , m_isLongTouch(false)
{}

bool LongPressButton::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    bool touched = Button::onTouchBegan(touch, event);
    if (touched == true)
    {
        m_longPressAction = this->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(m_pressDuration), cocos2d::CallFunc::create([this](){
            m_isLongTouch = true;
        }), NULL));
    }
    
    return touched;
}

void LongPressButton::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    stopAction(m_longPressAction);
    m_longPressAction = nullptr;
    
    if (isPressed())
        sendEvent({this, m_isLongTouch ? long_press_event_name : press_event_name});
    
    setState(eButtonState::IDLE);
    m_isLongTouch = false;
}

