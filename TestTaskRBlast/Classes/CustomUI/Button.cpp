//
//  Button.cpp
//
//  Created by Aleksey on 19.11.2020.
//

#include "Button.hpp"

#include <2d/CCSprite.h>

#include <base/CCDirector.h>
#include <base/CCEventDispatcher.h>
#include <base/CCEventListenerTouch.h>
#include <base/CCEventCustom.h>

using namespace custom_ui;

constexpr auto press_event_name = "button_pressed_event";

namespace common
{
    bool checkRectAndPoint(cocos2d::Vec2 rect_pos, cocos2d::Size rect_size, cocos2d::Point point)
    {
        cocos2d::Rect rect{{rect_pos - (rect_size / 2.f)}, rect_size};
        return rect.containsPoint(point);
    }
}

Button* Button::create()
{
    if (auto button = new (std::nothrow) Button)
    {
        button->autorelease();
        return button;
    }

    return nullptr;
}

Button* Button::create(const std::string& normal, const std::string& pressed, const std::string& dragout)
{
    if (auto button = Button::create())
        return button->setNormalImage(normal)->setPressedImage(pressed)->setDragoutImage(dragout);
    
    return nullptr;
}

Button* Button::create(const std::string& normal, const std::string& pressed, const std::string& dragout, const buttonCallback& callback)
{
    if (auto button = Button::create(normal, pressed, dragout))
        return button->setPressedCallback(callback);
    
    return nullptr;
}

Button* Button::setNormalImage(const std::string& file)
{
    if (auto sprite = cocos2d::Sprite::create(file))
    {
        if (auto old = m_stateView.find(eButtonState::IDLE); old != m_stateView.end())
            this->removeChild(old->second);
        
        m_stateView[eButtonState::IDLE] = sprite;
        addChild(sprite);
        
        if (m_stateView.find(eButtonState::PRESSED) == m_stateView.end())
            m_stateView[eButtonState::PRESSED] = sprite;
        
        if (m_stateView.find(eButtonState::DRAGOUT) == m_stateView.end())
            m_stateView[eButtonState::DRAGOUT] = sprite;
        
        setExpandZone(getContentSize());
        setSafeZone(getContentSize());
    }
    
    return this;
}

Button* Button::setPressedImage(const std::string& file)
{
    if (auto sprite = cocos2d::Sprite::create(file))
    {
        if (auto old_idle = m_stateView.find(eButtonState::IDLE); old_idle != m_stateView.end())
        {
            if (auto old = m_stateView.find(eButtonState::PRESSED); old != m_stateView.end())
            {
                if (old->second != old_idle->second)
                    this->removeChild(old->second);
            }
            
            m_stateView.erase(eButtonState::PRESSED);
        }
        
        m_stateView[eButtonState::PRESSED] = sprite;
        addChild(sprite);
        
        sprite->setVisible(false);
    }
        
    return this;
}

Button* Button::setDragoutImage(const std::string& file)
{
    if (auto sprite = cocos2d::Sprite::create(file))
    {
        if (auto old_idle = m_stateView.find(eButtonState::IDLE); old_idle != m_stateView.end())
        {
            if (auto old = m_stateView.find(eButtonState::DRAGOUT); old != m_stateView.end())
            {
                if (old->second != old_idle->second)
                    this->removeChild(old->second);
            }
            
            m_stateView.erase(eButtonState::DRAGOUT);
        }
        
        m_stateView[eButtonState::DRAGOUT] = sprite;
        addChild(sprite);
        
        sprite->setVisible(false);
    }
        
    return this;
}

Button* Button::setPressedCallback(buttonCallback callback) {
    m_callback = callback;
    
    return this;
}

void Button::setSafeZone(cocos2d::Size zone) {
    auto content_size = getContentSize();
    
    m_safeZone.width  = content_size.width  > zone.width  ? content_size.width  : zone.width;
    m_safeZone.height = content_size.height > zone.height ? content_size.height : zone.height;
    
    return this;
}

void Button::setExpandZone(cocos2d::Size zone) {
    auto content_size = getContentSize();
    
    m_expandZone.width  = content_size.width  > zone.width  ? content_size.width  : zone.width;
    m_expandZone.height = content_size.height > zone.height ? content_size.height : zone.height;
    
    return this;
}

const cocos2d::Size& Button::getContentSize() const
{
    if (auto it = m_stateView.find(eButtonState::IDLE); it != m_stateView.end())
        return it->second->getContentSize();
    
    return cocos2d::Node::getContentSize();
}

cocos2d::Node* Button::getNormalNode() const {
    return getStateNode(eButtonState::IDLE);
}

cocos2d::Node* Button::getPressedNode() const {
    return getStateNode(eButtonState::PRESSED);
}

cocos2d::Node* Button::getDragoutNode() const {
    return getStateNode(eButtonState::DRAGOUT);
}

cocos2d::Size Button::getSafeZone() const {
    return m_safeZone;
}

cocos2d::Size Button::getExpandZone() const {
    return m_expandZone;
}

bool Button::isButtonPressed() const {
    return m_currentState == eButtonState::PRESSED;
}

bool Button::isButtonDragout() const {
    return m_currentState == eButtonState::DRAGOUT;
}

Button::Button() noexcept
{
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    auto listener   = cocos2d::EventListenerTouchOneByOne::create();

    listener->onTouchBegan = CC_CALLBACK_2(Button::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(Button::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(Button::onTouchEnded, this);
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Button::onPressed()
{
    if (getTouchAccepted() == false)
        return;
    
    ButtonEventData_t data;
    data.sender = this;
    
    cocos2d::EventCustom event(getPressedEventName());
    event.setUserData(&data);
    
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    
    if (m_callback) m_callback(this);
}

void Button::switchState(eButtonState next_state)
{
    for (const auto& [state, node] : m_stateView)
        node->setVisible(state == next_state);
}

bool Button::getTouchAccepted() const
{
    return true;
}

const std::string Button::getPressedEventName() const
{
    return press_event_name;
}

bool Button::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto touch_pos = touch->getLocation();
    
    auto content_size = getContentSize();
    auto position     = getPosition();
    
    if (common::checkRectAndPoint(position, content_size, touch_pos) ||
        common::checkRectAndPoint(position, m_expandZone, touch_pos))
    {
        switchState(eButtonState::PRESSED);
        return true;
    }
    
    return false;
}

void Button::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto touch_pos = touch->getLocation();
    
    auto content_size = getContentSize();
    auto position     = getPosition();
    
    if (common::checkRectAndPoint(position, content_size, touch_pos) ||
        common::checkRectAndPoint(position, m_expandZone, touch_pos))
    {
        if (m_currentState != eButtonState::PRESSED)
            switchState(eButtonState::PRESSED);
    }
    else if (common::checkRectAndPoint(position, m_safeZone, touch_pos))
    {
        if (m_currentState != eButtonState::DRAGOUT)
            switchState(eButtonState::DRAGOUT);
    }
}

void Button::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto touch_pos = touch->getLocation();
    
    auto content_size = getContentSize();
    auto position     = getPosition();
    
    if (common::checkRectAndPoint(position, content_size, touch_pos) ||
        common::checkRectAndPoint(position, m_expandZone, touch_pos))
    {
        onPressed();
    }
    
    switchState(eButtonState::IDLE);
}

void Button::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{}

cocos2d::Node* Button::getStateNode(eButtonState state) const
{
    if (auto it = m_stateView.find(state); it != m_stateView.end())
        return it->second;
    
    return nullptr;
}
