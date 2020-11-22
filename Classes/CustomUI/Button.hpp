//
//  Button.hpp
//
//  Created by Aleksey on 19.11.2020.
//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <functional>

#include <2d/CCNode.h>

#include <math/CCGeometry.h>

namespace cocos2d {
    class Sprite;
    class EventCustom;
}

namespace custom_ui
{
    constexpr auto press_event_name = "button_pressed_event";

    using buttonCallback = std::function<void(class Button*)>;

    struct ButtonEventData_t
    {
        Button* sender;
        std::string event_name;
    };

    class Button : public cocos2d::Node
    {
    public:
        static Button* create();
        static Button* create(const std::string& normal, const std::string& pressed, const std::string& dragout);
        static Button* create(const std::string& normal, const std::string& pressed, const std::string& dragout, const buttonCallback& callback);
        
        virtual Button* setNormalImage (const std::string& file);
        virtual Button* setPressedImage(const std::string& file);
        virtual Button* setDragoutImage(const std::string& file);
        
        virtual Button* setPressedCallback(buttonCallback callback);
        
        void setSafeZone  (cocos2d::Size zone);
        void setExpandZone(cocos2d::Size zone);
        
        virtual const cocos2d::Size& getContentSize() const override;
        
        cocos2d::Node* getNormalNode () const;
        cocos2d::Node* getPressedNode() const;
        cocos2d::Node* getDragoutNode() const;
        
        bool isPressed() const;
        
        virtual ~Button() = default;
        
    protected:
        enum class eButtonState {
            IDLE    = 0,
            PRESSED = 1,
            DRAGOUT = 2,
        };
        
        explicit Button() noexcept;
        
        void sendEvent(ButtonEventData_t data);
        void setState(eButtonState next_state);
        bool touchTest(cocos2d::Rect rect, cocos2d::Point point);
    
        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
        
        cocos2d::Node* getStateNode(eButtonState state) const;
        
    protected:
        std::map<eButtonState, cocos2d::Sprite*> m_stateView;
        
        eButtonState m_currentState;
        
        cocos2d::Size m_safeZone;
        cocos2d::Size m_expandZone;
    
        buttonCallback m_callback;
    };
}

