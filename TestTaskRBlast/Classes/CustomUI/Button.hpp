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
}

namespace custom_ui
{
    using buttonCallback = std::function<void(cocos2d::Ref*)>;

    class Button;
    struct ButtonEventData_t
    {
        Button* sender;
        
    };

    class Button : public cocos2d::Node
    {
        enum class eButtonState {
            IDLE    = 0,
            PRESSED = 1,
            DRAGOUT = 2,
        };
        
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
        
        virtual const std::string    getPressedEventName() const;
        virtual const cocos2d::Size& getContentSize() const override;
        
        cocos2d::Node* getNormalNode () const;
        cocos2d::Node* getPressedNode() const;
        cocos2d::Node* getDragoutNode() const;
        
        cocos2d::Size getSafeZone  () const;
        cocos2d::Size getExpandZone() const;
        
        bool isButtonPressed() const;
        bool isButtonDragout() const;
        
        virtual ~Button() = default;
        
    protected:
        explicit Button() noexcept;
        
        void onPressed();
        void switchState(eButtonState next_state);
        
        virtual bool getTouchAccepted() const;
        
        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
        
        virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
        
    private:
        cocos2d::Node* getStateNode(eButtonState state) const;
        
    private:
        std::map<eButtonState, cocos2d::Sprite*> m_stateView;
        
        eButtonState m_currentState;
        
        cocos2d::Size m_safeZone;
        cocos2d::Size m_expandZone;
    
        std::function<void(cocos2d::Ref*)> m_callback;
    };
}

