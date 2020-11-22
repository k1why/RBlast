//
//  TestScene.cpp
//  test_proj
//
//  Created by Aleksey on 21.11.2020.
//

#include "TestScene.hpp"

#include <base/CCDirector.h>

#include <CustomUI/Button.hpp>
#include <CustomUI/LongPressButton.hpp>

#include <base/CCDirector.h>
#include <base/CCEventDispatcher.h>
#include <base/CCEventListenerCustom.h>
#include <base/CCEventCustom.h>

#include <2d/CCSprite.h>
#include <2d/CCLabel.h>

using namespace test_env;

constexpr auto event_label_text = "Last Event: ";

bool TestScene::init()
{
    if (!Scene::init())
        return false;
    
    setupUI();
    
    return true;
}

bool TestScene::setupUI()
{
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    constexpr float press_duration = 1.f;
    
    if ((m_button = custom_ui::Button::create("btn/idle.png", "btn/pressed.png", "btn/dragout.png")))
    {
        m_button->setPosition({m_button->getContentSize().width * 2.f, visibleSize.height / 2.f});
        m_button->setExpandZone(m_button->getContentSize() * 1.5f);
        m_button->setSafeZone(m_button->getContentSize() * 2.5f);
        m_button->setPressedCallback([](auto btn){
            btn->setRotation(btn->getRotation() + 45.f);
            
            auto scale = btn->getScale();
            if (scale > 1.f) {
                btn->setScale(1.f);
            } else if (scale < 1.f) {
                btn->setScale(1.3f);
            } else {
                btn->setScale(0.7f);
            }
        });
        
        if (auto pressed_label = cocos2d::Label::create())
        {
            m_button->getPressedNode()->addChild(pressed_label);
            
            pressed_label->setTextColor(cocos2d::Color4B::BLACK);
            pressed_label->setString("Pressed");
            pressed_label->setAnchorPoint({0.5f, 0.5f});
            pressed_label->setPosition(m_button->getContentSize() / 2.f);
        }
        
        addChild(m_button);
    }
    else
    {
        return false;
    }
    
    if ((m_longButton = custom_ui::LongPressButton::create("btn/idle.png", "btn/pressed.png", "btn/dragout.png", press_duration)))
    {
        m_longButton->setPosition({visibleSize.width - m_longButton->getContentSize().width * 2.f, visibleSize.height / 2.f});
        m_longButton->setSafeZone(m_button->getContentSize() * 2.f);
        addChild(m_longButton);
    }
    else
    {
        return false;
    }
    
    if ((m_eventLabel = cocos2d::Label::create()))
    {
        m_eventLabel->setAnchorPoint({0.f, 1.f});
        m_eventLabel->setString(event_label_text);
        m_eventLabel->setPosition({m_eventLabel->getContentSize().width, visibleSize.height - m_eventLabel->getContentSize().height});
        m_eventLabel->setSystemFontSize(20.f);
        
        addChild(m_eventLabel);
    }
    else
    {
        return false;
    }
    
    
    auto listiner_press = cocos2d::EventListenerCustom::create(custom_ui::press_event_name, [=](cocos2d::EventCustom* event){
        auto event_data = static_cast<custom_ui::ButtonEventData_t*>(event->getUserData());
        
        if (m_eventLabel && event_data)
            m_eventLabel->setString(event_label_text + event_data->event_name);
    });
    
    auto listiner_long_press = cocos2d::EventListenerCustom::create(custom_ui::long_press_event_name, [=](cocos2d::EventCustom* event){
        auto event_data = static_cast<custom_ui::ButtonEventData_t*>(event->getUserData());
        
        if (m_eventLabel && event_data)
            m_eventLabel->setString(event_label_text + event_data->event_name);
        
        if (event_data && event_data->sender)
        {
            auto scale = event_data->sender->getScale();
            if (scale > 1.8f)
            {
                event_data->sender->setScale(0.6f);
            }
            else
            {
                event_data->sender->setScale(scale + 0.2f);
            }
        }
    });
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listiner_press, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listiner_long_press, this);
    
    return true;
}

bool TestScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return true;
}

void TestScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{

}
