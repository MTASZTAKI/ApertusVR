//------------------------------------------------------------------------------
// <copyright file="KinectFusionExplorer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// System includes
#include "stdafx.h"

// Project includes
#include "resource.h"
#include "KinectFusionExplorer.h"
#include "KinectFusionProcessorFrame.h"
#include "KinectFusionHelper.h"

#define MIN_DEPTH_DISTANCE_MM 500   // Must be greater than 0
#define MAX_DEPTH_DISTANCE_MM 8000
#define MIN_INTEGRATION_WEIGHT 1    // Must be greater than 0
#define MAX_INTEGRATION_WEIGHT 1000

#define WM_FRAMEREADY           (WM_USER + 0)
#define WM_UPDATESENSORSTATUS   (WM_USER + 1)

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
    CKinectFusionExplorer application;
    application.Run(hInstance, nCmdShow);
}

/// <summary>
/// Constructor
/// </summary>
CKinectFusionExplorer::CKinectFusionExplorer() :
m_hWnd(nullptr),
    m_pD2DFactory(nullptr),
    m_pDrawReconstruction(nullptr),
    m_pDrawTrackingResiduals(nullptr),
    m_pDrawDepth(nullptr),
    m_bSavingMesh(false),
    m_saveMeshFormat(Stl),
    m_bInitializeError(false),
    m_bColorCaptured(false),
    m_bUIUpdated(false)
{
}

/// <summary>
/// Destructor
/// </summary>
CKinectFusionExplorer::~CKinectFusionExplorer()
{
    // clean up Direct2D renderer
    SAFE_DELETE(m_pDrawReconstruction);

    // clean up Direct2D renderer
    SAFE_DELETE(m_pDrawTrackingResiduals);

    // clean up Direct2D renderer
    SAFE_DELETE(m_pDrawDepth);

    // clean up Direct2D
    SafeRelease(m_pD2DFactory);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CKinectFusionExplorer::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc  = {0};

    // Dialog custom window class
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"KinectFusionExplorerAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        nullptr,
        (DLGPROC)CKinectFusionExplorer::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != nullptr) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CKinectFusionExplorer::MessageRouter(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    CKinectFusionExplorer* pThis = nullptr;

    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CKinectFusionExplorer*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CKinectFusionExplorer*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CKinectFusionExplorer::DlgProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            InitializeUIControls();

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

            int width = m_params.m_cDepthWidth;
            int height = m_params.m_cDepthHeight;

            // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
            // We'll use this to draw the data we receive from the Kinect to the screen
            m_pDrawDepth = new ImageRenderer();
            HRESULT hr = m_pDrawDepth->Initialize(
                GetDlgItem(m_hWnd, IDC_DEPTH_VIEW),
                m_pD2DFactory,
                width,
                height,
                width * sizeof(long));

            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
                m_bInitializeError = true;
            }

            m_pDrawReconstruction = new ImageRenderer();
            hr = m_pDrawReconstruction->Initialize(
                GetDlgItem(m_hWnd, IDC_RECONSTRUCTION_VIEW),
                m_pD2DFactory,
                width,
                height,
                width * sizeof(long));

            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
                m_bInitializeError = true;
            }

            m_pDrawTrackingResiduals = new ImageRenderer();
            hr = m_pDrawTrackingResiduals->Initialize(
                GetDlgItem(m_hWnd, IDC_TRACKING_RESIDUALS_VIEW),
                m_pD2DFactory,
                width,
                height,
                width * sizeof(long));

            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
                m_bInitializeError = true;
            }

            if (FAILED(m_processor.SetWindow(m_hWnd, WM_FRAMEREADY, WM_UPDATESENSORSTATUS)) ||
                FAILED(m_processor.SetParams(m_params)) ||
                FAILED(m_processor.StartProcessing()))
            {
                m_bInitializeError = true;
            }

            m_saveMeshFormat = m_params.m_saveMeshType;
        }
        break;

        // If the title bar X is clicked, destroy app
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        // Quit the main message pump
        m_processor.StopProcessing();
        PostQuitMessage(0);
        break;

        // Handle button press
    case WM_COMMAND:
        ProcessUI(wParam, lParam);
        break;

        // Handle sliders
    case  WM_HSCROLL:
        UpdateHSliders();
        break;

    case WM_FRAMEREADY:
        HandleCompletedFrame();
        break;

    }

    return FALSE;
}

