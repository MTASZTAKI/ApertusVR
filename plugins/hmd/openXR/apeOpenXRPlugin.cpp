#include "apeOpenXRPlugin.h"
#include <wrl/client.h>

ape::OpenXRPlugin::OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mOpenXRDepthLsr = false;
	mOpenXRAppConfigForm = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	mOpenXRSystemID = XR_NULL_SYSTEM_ID;
	mOpenXRAppConfigView = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	mOpenXRBlend = XrEnvironmentBlendMode::XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;
	mOpenXRHeadSpace = {};
	mOpenXRConfigViews = std::vector<XrViewConfigurationView>();
	mOpenXRViewPointView = std::vector<ape::Matrix4>();
	mOpenXRViewPointProjection = std::vector<ape::Matrix4>();
	mOpenXRSessionState = XR_SESSION_STATE_UNKNOWN;
	mIsOpenXRRunning = false;
	mCameraLeft = ape::CameraWeakPtr();
	mCameraRight = ape::CameraWeakPtr();
	mOpenXRViews = std::vector<XrView>();
	mOpenXRSwapchains = std::vector<XrSwapchain>();
	mOpenXRSwapchainDX11Textures = std::vector<XrSwapchainImageD3D11KHR>();
	mOpenXRSwapchainOGLTextures = std::vector<XrSwapchainImageOpenGLKHR>();
	APE_LOG_FUNC_LEAVE();
}

ape::OpenXRPlugin::~OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::openXRPreferredExtensions(uint32_t &out_extension_count, const char **out_extensions)
{
	const char *extensions[] = {
		XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
		XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME,
		XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME,
		XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME };
	uint32_t ext_count = 0;
	xrEnumerateInstanceExtensionProperties(nullptr, 0, &ext_count, nullptr);
	XrExtensionProperties *exts = (XrExtensionProperties *)malloc(sizeof(XrExtensionProperties) * ext_count);
	for (uint32_t i = 0; i < ext_count; i++) exts[i] = { XR_TYPE_EXTENSION_PROPERTIES };
	xrEnumerateInstanceExtensionProperties(nullptr, ext_count, &ext_count, exts);
	out_extension_count = 0;
	for (int32_t e = 0; e < _countof(extensions); e++) 
	{
		for (uint32_t i = 0; i < ext_count; i++) 
		{
			if (strcmp(exts[i].extensionName, extensions[e]) == 0) 
			{
				if (out_extensions != nullptr)
					out_extensions[out_extension_count] = extensions[e];
				out_extension_count += 1;
				break;
			}
		}
	}
	if (out_extensions != nullptr) 
	{
		for (uint32_t i = 0; i < out_extension_count; i++) 
		{
			if (strcmp(out_extensions[i], XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME) == 0)
				mOpenXRDepthLsr = true;
		}
	}
	free(exts);
}

XrReferenceSpaceType ape::OpenXRPlugin::openXRPreferredSpace() 
{
	XrReferenceSpaceType refspace_priority[] = {
		XR_REFERENCE_SPACE_TYPE_UNBOUNDED_MSFT,
		XR_REFERENCE_SPACE_TYPE_LOCAL,
		XR_REFERENCE_SPACE_TYPE_STAGE, };
	uint32_t refspace_count = 0;
	xrEnumerateReferenceSpaces(mOpenXRSession, 0, &refspace_count, nullptr);
	XrReferenceSpaceType *refspace_types = (XrReferenceSpaceType *)malloc(sizeof(XrReferenceSpaceType) * refspace_count);
	xrEnumerateReferenceSpaces(mOpenXRSession, refspace_count, &refspace_count, refspace_types);
	XrReferenceSpaceType result = (XrReferenceSpaceType)0;
	for (int32_t p = 0; p < _countof(refspace_priority); p++) 
	{
		for (uint32_t i = 0; i < refspace_count; i++) 
		{
			if (refspace_types[i] == refspace_priority[p]) 
			{
				result = refspace_types[i];
				break;
			}
		}
		if (result != 0)
			break;
	}
	free(refspace_types);
	return result;
}

