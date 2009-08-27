//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Marianne Gagnon
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


#ifndef HEADER_WIDGET_HPP
#define HEADER_WIDGET_HPP

#include <irrlicht.h>
#include <map>

#include "guiengine/skin.hpp"
#include "utils/ptr_vector.hpp"
#include "utils/vec3.hpp"

using namespace irr;
using namespace gui;

namespace GUIEngine
{
    
    class RibbonGridWidget;
    
    enum WidgetType
    {
        WTYPE_NONE = -1,
        WTYPE_RIBBON,
        WTYPE_SPINNER,
        WTYPE_BUTTON,
        WTYPE_ICON_BUTTON,
        WTYPE_CHECKBOX,
        WTYPE_LABEL,
        WTYPE_SPACER,
        WTYPE_DIV,
        WTYPE_RIBBON_GRID,
        WTYPE_MODEL_VIEW,
        WTYPE_LIST,
        WTYPE_TEXTBOX
    };
    
    enum Property
    {
        PROP_ID = 100,
        PROP_PROPORTION,
        PROP_WIDTH,
        PROP_HEIGHT,
        PROP_CHILD_WIDTH,
        PROP_CHILD_HEIGHT,
        PROP_WORD_WRAP,
        PROP_GROW_WITH_TEXT, // yet unused
        PROP_X,
        PROP_Y,
        PROP_LAYOUT,
        PROP_ALIGN,
        PROP_TEXT,
        PROP_ICON,
        PROP_TEXT_ALIGN,
        PROP_MIN_VALUE,
        PROP_MAX_VALUE,
        PROP_MAX_WIDTH,
        PROP_MAX_HEIGHT,
        PROP_SQUARE
    };
    
    static bool isWithinATextBox = false;
    
    /**
      * The nearly-abstract base of all widgets (not fully abstract since a bare Widget
      * can be created for the sore goal of containing children widgets in a group)
      *
      * Provides basic common functionnality, as well as providing a few callbacks
      * for children to override if they need to do something special on event.
      *
      * Each widget may have an irrlicht parent (most often used to put widgets in dialogs)
      * and also optionally one or many children.
      *
      * Each widget also has a set of properties stored in a ma (see enum above)
      */
    class Widget : public SkinWidgetContainer
    {
    protected:
        friend class EventHandler;
        friend class RibbonWidget;
        friend class Screen;
        friend class SpinnerWidget;
        friend class Skin;
        friend class RibbonGridWidget;
        
        /**
          * These methods provide new unique IDs each time you call them.
          * Since IDs are used to determine tabbing order, "non-tabbable"
          * objects are being given very different IDs so that they don't interfere.
          */
        int getNewID();
        int getNewNoFocusID();
        
        /**
          * Can be used in children to indicate whether a widget is selected or not
          * - in widgets where it makes sense (e.g. ribbon children) and where the
          * irrLicht widget can not directly contain this state
          */
        bool m_selected;
        
        /**
          * called when left/right keys pressed and focus is on widget. 
          * Returns 'true' if main event handler should be notified of a change.
          * Override in children to be notified of left/right events.
          */
        virtual bool rightPressed() { return false; }
        virtual bool leftPressed() { return false; }
        
        /** used when you set eventSupervisors - see m_event_handler explainations below
            called when one of a widget's children is hovered.
            Returns 'true' if main event handler should be notified of a change. */
        virtual bool mouseHovered(Widget* child) { return false; }
        
        /** override in children if you need to know when the widget is focused */
        virtual void focused() { isWithinATextBox = false; }
        
        /**
          * The XML loader stored coords in their raw string form inside this widget.
          * This method parses the strings. Most notably, expands coords relative to parent
          * and calculates percentages.
          */
        void readCoords(Widget* parent=NULL);
        
        /**
         * An irrlicht parent (most often used to put widgets in dialogs)
         */
        IGUIElement* m_parent;
        
        /**
         * Receives as string the raw property value retrieved from XML file.
         * Will try to make sense of it, as an absolute value or a percentage.
         *
         * Return values :
         *     Will write to either absolute or percentage, depending on the case.
         *     Returns false if couldn't convert to either
         */
        static bool convertToCoord(std::string& x, int* absolute, int* percentage);
        
        /**
         * IrrLicht widget created to represent this object.
         */
        IGUIElement* m_element;
        
        
        // FIXME... i forgot the m_ everywhere ... XD

        /** numerical ID used by irrLicht to identify this widget
         * (not the same as the string identificator specified in the XML file)
         */
        int id;

    public:
        /**
         * This is set to NULL by default; set to something else in a widget to mean
         * that events happening on this widget should also be passed to m_event_handler->transmitEvent,
         * which is usually the parent analysing events from its children.
         * This is especially useful with logical widgets built with more than
         * one irrlicht widgets (e.g. Spinner, Ribbon)
         */
        Widget* m_event_handler;
        
        
        /** Coordinates of the widget */
        int x, y, w, h;
        
        /** Whether to show a bounding box around this widget (used for sections) */
        bool m_show_bounding_box;
        
        /** Used in two cases :
            1) For 'placeholder' divisions; at the time the layout is created, there is nothing to
               place there yet, but we know there eventually will. So in this case pass 'true' to the
               Widget constructor and it will reserve a widget ID and store it here.
            2) Theorically, in 'add()', derived widgets should checked if this value is set, and use
               it instead of creating a new ID if it is. In practice, it's not widely implemented (FIXME) */
        int m_reserved_id;
        
        Widget(bool reserve_id = false);
        virtual ~Widget() {}
        
        /**
          * Get the underlying irrLicht GUI element, casted to the right type.
          */
        template<typename T> T* getIrrlichtElement()
        {
            #if defined(WIN32) || defined(NDEBUG)
                return static_cast<T*>(m_element);
            #else
                T* out = dynamic_cast<T*>(m_element);
                return out;
            #endif
        }

        template<typename T> const T* getIrrlichtElement() const
        {
            #if defined(WIN32) || defined(NDEBUG)
                return static_cast<T*>(m_element);
            #else
                T* out = dynamic_cast<T*>(m_element);
                return out;
            #endif
        }

        IGUIElement* getIrrlichtElement() { return m_element; }

        void setParent(IGUIElement* parent);
        
        /**
          * If this widget has any children, they go here. Children can be either
          * specified in the XML file (e.g. Ribbon or Div children), or can also
          * be created automatically for logical widgets built with more than
          * one irrlicht widgets (e.g. Spinner)
          */
        ptr_vector<Widget> m_children;
        
        /** Type of this widget */
        WidgetType m_type;
        
        /** A map that holds values for all specified widget properties (in the XML file)*/
        std::map<Property, std::string> m_properties;
        
        static void resetIDCounters();
        
        /**
          * Call to resize/move the widget. Not all widgets can resize gracefully.
          */
        virtual void move(const int x, const int y, const int w, const int h);
        
        
        bool isSelected() const { return m_selected; }
        
        
        /**
         * Override in children to possibly receive updates (you may need to register to
         * them first)
         */
        virtual void update(float delta) { }
        
        /** All widgets, including their parents (m_event_handler) will be notified on event through
         this call. Must return whether main (GUI engine user) event callback should be notified or not.
         Note that in the case of a hierarchy of widgets (with m_event_handler), only the topmost widget
         of the chain decides whether the main handler is notified; return value is not read for others. */
        virtual bool transmitEvent(Widget* w, std::string& originator) { return true; }
        
        /**
         * Create and add the irrLicht widget(s) associated with this object.
         * Call after Widget was read from XML file and laid out.
         */
        virtual void add() {};
    };

    
}
#endif