/// <summary>
/// Handle a completed frame from the Kinect Fusion processor.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
void CKinectFusionExplorer::HandleCompletedFrame()
{
    KinectFusionProcessorFrame const* pFrame = nullptr;

    // Flush any extra WM_FRAMEREADY messages from the queue
    MSG msg;
    while (PeekMessage(&msg, m_hWnd, WM_FRAMEREADY, WM_FRAMEREADY, PM_REMOVE)) {}

    m_processor.LockFrame(&pFrame);

    if (!m_bSavingMesh) // don't render while a mesh is being saved
    {
        if (m_processor.IsVolumeInitialized())
        {
            m_pDrawDepth->Draw(pFrame->m_pDepthRGBX, pFrame->m_cbImageSize);
            m_pDrawReconstruction->Draw(pFrame->m_pReconstructionRGBX, pFrame->m_cbImageSize);
            m_pDrawTrackingResiduals->Draw(pFrame->m_pTrackingDataRGBX, pFrame->m_cbImageSize);
        }

        SetStatusMessage(pFrame->m_statusMessage);
        SetFramesPerSecond(pFrame->m_fFramesPerSecond);
    }

    if (!m_bUIUpdated && m_processor.IsVolumeInitialized())
    {
        const int Mebi = 1024 * 1024;

        // We now create both a color and depth volume, doubling the required memory, so we restrict
        // which resolution settings the user can choose when the graphics card is limited in memory.
        if (pFrame->m_deviceMemory <= 1 * Mebi)  // 1GB
        {
            // Disable 640 voxel resolution in all axes - cards with only 1GB cannot handle this
            HWND hButton = GetDlgItem(m_hWnd, IDC_VOXELS_X_640);
            EnableWindow(hButton, FALSE);
            hButton = GetDlgItem(m_hWnd, IDC_VOXELS_Y_640);
            EnableWindow(hButton, FALSE);
            hButton = GetDlgItem(m_hWnd, IDC_VOXELS_Z_640);
            EnableWindow(hButton, FALSE);

            if (Is64BitApp() == FALSE)
            {
                // Also disable 512 voxel resolution in one arbitrary axis on 32bit machines
                hButton = GetDlgItem(m_hWnd, IDC_VOXELS_Y_512);
                EnableWindow(hButton, FALSE);
            }
        }
        else if (pFrame->m_deviceMemory <= 2 * Mebi)  // 2GB
        {
            if (Is64BitApp() == FALSE)
            {
                // Disable 640 voxel resolution in one arbitrary axis on 32bit machines
                HWND hButton = GetDlgItem(m_hWnd, IDC_VOXELS_Y_640);
                EnableWindow(hButton, FALSE);
            }
            // True 64 bit apps seem to be more able to cope with large volume sizes.
        }

        m_bUIUpdated = true;
    }

    m_bColorCaptured = pFrame->m_bColorCaptured;

    m_processor.UnlockFrame();
}

