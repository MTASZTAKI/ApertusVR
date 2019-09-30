//------------------------------------------------------------------------------
// <copyright file="UIElement.cpp" company="Microsoft">
//     Base class for 2D user interface elements
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include <collection.h>
#include "UIElement.h"

using namespace Platform::Collections;
using namespace Windows::Foundation;

//--------------------------------------------------------------------------------------
// Name: UIElement()
// Desc: Constructor
//--------------------------------------------------------------------------------------
UIElement::UIElement(UIElement^ parent) :
    m_parent(parent),
    m_mxManip(XMMatrixIdentity()),
    m_ptInit(XMFLOAT2(0.0f, 0.0f))
{
}

UIElement::~UIElement()
{
}

bool UIElement::GetElementsAtPoint(Windows::Foundation::Point position, Windows::Foundation::Collections::IVector<UIElement^>^ elementsAtPoint)
{
    if (HitTest(position))
    {
        elementsAtPoint->Append(this);
    }

    for (auto child : Children)
    {
        child->GetElementsAtPoint(position, elementsAtPoint);
    }

    return (elementsAtPoint->Size > 0);
}

//--------------------------------------------------------------------------------------
// Name: UpdateManipulationTransform()
// Desc: Manipulation update calculations
//--------------------------------------------------------------------------------------
void UIElement::UpdateManipulationTransform(Point delta)
{
    // Delta is in parent's coordinate system
    XMMATRIX mxDelta = XMMatrixTranslation(-delta.X, -delta.Y, 0.0f);
    m_mxManip = m_mxManip * mxDelta;
}

//--------------------------------------------------------------------------------------
// Name: UIElementTransform()
// Desc: Constructor
//--------------------------------------------------------------------------------------
UIElementTransform::UIElementTransform(UIElement^ referentObject)
{
    if (nullptr != referentObject)
    {
        m_matrix = referentObject->Transform();
    }
    else
    {
        m_matrix = XMMatrixIdentity();
    }

    // m_matrix is IPointerPointTransform transform "input" transform: from client to local coordinate system
    // Transform matrix used by UIElement is "output" transform: from local to client coordinate system
    XMVECTOR determinant;
    m_matrix = XMMatrixInverse(&determinant, m_matrix);
}

//--------------------------------------------------------------------------------------
// Name: TryTransform()
// Desc: Attempts to perform the transformation on the specified input point
//--------------------------------------------------------------------------------------
bool UIElementTransform::TryTransform(Windows::Foundation::Point inPoint, Windows::Foundation::Point* outPoint)
{
    XMVECTOR pt = XMVectorSet(inPoint.X, inPoint.Y, 0.0f, 0.0f);
    pt = XMVector3Transform(pt, m_matrix);

    outPoint->X = XMVectorGetX(pt);
    outPoint->Y = XMVectorGetY(pt);
    return true;
}

//--------------------------------------------------------------------------------------
// Name: TransformBounds()
// Desc: Transforms the specified bounding rectangle
//--------------------------------------------------------------------------------------
Windows::Foundation::Rect UIElementTransform::TransformBounds(Windows::Foundation::Rect rect)
{
    Windows::Foundation::Point center(rect.X + rect.Width/2, rect.Y + rect.Height/2);

    XMVECTOR determinant = XMMatrixDeterminant(m_matrix);

    float scale = sqrt(fabs(XMVectorGetX(determinant)));

    if (TryTransform(center, &center) && (scale > 0))
    {
        rect.Width *= scale;
        rect.Height *= scale;
        rect.X = center.X - rect.Width/2;
        rect.Y = center.Y - rect.Height/2;
    }
    return rect;
}

//--------------------------------------------------------------------------------------
// Name: Inverse::get()
// Desc: Returns the inverse of the transform as a UIElementTransform
//--------------------------------------------------------------------------------------
Windows::UI::Input::IPointerPointTransform^ UIElementTransform::Inverse::get()
{
    XMVECTOR determinant;
    XMMATRIX matrixInv = XMMatrixInverse(&determinant, m_matrix);

    UIElementTransform^ transform = ref new UIElementTransform(nullptr);
    transform->SetMatrix(matrixInv);
    return transform;
}