void ape::OpenXRPlugin::openXRPreferredFormat(DXGI_FORMAT &out_pixel_format, tex_format_ &out_depth_format)
{
	DXGI_FORMAT pixel_formats[] = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, };
	DXGI_FORMAT depth_formats[] = {
		DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT_D16_UNORM,
		DXGI_FORMAT_D24_UNORM_S8_UINT, };
	uint32_t count = 0;
	xrEnumerateSwapchainFormats(mOpenXRSession, 0, &count, nullptr);
	int64_t *formats = (int64_t *)malloc(sizeof(int64_t) * count);
	xrEnumerateSwapchainFormats(mOpenXRSession, count, &count, formats);
	out_pixel_format = DXGI_FORMAT_UNKNOWN;
	for (uint32_t i = 0; i < count; i++) {
		for (int32_t f = 0; f < _countof(pixel_formats); f++) 
		{
			if (formats[i] == pixel_formats[f]) 
			{
				out_pixel_format = pixel_formats[f];
				break;
			}
		}
		if (out_pixel_format != DXGI_FORMAT_UNKNOWN)
			break;
	}
	DXGI_FORMAT depth_format = DXGI_FORMAT_UNKNOWN;
	for (uint32_t i = 0; i < count; i++) 
	{
		for (int32_t f = 0; f < _countof(depth_formats); f++) 
		{
			if (formats[i] == depth_formats[f]) 
			{
				depth_format = depth_formats[f];
				break;
			}
		}
		if (depth_format != DXGI_FORMAT_UNKNOWN)
			break;
	}
	out_depth_format = tex_format_depth16;
	switch (depth_format) 
	{
	case DXGI_FORMAT_D16_UNORM:         out_depth_format = tex_format_depth16; break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: out_depth_format = tex_format_depthstencil; break;
	case DXGI_FORMAT_D32_FLOAT:         out_depth_format = tex_format_depth32; break;
	}
	free(formats);
}