/// <summary>
/// Save Mesh to disk.
/// </summary>
/// <param name="mesh">The mesh to save.</param>
/// <returns>indicates success or failure</returns>
HRESULT CKinectFusionExplorer::SaveMeshFile(INuiFusionColorMesh* pMesh, KinectFusionMeshTypes saveMeshType)
{
    HRESULT hr = S_OK;

    if (nullptr == pMesh)
    {
        return E_INVALIDARG;
    }

    CComPtr<IFileSaveDialog> pSaveDlg;

    // Create the file save dialog object.
    hr = pSaveDlg.CoCreateInstance(__uuidof(FileSaveDialog));

    if (FAILED(hr))
    {
        return hr;
    }

    // Set the dialog title
    hr = pSaveDlg->SetTitle(L"Save Kinect Fusion Mesh");
    if (SUCCEEDED(hr))
    {
        // Set the button text
        hr = pSaveDlg->SetOkButtonLabel (L"Save");
        if (SUCCEEDED(hr))
        {
            // Set a default filename
            if (Stl == saveMeshType)
            {
                hr = pSaveDlg->SetFileName(L"MeshedReconstruction.stl");
            }
            else if (Obj == saveMeshType)
            {
                hr = pSaveDlg->SetFileName(L"MeshedReconstruction.obj");
            }
            else if (Ply == saveMeshType)
            {
                hr = pSaveDlg->SetFileName(L"MeshedReconstruction.ply");
            }

            if (SUCCEEDED(hr))
            {
                // Set the file type extension
                if (Stl == saveMeshType)
                {
                    hr = pSaveDlg->SetDefaultExtension(L"stl");
                }
                else if (Obj == saveMeshType)
                {
                    hr = pSaveDlg->SetDefaultExtension(L"obj");
                }
                else if (Ply == saveMeshType)
                {
                    hr = pSaveDlg->SetDefaultExtension(L"ply");
                }

                if (SUCCEEDED(hr))
                {
                    // Set the file type filters
                    if (Stl == saveMeshType)
                    {
                        COMDLG_FILTERSPEC allPossibleFileTypes[] = {
                            { L"Stl mesh files", L"*.stl" },
                            { L"All files", L"*.*" }
                        };

                        hr = pSaveDlg->SetFileTypes(
                            ARRAYSIZE(allPossibleFileTypes),
                            allPossibleFileTypes);
                    }
                    else if (Obj == saveMeshType)
                    {
                        COMDLG_FILTERSPEC allPossibleFileTypes[] = {
                            { L"Obj mesh files", L"*.obj" },
                            { L"All files", L"*.*" }
                        };

                        hr = pSaveDlg->SetFileTypes(
                            ARRAYSIZE(allPossibleFileTypes),
                            allPossibleFileTypes );
                    }
                    else if (Ply == saveMeshType)
                    {
                        COMDLG_FILTERSPEC allPossibleFileTypes[] = {
                            { L"Ply mesh files", L"*.ply" },
                            { L"All files", L"*.*" }
                        };

                        hr = pSaveDlg->SetFileTypes(
                            ARRAYSIZE(allPossibleFileTypes),
                            allPossibleFileTypes );
                    }

                    if (SUCCEEDED(hr))
                    {
                        // Show the file selection box
                        hr = pSaveDlg->Show(m_hWnd);

                        // Save the mesh to the chosen file.
                        if (SUCCEEDED(hr))
                        {
                            CComPtr<IShellItem> pItem;
                            hr = pSaveDlg->GetResult(&pItem);

                            if (SUCCEEDED(hr))
                            {
                                LPOLESTR pwsz = nullptr;
                                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

                                if (SUCCEEDED(hr))
                                {
                                    SetStatusMessage(L"Saving mesh file, please wait...");
                                    SetCursor(LoadCursor(nullptr, MAKEINTRESOURCE(IDC_WAIT)));

                                    if (Stl == saveMeshType)
                                    {
                                        hr = WriteBinarySTLMeshFile(pMesh, pwsz);
                                    }
                                    else if (Obj == saveMeshType)
                                    {
                                        hr = WriteAsciiObjMeshFile(pMesh, pwsz);
                                    }
                                    else if (Ply == saveMeshType)
                                    {
                                        hr = WriteAsciiPlyMeshFile(pMesh, pwsz, true, m_bColorCaptured);
                                    }

                                    CoTaskMemFree(pwsz);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Initialize the UI
/// </summary>
void CKinectFusionExplorer::InitializeUIControls()
{
    // Create NuiSensorChooser UI control
    RECT rc;
    GetClientRect(m_hWnd, &rc);

    POINT ptCenterTop;
    ptCenterTop.x = (rc.right - rc.left)/2;
    ptCenterTop.y = 0;

    // Set slider ranges
    SendDlgItemMessage(
        m_hWnd,
        IDC_SLIDER_DEPTH_MIN,
        TBM_SETRANGE,
        TRUE,
        MAKELPARAM(MIN_DEPTH_DISTANCE_MM, MAX_DEPTH_DISTANCE_MM));

    SendDlgItemMessage(m_hWnd,
        IDC_SLIDER_DEPTH_MAX,
        TBM_SETRANGE,
        TRUE,
        MAKELPARAM(MIN_DEPTH_DISTANCE_MM, MAX_DEPTH_DISTANCE_MM));

    SendDlgItemMessage(
        m_hWnd,
        IDC_INTEGRATION_WEIGHT_SLIDER,
        TBM_SETRANGE,
        TRUE,
        MAKELPARAM(MIN_INTEGRATION_WEIGHT, MAX_INTEGRATION_WEIGHT));

    // Set slider positions
    SendDlgItemMessage(
        m_hWnd,
        IDC_SLIDER_DEPTH_MAX,
        TBM_SETPOS,
        TRUE,
        (UINT)m_params.m_fMaxDepthThreshold * 1000);

    SendDlgItemMessage(
        m_hWnd,
        IDC_SLIDER_DEPTH_MIN,
        TBM_SETPOS,
        TRUE,
        (UINT)m_params.m_fMinDepthThreshold * 1000);

    SendDlgItemMessage(
        m_hWnd,
        IDC_INTEGRATION_WEIGHT_SLIDER,
        TBM_SETPOS,
        TRUE,
        (UINT)m_params.m_cMaxIntegrationWeight);

    // Set intermediate slider tics at meter intervals
    for (int i=1; i<(MAX_DEPTH_DISTANCE_MM/1000); i++)
    {
        SendDlgItemMessage(m_hWnd, IDC_SLIDER_DEPTH_MAX, TBM_SETTIC, 0, i*1000);
        SendDlgItemMessage(m_hWnd, IDC_SLIDER_DEPTH_MIN, TBM_SETTIC, 0, i*1000);
    }

    // Update slider text
    WCHAR str[MAX_PATH];
    swprintf_s(str, ARRAYSIZE(str), L"%4.2fm", m_params.m_fMinDepthThreshold);
    SetDlgItemText(m_hWnd, IDC_MIN_DIST_TEXT, str);
    swprintf_s(str, ARRAYSIZE(str), L"%4.2fm", m_params.m_fMaxDepthThreshold);
    SetDlgItemText(m_hWnd, IDC_MAX_DIST_TEXT, str);

    swprintf_s(str, ARRAYSIZE(str), L"%u", m_params.m_cMaxIntegrationWeight);
    SetDlgItemText(m_hWnd, IDC_INTEGRATION_WEIGHT_TEXT, str);

    // Set the radio buttons for Volume Parameters
    switch((int)m_params.m_reconstructionParams.voxelsPerMeter)
    {
    case 768:
        CheckDlgButton(m_hWnd, IDC_VPM_768, BST_CHECKED);
        break;
    case 640:
        CheckDlgButton(m_hWnd, IDC_VPM_640, BST_CHECKED);
        break;
    case 512:
        CheckDlgButton(m_hWnd, IDC_VPM_512, BST_CHECKED);
        break;
    case 384:
        CheckDlgButton(m_hWnd, IDC_VPM_384, BST_CHECKED);
        break;
    case 256:
        CheckDlgButton(m_hWnd, IDC_VPM_256, BST_CHECKED);
        break;
    case 128:
        CheckDlgButton(m_hWnd, IDC_VPM_128, BST_CHECKED);
        break;
    default:
        m_params.m_reconstructionParams.voxelsPerMeter = 256.0f;	// set to medium default
        CheckDlgButton(m_hWnd, IDC_VPM_256, BST_CHECKED);
        break;
    }

    switch((int)m_params.m_reconstructionParams.voxelCountX)
    {
    case 640:
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_640, BST_CHECKED);
        break;
    case 512:
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_512, BST_CHECKED);
        break;
    case 384:
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_384, BST_CHECKED);
        break;
    case 256:
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_256, BST_CHECKED);
        break;
    case 128:
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_128, BST_CHECKED);
        break;
    default:
        m_params.m_reconstructionParams.voxelCountX = 384;	// set to medium default
        CheckDlgButton(m_hWnd, IDC_VOXELS_X_384, BST_CHECKED);
        break;
    }

    switch((int)m_params.m_reconstructionParams.voxelCountY)
    {
    case 640:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_640, BST_CHECKED);
        break;
    case 512:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_512, BST_CHECKED);
        break;
    case 384:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_384, BST_CHECKED);
        break;
    case 256:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_256, BST_CHECKED);
        break;
    case 128:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_128, BST_CHECKED);
        break;
    default:
        m_params.m_reconstructionParams.voxelCountX = 384;	// set to medium default
        CheckDlgButton(m_hWnd, IDC_VOXELS_Y_384, BST_CHECKED);
        break;
    }

    switch((int)m_params.m_reconstructionParams.voxelCountZ)
    {
    case 640:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_640, BST_CHECKED);
        break;
    case 512:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_512, BST_CHECKED);
        break;
    case 384:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_384, BST_CHECKED);
        break;
    case 256:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_256, BST_CHECKED);
        break;
    case 128:
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_128, BST_CHECKED);
        break;
    default:
        m_params.m_reconstructionParams.voxelCountX = 384;	// set to medium default
        CheckDlgButton(m_hWnd, IDC_VOXELS_Z_384, BST_CHECKED);
        break;
    }

    if (Stl == m_saveMeshFormat)
    {
        CheckDlgButton(m_hWnd, IDC_MESH_FORMAT_STL_RADIO, BST_CHECKED);
    }
    else if (Obj == m_saveMeshFormat)
    {
        CheckDlgButton(m_hWnd, IDC_MESH_FORMAT_OBJ_RADIO, BST_CHECKED);
    }
    else if (Ply == m_saveMeshFormat)
    {
        CheckDlgButton(m_hWnd, IDC_MESH_FORMAT_PLY_RADIO, BST_CHECKED);
    }

    if (m_params.m_bCaptureColor)
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CAPTURE_COLOR, BST_CHECKED);
    }

    if (m_params.m_bAutoFindCameraPoseWhenLost)
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CAMERA_POSE_FINDER, BST_CHECKED);
    }
}

