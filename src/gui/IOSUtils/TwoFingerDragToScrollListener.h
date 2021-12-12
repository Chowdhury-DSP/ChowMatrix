#pragma once

#if JUCE_IOS

#include <pch.h>

using ViewportTwoFingerDragPosition = AnimatedPosition<AnimatedPositionBehaviours::ContinuousWithMomentum>;

struct TwoFingerDragToScrollListener : private MouseListener,
                                       private ViewportTwoFingerDragPosition::Listener
{
    TwoFingerDragToScrollListener (Viewport& v) : viewport (v)
    {
        getContentHolder().addMouseListener (this, true);
        offsetX.addListener (this);
        offsetY.addListener (this);
        offsetX.behaviour.setMinimumVelocity (0.1);
        offsetY.behaviour.setMinimumVelocity (0.1);
        offsetX.behaviour.setFriction (0.1);
        offsetY.behaviour.setFriction (0.1);
    }

    ~TwoFingerDragToScrollListener() override
    {
        getContentHolder().removeMouseListener (this);
        Desktop::getInstance().removeGlobalMouseListener (this);
    }

    void positionChanged (ViewportTwoFingerDragPosition&, double) override
    {
        viewport.setViewPosition (originalViewPos - Point<int> ((int) offsetX.getPosition(), (int) offsetY.getPosition()));
    }

    void mouseDown (const MouseEvent&) override
    {
        if (! isGlobalMouseListener)
        {
            offsetX.setPosition (offsetX.getPosition());
            offsetY.setPosition (offsetY.getPosition());

            // switch to a global mouse listener so we still receive mouseUp events
            // if the original event component is deleted
            getContentHolder().removeMouseListener (this);
            Desktop::getInstance().addGlobalMouseListener (this);

            isGlobalMouseListener = true;
        }
    }

    void mouseDrag (const MouseEvent& e) override
    {
        if (Desktop::getInstance().getNumDraggingMouseSources() != 2 || doesMouseEventComponentBlockViewportDrag (e.eventComponent))
            return;
        
        const auto* dragSource0 = Desktop::getInstance().getDraggingMouseSource (0);
        const auto* dragSource1 = Desktop::getInstance().getDraggingMouseSource (1);
        
        if (dragSource0 == nullptr || dragSource1 == nullptr)
        {
            // But the Desktop just told us that 2 drag sources existed... something is terribly wrong!
            jassertfalse;
            return;
        }
        
        bool haveDragSourcesMoved = dragSource0->hasMovedSignificantlySincePressed() && dragSource1->hasMovedSignificantlySincePressed();
        auto dragSource0Offset = dragSource0->getScreenPosition() - dragSource0->getLastMouseDownPosition();
        auto dragSource1Offset = dragSource1->getScreenPosition() - dragSource1->getLastMouseDownPosition();
        auto averageOffset = (dragSource0Offset + dragSource1Offset) / 2.0f;

        if (! isDragging && haveDragSourcesMoved)
        {
            isDragging = true;

            originalViewPos = viewport.getViewPosition();
            offsetX.setPosition (0.0);
            offsetX.beginDrag();
            offsetY.setPosition (0.0);
            offsetY.beginDrag();
        }

        if (isDragging)
        {
            offsetX.drag (averageOffset.x);
            offsetY.drag (averageOffset.y);
        }
    }

    void mouseUp (const MouseEvent&) override
    {
        if (isGlobalMouseListener && Desktop::getInstance().getNumDraggingMouseSources() != 2)
            endDragAndClearGlobalMouseListener();
    }

    void endDragAndClearGlobalMouseListener()
    {
        offsetX.endDrag();
        offsetY.endDrag();
        isDragging = false;

        getContentHolder().addMouseListener (this, true);
        Desktop::getInstance().removeGlobalMouseListener (this);

        isGlobalMouseListener = false;
    }

    bool doesMouseEventComponentBlockViewportDrag (const Component* eventComp)
    {
        for (auto c = eventComp; c != nullptr && c != &viewport; c = c->getParentComponent())
            if (c->getViewportIgnoreDragFlag())
                return true;

        return false;
    }

    Component& getContentHolder()
    {
        auto* viewedComp = viewport.getViewedComponent();
        jassert (viewedComp != nullptr); // Viewport must own a component before creating a drag-to-scroll listener

        auto* contentHolder = viewedComp->getParentComponent();
        jassert (contentHolder != nullptr); // This should never happen unless JUCE changes how Viewport works!

        return *contentHolder;
    }

    Viewport& viewport;
    ViewportTwoFingerDragPosition offsetX, offsetY;
    Point<int> originalViewPos;
    bool isDragging = false;
    bool isGlobalMouseListener = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoFingerDragToScrollListener)
};

#endif // JUCE_IOS