bool ape::OpenXRPlugin::openXRLocValid(XrSpaceLocation &loc) 
{
	return
		(loc.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
		(loc.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0;
}

void ape::OpenXRPlugin::openXRPollEvents() 
{
	//APE_LOG_DEBUG("openXRPollEvents: ");
	XrEventDataBuffer event_buffer = { XR_TYPE_EVENT_DATA_BUFFER };
	while (xrPollEvent(mOpenXRInstance, &event_buffer) == XR_SUCCESS) 
	{
		switch (event_buffer.type)
		{
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: 
			{
				XrEventDataSessionStateChanged *changed = (XrEventDataSessionStateChanged*)&event_buffer;
				mOpenXRSessionState = changed->state;
				switch (mOpenXRSessionState)
				{
					case XR_SESSION_STATE_READY: 
					{
						XrSessionBeginInfo begin_info = { XR_TYPE_SESSION_BEGIN_INFO };
						begin_info.primaryViewConfigurationType = mOpenXRAppConfigView;
						xrBeginSession(mOpenXRSession, &begin_info);
						mIsOpenXRRunning = true;
						APE_LOG_DEBUG("OpenXR session begin.");
					} 
					break;
					case XR_SESSION_STATE_STOPPING:     xrEndSession(mOpenXRSession); mIsOpenXRRunning = false; break;
				}
			} 
			break;
		}
		event_buffer = { XR_TYPE_EVENT_DATA_BUFFER };
	}
}

void ape::OpenXRPlugin::openXRProjection(XrFovf fov, float clip_near, float clip_far, float *result) 
{
	const float tanLeft = tanf(fov.angleLeft);
	const float tanRight = tanf(fov.angleRight);
	const float tanDown = tanf(fov.angleDown);
	const float tanUp = tanf(fov.angleUp);
	const float tanAngleWidth = tanRight - tanLeft;
	const float tanAngleHeight = tanUp - tanDown;
	const float range = clip_far / (clip_near - clip_far);
	memset(result, 0, sizeof(float) * 16);
	result[0] = 2 / tanAngleWidth;                   
	result[5] = 2 / tanAngleHeight;                   
	result[8] = (tanRight + tanLeft) / tanAngleWidth; 
	result[9] = (tanUp + tanDown) / tanAngleHeight;  
	result[10] = range;                               
	result[11] = -1;                                  
	result[14] = range * clip_near;                  
}

void ape::OpenXRPlugin::openXRPollActions()
{
	//APE_LOG_DEBUG("openXRPollActions: " << mOpenXRTime);
	if (mOpenXRSessionState != XR_SESSION_STATE_FOCUSED)
		return;
	XrSpaceLocation space_location = { XR_TYPE_SPACE_LOCATION };
	XrResult res = xrLocateSpace(mOpenXRHeadSpace, mOpenXRAppSpace, mOpenXRTime, &space_location);
	if (XR_UNQUALIFIED_SUCCESS(res) && openXRLocValid(space_location))
	{
		//TODO send it like in openVR plugin
		//memcpy(&out_pose.position, &space_location.pose.position, sizeof(vec3));
		//memcpy(&out_pose.orientation, &space_location.pose.orientation, sizeof(quat));
	}
}

bool ape::OpenXRPlugin::openXRRenderLayer(XrTime predictedTime, std::vector<XrCompositionLayerProjectionView> &views, XrCompositionLayerProjection &layer) 
{
	uint32_t view_count = 0;
	XrViewState view_state = { XR_TYPE_VIEW_STATE };
	XrViewLocateInfo locate_info = { XR_TYPE_VIEW_LOCATE_INFO };
	locate_info.viewConfigurationType = mOpenXRAppConfigView;
	locate_info.displayTime = predictedTime;
	locate_info.space = mOpenXRAppSpace;
	xrLocateViews(mOpenXRSession, &locate_info, &view_state, (uint32_t)mOpenXRViews.size(), &view_count, mOpenXRViews.data());
	views.resize(view_count);
	for (uint32_t i = 0; i < view_count; i++) 
	{
		uint32_t img_id;
		XrSwapchainImageAcquireInfo acquire_info = { XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		xrAcquireSwapchainImage(mOpenXRSwapchains[i], &acquire_info, &img_id);
		//APE_LOG_DEBUG("mOpenXRSwapchains: " << mOpenXRSwapchains[i] << " img_id: " << img_id);
		XrSwapchainImageWaitInfo wait_info = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		wait_info.timeout = XR_INFINITE_DURATION;
		xrWaitSwapchainImage(mOpenXRSwapchains[i], &wait_info);
		views[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
		views[i].pose = mOpenXRViews[i].pose;
		views[i].fov = mOpenXRViews[i].fov;
		views[i].subImage.swapchain = mOpenXRSwapchains[i];
		views[i].subImage.imageRect.offset = { 0, 0 };
		views[i].subImage.imageRect.extent = { (int32_t)mpCoreConfig->getWindowConfig().width, (int32_t)mpCoreConfig->getWindowConfig().height };
		float xr_projection[16];
		openXRProjection(views[i].fov, 0.1f, 50, xr_projection);
		memcpy(&mOpenXRViewPointProjection[i], xr_projection, sizeof(float) * 16);
		//TODO
		//matrix_inverse(matrix_trs((vec3&)views[i].pose.position, (quat&)views[i].pose.orientation, vec3_one), mOpenXRViewPointView[i]);
		int textureID = (i + 1) * img_id;
		if (i == 0)
		{
			if (auto manualTextureLeftEye = mManualTextureLeftEye.lock())
			{
				if (renderBackend == "DX11") {
					Microsoft::WRL::ComPtr<ID3D11Device> dev11;
					Microsoft::WRL::ComPtr<ID3D11DeviceContext> devcon11;
					mOpenXRSwapchainDX11Textures[textureID].texture->GetDevice(&dev11);
					dev11->GetImmediateContext(&devcon11);
					devcon11->CopyResource(mOpenXRSwapchainDX11Textures[textureID].texture, (ID3D11Texture2D*)manualTextureLeftEye->getGraphicsApiID());
				}
				else if (renderBackend == "OGL") {
					manualTextureLeftEye->setGraphicsApiID((void*)textureID);
					manualTextureLeftEye->setContextID((void*)wglGetCurrentContext());
				}
				
			}
		}
		else
		{
			if (auto manualTextureRightEye = mManualTextureRightEye.lock())
			{
				if (renderBackend == "DX11") {
					Microsoft::WRL::ComPtr<ID3D11Device> dev11;
					Microsoft::WRL::ComPtr<ID3D11DeviceContext> devcon11;
					mOpenXRSwapchainDX11Textures[textureID].texture->GetDevice(&dev11);
					dev11->GetImmediateContext(&devcon11);
					devcon11->CopyResource(mOpenXRSwapchainDX11Textures[textureID].texture, (ID3D11Texture2D*)manualTextureRightEye->getGraphicsApiID());
				}
				else if (renderBackend == "OGL") {
					manualTextureRightEye->setGraphicsApiID((void*)textureID);
					manualTextureRightEye->setContextID((void*)wglGetCurrentContext());
				}
			}
		}
		XrSwapchainImageReleaseInfo release_info = { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		xrReleaseSwapchainImage(mOpenXRSwapchains[i], &release_info);
	}
	layer.space = mOpenXRAppSpace;
	layer.viewCount = (uint32_t)views.size();
	layer.views = views.data();
	layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	return true;
}

void ape::OpenXRPlugin::openXRRenderFrame() 
{
	XrFrameState frame_state = { XR_TYPE_FRAME_STATE };
	xrWaitFrame(mOpenXRSession, nullptr, &frame_state);
	xrBeginFrame(mOpenXRSession, nullptr);
	mOpenXRTime = frame_state.predictedDisplayTime + frame_state.predictedDisplayPeriod;
	XrCompositionLayerBaseHeader* layer = nullptr;
	XrCompositionLayerProjection layer_proj = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	std::vector<XrCompositionLayerProjectionView> views;
	bool session_active = mOpenXRSessionState == XR_SESSION_STATE_VISIBLE || mOpenXRSessionState == XR_SESSION_STATE_FOCUSED;
	//APE_LOG_DEBUG("mOpenXRSessionState: " << mOpenXRSessionState);
	//APE_LOG_DEBUG("mOpenXRTime: " << mOpenXRTime);
	if (session_active && openXRRenderLayer(frame_state.predictedDisplayTime, views, layer_proj))
	{
		layer = (XrCompositionLayerBaseHeader*)&layer_proj;
	}
	XrFrameEndInfo end_info{ XR_TYPE_FRAME_END_INFO };
	end_info.displayTime = frame_state.predictedDisplayTime;
	end_info.environmentBlendMode = mOpenXRBlend;
	end_info.layerCount = layer == nullptr ? 0 : 1;
	end_info.layers = &layer;
	xrEndFrame(mOpenXRSession, &end_info);
}

void ape::OpenXRPlugin::submitTextureLeftToOpenXR()
{
	//APE_LOG_DEBUG("submitTextureLeftToOpenXR");
	if (mIsOpenXRRunning)
	{
		openXRPollActions();
		openXRRenderFrame();
	}
}

void ape::OpenXRPlugin::submitTextureRightToOpenXR()
{
	
}

void ape::OpenXRPlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID)
	{
		if (auto textureManual = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			if (event.subjectName == "OpenXRRenderTextureLeft")
			{
				textureManual->registerFunction(std::bind(&OpenXRPlugin::submitTextureLeftToOpenXR, this));
			}
			else if (event.subjectName == "OpenXRRenderTextureRight")
			{
				textureManual->registerFunction(std::bind(&OpenXRPlugin::submitTextureRightToOpenXR, this));
			}
		}
	}
}

void ape::OpenXRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeOpenXRPlugin.json";
	auto mApeOpenXRConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
	mOpenXRJson = nlohmann::json::parse(mApeOpenXRConfigFile);
	renderBackend = mOpenXRJson.get_render_system();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();

	//if (renderBackend == "DX11") {
		APE_LOG_DEBUG("waiting for main window");
		while (mpCoreConfig->getWindowConfig().handle == nullptr && mpCoreConfig->getWindowConfig().device == nullptr)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		APE_LOG_DEBUG("main window was found");
	//}
	


	APE_LOG_DEBUG("try to initialize openXR HMD");
	uint32_t extension_count = 0;
	openXRPreferredExtensions(extension_count, nullptr);
	const char **extensions = (const char**)malloc(sizeof(char *) * extension_count);
	openXRPreferredExtensions(extension_count, extensions);
	XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
	createInfo.enabledExtensionCount = extension_count;
	createInfo.enabledExtensionNames = extensions;
	createInfo.applicationInfo.applicationVersion = 1;
	createInfo.applicationInfo.engineVersion = 1;
	createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
	strcpy_s(createInfo.applicationInfo.applicationName, "apeSampleLauncher");
	strcpy_s(createInfo.applicationInfo.engineName, "ApertusVR");
	XrResult result = xrCreateInstance(&createInfo, &mOpenXRInstance);
	free(extensions);
	if (XR_FAILED(result) || mOpenXRInstance == XR_NULL_HANDLE) 
	{
		APE_LOG_DEBUG("Couldn't create OpenXR instance " << result << " is OpenXR installed and set as the active runtime?");
	}
	mOpenXRExtensions = xrCreateExtensionTable(mOpenXRInstance);
	XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = mOpenXRAppConfigForm;
	result = xrGetSystem(mOpenXRInstance, &systemInfo, &mOpenXRSystemID);
	if (XR_FAILED(result))
	{
		APE_LOG_DEBUG("xrGetSystem failed " << result);
	}
	if (renderBackend == "OGL") {
		XrGraphicsRequirementsOpenGLKHR requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
		result = mOpenXRExtensions.xrGetOpenGLGraphicsRequirementsKHR(mOpenXRInstance, mOpenXRSystemID, &requirement);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrGetD3D11GraphicsRequirementsKHR failed " << result);
		}
		uint32_t blend_count = 0;
		std::vector<XrEnvironmentBlendMode> blend_modes;
		result = xrEnumerateEnvironmentBlendModes(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, 0, &blend_count, nullptr);
		blend_modes.resize(blend_count);
		result = xrEnumerateEnvironmentBlendModes(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, blend_count, &blend_count, blend_modes.data());
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrEnumerateEnvironmentBlendModes failed " << result);
		}
		for (size_t i = 0; i < blend_count; i++)
		{
			if (blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_ADDITIVE ||
				blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_OPAQUE ||
				blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND)
			{
				mOpenXRBlend = blend_modes[i];
				break;
			}
		}

		XrGraphicsBindingOpenGLWin32KHR ogl_bindig = { XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
		ogl_bindig.hDC = (HDC)mpCoreConfig->getWindowConfig().device;
		ogl_bindig.hGLRC = wglGetCurrentContext();
		//ogl_bindig.hDC = wglGetCurrentDC();
		XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
		sessionInfo.next = &ogl_bindig;
		sessionInfo.systemId = mOpenXRSystemID;

		xrCreateSession(mOpenXRInstance, &sessionInfo, &mOpenXRSession);
		if (XR_FAILED(result) || mOpenXRSession == XR_NULL_HANDLE)
		{
			APE_LOG_DEBUG("Couldn't create an OpenXR session, no MR device attached/ready? " << result);
		}
		mOpenXRRefSpace = openXRPreferredSpace();
		XrReferenceSpaceCreateInfo ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		ref_space.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };
		ref_space.referenceSpaceType = mOpenXRRefSpace;
		result = xrCreateReferenceSpace(mOpenXRSession, &ref_space, &mOpenXRAppSpace);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrCreateReferenceSpace failed " << result);
		}
		ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		ref_space.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };
		ref_space.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
		result = xrCreateReferenceSpace(mOpenXRSession, &ref_space, &mOpenXRHeadSpace);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrCreateReferenceSpace failed " << result);
		}

		DXGI_FORMAT color_format;
		tex_format_ depth_format;
		openXRPreferredFormat(color_format, depth_format);
		uint32_t view_count = 0;
		xrEnumerateViewConfigurationViews(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, 0, &view_count, nullptr);
		mOpenXRConfigViews.resize(view_count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
		mOpenXRViews.resize(view_count, { XR_TYPE_VIEW });
		mOpenXRViewPointView.resize(view_count, { });
		mOpenXRViewPointProjection.resize(view_count, { });
		result = xrEnumerateViewConfigurationViews(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, view_count, &view_count, mOpenXRConfigViews.data());
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrEnumerateViewConfigurationViews failed " << result);
		}
		for (auto openXRConfigViews : mOpenXRConfigViews)
		{
			XrSwapchainCreateInfo swapchain_info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
			XrSwapchain xrSwapchain;
			swapchain_info.arraySize = 1;
			swapchain_info.mipCount = 1;
			swapchain_info.faceCount = 1;
			swapchain_info.format = color_format;
			swapchain_info.width = openXRConfigViews.recommendedImageRectWidth;
			swapchain_info.height = openXRConfigViews.recommendedImageRectHeight;
			swapchain_info.sampleCount = openXRConfigViews.recommendedSwapchainSampleCount;
			swapchain_info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
			result = xrCreateSwapchain(mOpenXRSession, &swapchain_info, &xrSwapchain);
			if (XR_FAILED(result))
			{
				APE_LOG_DEBUG("xrCreateSwapchain failed " << result);
			}
			else
			{
				APE_LOG_DEBUG("xrCreateSwapchain, recommendedImageRectWidth: " << swapchain_info.width);
				APE_LOG_DEBUG("xrCreateSwapchain, recommendedImageRectHeight: " << swapchain_info.height);
			}
			uint32_t surface_count = 0;
			xrEnumerateSwapchainImages(xrSwapchain, 0, &surface_count, nullptr);
			std::vector<XrSwapchainImageOpenGLKHR> openXRSwapchainTextures;
			openXRSwapchainTextures.resize(surface_count, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR });
			result = xrEnumerateSwapchainImages(xrSwapchain, surface_count, &surface_count, (XrSwapchainImageBaseHeader*)openXRSwapchainTextures.data());
			if (XR_FAILED(result))
			{
				APE_LOG_DEBUG("xrEnumerateSwapchainImages failed " << result);
			}
			for (auto openXRSwapchainTexture : openXRSwapchainTextures)
			{
				mOpenXRSwapchainOGLTextures.push_back(openXRSwapchainTexture);
			}
			mOpenXRSwapchains.push_back(xrSwapchain);
		}
		for (auto openXRSwapchain : mOpenXRSwapchains)
		{
			APE_LOG_DEBUG("xrSwapchain: " << openXRSwapchain);
		}
		for (auto openXRSwapchainTexture : mOpenXRSwapchainOGLTextures)
		{
			APE_LOG_DEBUG("xrSwapchain image: " << openXRSwapchainTexture.image);
		}
	}
	else if (renderBackend == "DX11") {
		XrGraphicsRequirementsD3D11KHR requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
		result = mOpenXRExtensions.xrGetD3D11GraphicsRequirementsKHR(mOpenXRInstance, mOpenXRSystemID, &requirement);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrGetD3D11GraphicsRequirementsKHR failed " << result);
		}
		uint32_t blend_count = 0;
		std::vector<XrEnvironmentBlendMode> blend_modes;
		result = xrEnumerateEnvironmentBlendModes(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, 0, &blend_count, nullptr);
		blend_modes.resize(blend_count);
		result = xrEnumerateEnvironmentBlendModes(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, blend_count, &blend_count, blend_modes.data());
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrEnumerateEnvironmentBlendModes failed " << result);
		}
		for (size_t i = 0; i < blend_count; i++)
		{
			if (blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_ADDITIVE ||
				blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_OPAQUE ||
				blend_modes[i] == XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND)
			{
				mOpenXRBlend = blend_modes[i];
				break;
			}
		}
		XrGraphicsBindingD3D11KHR d3d_binding = { XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
		d3d_binding.device = (ID3D11Device*)mpCoreConfig->getWindowConfig().device;
		XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
		sessionInfo.next = &d3d_binding;
		sessionInfo.systemId = mOpenXRSystemID;
		xrCreateSession(mOpenXRInstance, &sessionInfo, &mOpenXRSession);
		if (XR_FAILED(result) || mOpenXRSession == XR_NULL_HANDLE)
		{
			APE_LOG_DEBUG("Couldn't create an OpenXR session, no MR device attached/ready? " << result);
		}
		mOpenXRRefSpace = openXRPreferredSpace();
		XrReferenceSpaceCreateInfo ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		ref_space.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };
		ref_space.referenceSpaceType = mOpenXRRefSpace;
		result = xrCreateReferenceSpace(mOpenXRSession, &ref_space, &mOpenXRAppSpace);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrCreateReferenceSpace failed " << result);
		}
		ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		ref_space.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };
		ref_space.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
		result = xrCreateReferenceSpace(mOpenXRSession, &ref_space, &mOpenXRHeadSpace);
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrCreateReferenceSpace failed " << result);
		}
		DXGI_FORMAT color_format;
		tex_format_ depth_format;
		openXRPreferredFormat(color_format, depth_format);
		uint32_t view_count = 0;
		xrEnumerateViewConfigurationViews(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, 0, &view_count, nullptr);
		mOpenXRConfigViews.resize(view_count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
		mOpenXRViews.resize(view_count, { XR_TYPE_VIEW });
		mOpenXRViewPointView.resize(view_count, { });
		mOpenXRViewPointProjection.resize(view_count, { });
		result = xrEnumerateViewConfigurationViews(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, view_count, &view_count, mOpenXRConfigViews.data());
		if (XR_FAILED(result))
		{
			APE_LOG_DEBUG("xrEnumerateViewConfigurationViews failed " << result);
		}
		for (auto openXRConfigViews : mOpenXRConfigViews)
		{
			XrSwapchainCreateInfo swapchain_info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
			XrSwapchain xrSwapchain;
			swapchain_info.arraySize = 1;
			swapchain_info.mipCount = 1;
			swapchain_info.faceCount = 1;
			swapchain_info.format = color_format;
			swapchain_info.width = openXRConfigViews.recommendedImageRectWidth;
			swapchain_info.height = openXRConfigViews.recommendedImageRectHeight;
			swapchain_info.sampleCount = openXRConfigViews.recommendedSwapchainSampleCount;
			swapchain_info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
			result = xrCreateSwapchain(mOpenXRSession, &swapchain_info, &xrSwapchain);
			if (XR_FAILED(result))
			{
				APE_LOG_DEBUG("xrCreateSwapchain failed " << result);
			}
			else
			{
				APE_LOG_DEBUG("xrCreateSwapchain, recommendedImageRectWidth: " << swapchain_info.width);
				APE_LOG_DEBUG("xrCreateSwapchain, recommendedImageRectHeight: " << swapchain_info.height);
			}
			uint32_t surface_count = 0;
			xrEnumerateSwapchainImages(xrSwapchain, 0, &surface_count, nullptr);
			std::vector<XrSwapchainImageD3D11KHR> openXRSwapchainTextures;
			openXRSwapchainTextures.resize(surface_count, { XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR });
			result = xrEnumerateSwapchainImages(xrSwapchain, surface_count, &surface_count, (XrSwapchainImageBaseHeader*)openXRSwapchainTextures.data());
			if (XR_FAILED(result))
			{
				APE_LOG_DEBUG("xrEnumerateSwapchainImages failed " << result);
			}
			for (auto openXRSwapchainTexture : openXRSwapchainTextures)
			{
				mOpenXRSwapchainDX11Textures.push_back(openXRSwapchainTexture);
			}
			mOpenXRSwapchains.push_back(xrSwapchain);
		}
		for (auto openXRSwapchain : mOpenXRSwapchains)
		{
			APE_LOG_DEBUG("xrSwapchain: " << openXRSwapchain);
		}
		for (auto openXRSwapchainTexture : mOpenXRSwapchainDX11Textures)
		{
			APE_LOG_DEBUG("xrSwapchain image: " << openXRSwapchainTexture.texture);
		}
	}
	
	
	APE_LOG_DEBUG("openxr_init OK");
	APE_LOG_DEBUG("try to set up ApertusVR rendertargets(render2textures) and cameras");
	if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("OpenXRRenderTextureLeft", ape::Entity::TEXTURE_MANUAL, false, "").lock()))
	{
		manualTexture->setParameters(mpCoreConfig->getWindowConfig().width, mpCoreConfig->getWindowConfig().height, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::RENDERTARGET, true, true, false);
		mManualTextureLeftEye = manualTexture;
	}
	if (auto manualTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity("OpenXRRenderTextureRight", ape::Entity::TEXTURE_MANUAL, false, "").lock()))
	{
		manualTexture->setParameters(mpCoreConfig->getWindowConfig().width, mpCoreConfig->getWindowConfig().height, ape::Texture::PixelFormat::R8G8B8A8, ape::Texture::Usage::RENDERTARGET, true, true, false);
		mManualTextureRightEye = manualTexture;
	}
	mCameraLeft = mpApeUserInputMacro->createCamera("OpenXRHmdLeftCamera");
	mCameraRight = mpApeUserInputMacro->createCamera("OpenXRHmdRightCamera");
	//TODO
	//vr::HmdMatrix44_t projectionLeft = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Left, 1, 10000);
	//vr::HmdMatrix44_t projectionRight = mpOpenVrSystem->GetProjectionMatrix(vr::Eye_Right, 1, 10000);
	if (auto cameraLeft = mCameraLeft.lock())
	{
		cameraLeft->setAutoAspectRatio(true);
		if (auto cameraNode = cameraLeft->getParentNode().lock())
		{
			ape::Vector3 scale;
			ape::Quaternion rotation;
			ape::Vector3 translate;
			//TODO
			//conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Left)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = mManualTextureLeftEye.lock())
			texture->setSourceCamera(cameraLeft);
		//TODO
		//cameraLeft->setProjection(conversionFromOpenVR(projectionLeft));
	}
	if (auto cameraRight = mCameraRight.lock())
	{
		cameraRight->setAutoAspectRatio(true);
		if (auto cameraNode = cameraRight->getParentNode().lock())
		{
			ape::Vector3 scale;
			ape::Quaternion rotation;
			ape::Vector3 translate;
			//TODO
			//conversionFromOpenVR(mpOpenVrSystem->GetEyeToHeadTransform(vr::Eye_Right)).makeTransform(scale, rotation, translate);
			cameraNode->setPosition(translate);
		}
		if (auto texture = mManualTextureRightEye.lock())
			texture->setSourceCamera(cameraRight);
		//TODO
		//cameraRight->setProjection(conversionFromOpenVR(projectionRight));
	}
	APE_LOG_DEBUG("init end");
}

void ape::OpenXRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("Wait while RTT textures are created...");
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (auto manualTextureRightEye = mManualTextureRightEye.lock())
		{
			if (auto manualTextureLeftEye = mManualTextureLeftEye.lock())
			{
				if ((manualTextureRightEye->getGraphicsApiID() != nullptr) && (manualTextureLeftEye->getGraphicsApiID() != nullptr))
				{
					APE_LOG_DEBUG("RTT textures are successfully created: " << manualTextureRightEye->getGraphicsApiID() << " ; " << manualTextureLeftEye->getGraphicsApiID());
					break;
				}
			}
		}
	}
	APE_LOG_DEBUG("try to run OpenXR");
	while (true)
	{
		openXRPollEvents();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OpenXRPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