/// <summary>
/// Process the UI inputs
/// </summary>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
void CKinectFusionExplorer::ProcessUI(WPARAM wParam, LPARAM)
{
    // If it was for the display surface normals toggle this variable
    if (IDC_CHECK_CAPTURE_COLOR == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        // Toggle capture color
        m_params.m_bCaptureColor = !m_params.m_bCaptureColor;
    }
    // If it was for the display surface normals toggle this variable
    if (IDC_CHECK_MIRROR_DEPTH == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        // Toggle depth mirroring
        m_params.m_bMirrorDepthFrame = !m_params.m_bMirrorDepthFrame;

        // Un-check pause
        CheckDlgButton(m_hWnd, IDC_CHECK_PAUSE_INTEGRATION, BST_UNCHECKED);
        m_processor.ResetReconstruction();
    }
    if (IDC_CHECK_CAMERA_POSE_FINDER == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_bAutoFindCameraPoseWhenLost = !m_params.m_bAutoFindCameraPoseWhenLost;
    }
    // If it was the reset button clicked, clear the volume
    if (IDC_BUTTON_RESET_RECONSTRUCTION == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        // Un-check pause
        CheckDlgButton(m_hWnd, IDC_CHECK_PAUSE_INTEGRATION, BST_UNCHECKED);
        m_processor.ResetReconstruction();
    }
    // If it was the mesh button clicked, mesh the volume and save
    if (IDC_BUTTON_MESH_RECONSTRUCTION == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        SetStatusMessage(L"Creating and saving mesh of reconstruction, please wait...");
        m_bSavingMesh = true;

        // Pause integration while we're saving
        bool wasPaused = m_params.m_bPauseIntegration;
        m_params.m_bPauseIntegration = true;
        m_processor.SetParams(m_params);

        INuiFusionColorMesh *mesh = nullptr;
        HRESULT hr = m_processor.CalculateMesh(&mesh);

        if (SUCCEEDED(hr))
        {
            // Save mesh
            hr = SaveMeshFile(mesh, m_saveMeshFormat);

            if (SUCCEEDED(hr))
            {
                SetStatusMessage(L"Saved Kinect Fusion mesh.");
            }
            else if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
            {
                SetStatusMessage(L"Mesh save canceled.");
            }
            else
            {
                SetStatusMessage(L"Error saving Kinect Fusion mesh!");
            }

            // Release the mesh
            SafeRelease(mesh);
        }
        else
        {
            SetStatusMessage(L"Failed to create mesh of reconstruction.");
        }

        // Restore pause state of integration
        m_params.m_bPauseIntegration = wasPaused;
        m_processor.SetParams(m_params);

        m_bSavingMesh = false;
    }
    if (IDC_CHECK_PAUSE_INTEGRATION == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        // Toggle the pause state of the reconstruction
        m_params.m_bPauseIntegration = !m_params.m_bPauseIntegration;
    }
    if (IDC_VPM_768 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 768.0f;
    }
    if (IDC_VPM_640 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 640.0f;
    }
    if (IDC_VPM_512 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 512.0f;
    }
    if (IDC_VPM_384 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 384.0f;
    }
    if (IDC_VPM_256 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 256.0f;
    }
    if (IDC_VPM_128 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelsPerMeter = 128.0f;
    }
    if (IDC_VOXELS_X_640 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountX = 640;
    }
    if (IDC_VOXELS_X_512 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountX = 512;
    }
    if (IDC_VOXELS_X_384 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountX = 384;
    }
    if (IDC_VOXELS_X_256 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountX = 256;
    }
    if (IDC_VOXELS_X_128 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountX = 128;
    }
    if (IDC_VOXELS_Y_640 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountY = 640;
    }
    if (IDC_VOXELS_Y_512 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountY = 512;
    }
    if (IDC_VOXELS_Y_384 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountY = 384;
    }
    if (IDC_VOXELS_Y_256 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountY = 256;
    }
    if (IDC_VOXELS_Y_128 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountY = 128;
    }
    if (IDC_VOXELS_Z_640 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountZ = 640;
    }
    if (IDC_VOXELS_Z_512 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountZ = 512;
    }
    if (IDC_VOXELS_Z_384 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountZ = 384;
    }
    if (IDC_VOXELS_Z_256 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountZ = 256;
    }
    if (IDC_VOXELS_Z_128 == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_params.m_reconstructionParams.voxelCountZ = 128;
    }
    if (IDC_MESH_FORMAT_STL_RADIO == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_saveMeshFormat = Stl;
    }
    if (IDC_MESH_FORMAT_OBJ_RADIO == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_saveMeshFormat = Obj;
    }
    if (IDC_MESH_FORMAT_PLY_RADIO == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        m_saveMeshFormat = Ply;
    }

    m_processor.SetParams(m_params);
}

