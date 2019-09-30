//------------------------------------------------------------------------------
// <copyright file="AudioPanel.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

// Direct2D Header Files
#include <d2d1.h>

/// <summary>
/// Represents possible actions for turtle, such as moving forwards,
/// turning 90 degrees to the left, etc.
/// </summary>
enum TurtleAction
{
    TurtleActionForward,
    TurtleActionBackward,
    TurtleActionTurnRight,
    TurtleActionTurnLeft,
    TurtleActionNone
};

/// <summary>
/// Controls turtle movement and renders turtle shape in different positions and orientations within display area.
/// </summary>
/// <remarks>
/// Note that all turtle elements are laid out directly in an {X,Y} coordinate space  where X is in [0.0,99.057] interval
/// and Y is in [0.0,128.859] interval. Whole panel is later re-scaled to fit available area via a scaling transform.
/// Body parts are created as geometries with boundaries defined as cubic Bezier curves. Each Bezier segment is defined
/// by 4 points: the curve goes through the start and end points and the intermediate points act as magnets that influence
/// the path of the curve.
/// </remarks>
class TurtleController
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    TurtleController();

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~TurtleController();

    /// <summary>
    /// Set the window to draw to.
    /// </summary>
    /// <param name="hWnd">window to draw to.</param>
    /// <param name="pD2DFactory">already created D2D factory object.</param>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT Initialize(const HWND hwnd, ID2D1Factory* pD2DFactory);

    /// <summary>
    /// Draws audio panel.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT Draw();

    /// <summary>
    /// Makes the turtle performs the specified action.
    /// </summary>
    void DoAction(TurtleAction action);

private:
    // Define directions in which turtle can be facing
    // NOTE: These are defined numerically in clockwise order to simplify operation of finding the next direction.
    typedef int Direction;
    static const Direction DirectionUp = 0;
    static const Direction DirectionRight = 1;
    static const Direction DirectionDown = 2;
    static const Direction DirectionLeft = 3;
    static const int NumDirections = 4;

    // Displacements corresponding to each direction
    static const D2D1_POINT_2U Displacements[NumDirections];

    // Number of units of total available width relative to turtle layout coordinates. Used for re-scaling as needed.
    static const UINT ReferenceWidth = 600;

    // Number of units turtle should move forwards or backwards each time, relative to turtle layout coordinates.
    static const int DisplacementAmount = 60;

    // Size of turtle body
    static const D2D1_SIZE_F m_TurtleBodySize;

    /// <summary>
    /// Get the next direction in the specified rotation order.
    /// </summary>
    /// <param name="direction">
    /// Reference direction.
    /// </param>
    /// <param name="clockwise">
    /// true if next direction should be found in clockwise order from reference direction.
    /// false if next direction should be found in counter-clockwise order from reference direction.
    /// </param>
    static Direction NextDirection(Direction direction, bool clockwise)
    {
        return (direction + NumDirections + (clockwise ? 1 : -1)) % NumDirections;
    }

    // Main application window
    HWND                        m_hWnd;

    // Direct2D objects
    ID2D1Factory*               m_pD2DFactory;
    ID2D1HwndRenderTarget*      m_pRenderTarget;
    ID2D1PathGeometry*          m_pTurtleBody;
    ID2D1PathGeometry*          m_pTurtleHead;
    ID2D1PathGeometry*          m_pTurtleTail;
    ID2D1PathGeometry*          m_pTurtleFrontLeftFoot;
    ID2D1PathGeometry*          m_pTurtleBackLeftFoot;
    ID2D1PathGeometry*          m_pTurtleFrontRightFoot;
    ID2D1PathGeometry*          m_pTurtleBackRightFoot;
    ID2D1SolidColorBrush*       m_pBodyFill;

    // Size of display area, in layout coordinates
    D2D1_SIZE_U                 m_LayoutSize;

    // Direction in which turtle is currently facing
    Direction                   m_CurrentDirection;

    // current angle of rotation for turtle (in degrees)
    int                         m_CurrentRotationAngle;

    // Current position of turtle (in layout coordinates)
    D2D1_POINT_2U               m_CurrentPosition;

    // Centers turtle around origin, so it can be easily rotated and translated
    D2D1_MATRIX_3X2_F           m_OriginCenteringTransform;

    // Scales turtle so that it always appears the same size relative to available display area
    D2D1_MATRIX_3X2_F           m_ScalingTransform;

    /// <summary>
    /// Dispose of Direct2d resources.
    /// </summary>
    void DiscardResources( );

    /// <summary>
    /// Ensure necessary Direct2d resources are created.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT EnsureResources();

    /// <summary>
    /// Create geometry for main turtle body.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateMainBody();

    /// <summary>
    /// Create geometry for turtle head.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateHead();

    /// <summary>
    /// Create geometry for turtle tail.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateTail();

    /// <summary>
    /// Create geometry for turtle feet.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateFeet();

    /// <summary>
    /// Create geometry for front left turtle foot.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateFrontLeftFoot();

    /// <summary>
    /// Create geometry for back left turtle foot.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateBackLeftFoot();

    /// <summary>
    /// Create geometry for front right turtle foot.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateFrontRightFoot();

    /// <summary>
    /// Create geometry for back right turtle foot.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateBackRightFoot();

    /// <summary>
    /// Brushes used to fill turtle regions.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT CreateBrushes();

    /// <summary>
    /// Get transformation matrix needed to get turtle body parts from layout coordinates into
    /// display coordinates.
    /// </summary>
    /// <returns>Direct2D transform matrix.</return>
    D2D1_MATRIX_3X2_F GetTurtleTransform();
};