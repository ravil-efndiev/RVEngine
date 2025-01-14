#pragma once

#include "Event.hpp"

namespace Rvl
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height);

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        static EventType GetTypeStatic();

        EventCategory GetCategory() const override;
        EventType GetType() const override;

    private:
        uint32_t _width, _height;
    
    };
} 


