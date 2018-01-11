//------------------------------------------------------------------------------
// <copyright file="UIElement.h" company="Microsoft">
//     Base class for 2D user interface elements
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <collection.h>

using namespace WindowsPreview::Kinect::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

static const Windows::UI::Color ColorValue[] =
{
    {255, 209, 211, 212}, //Light Gray
    {255, 147, 149, 152}, //Medium Gray
    {255, 65, 64, 66}, //Dark Gray
};

enum class Color
{
    First = 0,
    LightGray = First,
    MediumGray,
    DarkGray,
    MaxCount
};

interface class IRenderer
{
    virtual void RequestRedraw() = 0;
    virtual Size ViewSize() = 0;
};

ref class UIElement abstract
{
internal:
    UIElement(UIElement^ parent);

public:
    virtual ~UIElement();

    UIElement^ Parent() { return m_parent; }

    // Manipulation update calculations.
    // Update manipulation transform, given manipulation delta in parent coordinate system.
    void UpdateManipulationTransform(Point delta);

    virtual property Microsoft::Kinect::Toolkit::Input::IInputModel^ InputModel
    {
        Microsoft::Kinect::Toolkit::Input::IInputModel^ get() = 0;
    }

    property Windows::Foundation::Collections::IVector<UIElement^>^ Children
    {
        Windows::Foundation::Collections::IVector<UIElement^>^ get()
        {
            return m_children;
        }
    }

    // Returns true if point (x,y) belongs to the object. The point should be in parent coordinate system.
    virtual bool HitTest(Windows::Foundation::Point position) = 0;
    
    virtual bool UIElement::GetElementsAtPoint(Windows::Foundation::Point position, Windows::Foundation::Collections::IVector<UIElement^>^ elementsAtPoint);

internal:
    // Local to client(global) coordinate system transform
    // This transform includes all transforms in the object parent chain
    virtual const XMMATRIX Transform() const { return LocalTransform() * ParentTransform(); }

    // Components of the transform.
    // Local transform, transforms from this object's intrinsic coordinate system to parent coordinate system.
    const XMMATRIX LocalTransform() const { return XMMatrixTranslation(m_ptInit.x, m_ptInit.y, 0.0f) * m_mxManip; }
    // Parent transform, transforms from parent coordinate system to client(global) coordinate system.
    const XMMATRIX ParentTransform() const  { return m_parent ? m_parent->Transform() : XMMatrixIdentity(); }

    // Components of the local transform.
    void SetInitialPosition(XMFLOAT2 ptInit) { m_ptInit = ptInit; }
    void SetManipulationTransform(CXMMATRIX mxManip) { m_mxManip = mxManip; }

private:
    UIElement^ m_parent; // parent object
    XMMATRIX m_mxManip;  // manipulation transform
    XMFLOAT2 m_ptInit;   // initial position
    Platform::Collections::Vector<UIElement^>^ m_children;
};

ref class UIElementTransform sealed : public Windows::UI::Input::IPointerPointTransform
{
public:
    UIElementTransform(UIElement^ referentObject);

    virtual bool TryTransform(Windows::Foundation::Point inPoint, Windows::Foundation::Point* outPoint);
    virtual Windows::Foundation::Rect TransformBounds(Windows::Foundation::Rect rect);
	virtual property Windows::UI::Input::IPointerPointTransform^ Inverse { Windows::UI::Input::IPointerPointTransform^ get(); }

private:
    void SetMatrix(CXMMATRIX matrix) { m_matrix = matrix; }

    XMMATRIX m_matrix; // client (global) to object parent coordinate system transform
};

template <class T>
ref class MapHelpers
{
public:
    //--------------------------------------------------------------------------------------
    // Name: SafeLookop()
    // Desc: Safely looks up a map entry, returning null if the key does not exist.
    //--------------------------------------------------------------------------------------
    static T SafeLookup(IMap<uint32, UIElement^>^ elementsMap, uint32 pointerId)
    {
        return elementsMap->HasKey(pointerId) ? dynamic_cast<T>(elementsMap->Lookup(pointerId)) : nullptr;
    }
};
