//
//  Button.cpp
//
//  Created by Aleksey on 19.11.2020.
//

#include "Button.hpp"

#include <2d/CCSprite.h>
#include <2d/CCCamera.h>

#include <base/CCDirector.h>
#include <base/CCEventDispatcher.h>
#include <base/CCEventListenerTouch.h>
#include <base/CCEventCustom.h>

using namespace custom_ui;

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
        
        m_stateView.insert_or_assign(eButtonState::IDLE, sprite);
        
        m_stateView.try_emplace(eButtonState::PRESSED, sprite);
        m_stateView.try_emplace(eButtonState::DRAGOUT, sprite);

        addChild(sprite);
        
        setExpandZone(getContentSize());
        setSafeZone(getContentSize());
        
        sprite->setVisible(true);
        sprite->setAnchorPoint({0.5f, 0.5f});
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
        }
        
        m_stateView.insert_or_assign(eButtonState::PRESSED, sprite);
        addChild(sprite);
        
        sprite->setVisible(false);
        sprite->setAnchorPoint({0.5f, 0.5f});
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
        }
        
        m_stateView.insert_or_assign(eButtonState::DRAGOUT, sprite);
        addChild(sprite);
        
        sprite->setVisible(false);
        sprite->setAnchorPoint({0.5f, 0.5f});
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

bool Button::isPressed() const {
    return m_currentState == eButtonState::PRESSED;
}

Button::Button() noexcept
{
    setAnchorPoint({0.5f, 0.5f});
    
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    auto listener   = cocos2d::EventListenerTouchOneByOne::create();

    listener->onTouchBegan = CC_CALLBACK_2(Button::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(Button::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(Button::onTouchEnded, this);
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Button::sendEvent(ButtonEventData_t data)
{
    cocos2d::EventCustom event(data.event_name);
    event.setUserData(&data);
    
    _eventDispatcher->dispatchEvent(&event);
    
    if (m_callback) m_callback(this);
}

void Button::setState(eButtonState next_state)
{
    for (const auto& [state, node] : m_stateView)
        node->setVisible(state == next_state);
    
    m_currentState = next_state;
}

bool Button::touchTest(cocos2d::Rect rect, cocos2d::Point point) {
    return isScreenPointInRect(point, cocos2d::Camera::getVisitingCamera(), getWorldToNodeTransform(), rect, nullptr);
}

bool Button::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto touch_pos = touch->getLocation();
    
    cocos2d::Rect base_rect{getNormalNode()->getPosition() - getContentSize() / 2.f, getContentSize()};
    cocos2d::Rect expand_rect{getNormalNode()->getPosition() - m_expandZone / 2.f, m_expandZone};
    
    if (touchTest(base_rect, touch_pos) || touchTest(expand_rect, touch_pos))
    {
        setState(eButtonState::PRESSED);
        return true;
    }

    return false;
}

void Button::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto touch_pos = touch->getLocation();
    
    cocos2d::Rect base_rect{getNormalNode()->getPosition() - getContentSize() / 2.f, getContentSize()};
    cocos2d::Rect expand_rect{getNormalNode()->getPosition() - m_expandZone / 2.f, m_expandZone};
    cocos2d::Rect safe_rect{getNormalNode()->getPosition() - m_safeZone / 2.f, m_safeZone};
    
    if (touchTest(base_rect, touch_pos) || touchTest(expand_rect, touch_pos))
    {
        if (m_currentState != eButtonState::PRESSED)
            setState(eButtonState::PRESSED);
    }
    else if (touchTest(safe_rect, touch_pos))
    {
        if (m_currentState != eButtonState::DRAGOUT)
            setState(eButtonState::DRAGOUT);
    }
    else
    {
        setState(eButtonState::IDLE);
    }
}

void Button::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (isPressed())
        sendEvent({this, press_event_name});
    
    setState(eButtonState::IDLE);
}

cocos2d::Node* Button::getStateNode(eButtonState state) const
{
    if (auto it = m_stateView.find(state); it != m_stateView.end())
        return it->second;
    
    return nullptr;
}