/// <summary>
/// Update the internal variable values from the UI Horizontal sliders.
/// </summary>
void CKinectFusionExplorer::UpdateHSliders()
{
    int mmMinPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER_DEPTH_MIN, TBM_GETPOS, 0,0);

    if (mmMinPos >= MIN_DEPTH_DISTANCE_MM && mmMinPos <= MAX_DEPTH_DISTANCE_MM)
    {
        m_params.m_fMinDepthThreshold = (float)mmMinPos * 0.001f;
    }

    int mmMaxPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER_DEPTH_MAX, TBM_GETPOS, 0,0);

    if (mmMaxPos >= MIN_DEPTH_DISTANCE_MM && mmMaxPos <= MAX_DEPTH_DISTANCE_MM)
    {
        m_params.m_fMaxDepthThreshold = (float)mmMaxPos * 0.001f;
    }

    int maxWeight = (int)SendDlgItemMessage(m_hWnd, IDC_INTEGRATION_WEIGHT_SLIDER, TBM_GETPOS, 0,0);
    m_params.m_cMaxIntegrationWeight = maxWeight % (MAX_INTEGRATION_WEIGHT+1);


    // update text
    WCHAR str[MAX_PATH];
    swprintf_s(str, ARRAYSIZE(str), L"%4.2fm", m_params.m_fMinDepthThreshold);
    SetDlgItemText(m_hWnd, IDC_MIN_DIST_TEXT, str);
    swprintf_s(str, ARRAYSIZE(str), L"%4.2fm", m_params.m_fMaxDepthThreshold);
    SetDlgItemText(m_hWnd, IDC_MAX_DIST_TEXT, str);

    swprintf_s(str, ARRAYSIZE(str), L"%u", m_params.m_cMaxIntegrationWeight);
    SetDlgItemText(m_hWnd, IDC_INTEGRATION_WEIGHT_TEXT, str);

    m_processor.SetParams(m_params);
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void CKinectFusionExplorer::SetStatusMessage(const WCHAR * szMessage)
{
    size_t length = 0;
    if (FAILED(StringCchLength(
        szMessage,
        KinectFusionProcessorFrame::StatusMessageMaxLen,
        &length)))
    {
        length = 0;
    }

    if (length > 0)
    {
        SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
        m_tickLastStatus = GetTickCount64();
    }
    else
    {
        // Clear the status message after a timeout (as long as frames are flowing)
        if (GetTickCount64() - m_tickLastStatus > cStatusTimeoutInMilliseconds &&
            m_fFramesPerSecond > 0)
        {
            SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, 0);
            m_tickLastStatus = GetTickCount64();
        }
    }
}

/// <summary>
/// Set the frames-per-second message
/// </summary>
/// <param name="fFramesPerSecond">current frame rate</param>
void CKinectFusionExplorer::SetFramesPerSecond(float fFramesPerSecond)
{
    if (fFramesPerSecond != m_fFramesPerSecond)
    {
        m_fFramesPerSecond = fFramesPerSecond;
        WCHAR str[MAX_PATH] = {0};
        if (fFramesPerSecond > 0)
        {
            swprintf_s(str, ARRAYSIZE(str), L"%5.2f FPS", fFramesPerSecond);
        }

        SendDlgItemMessageW(m_hWnd, IDC_FRAMES_PER_SECOND, WM_SETTEXT, 0, (LPARAM)str);
    }
}
