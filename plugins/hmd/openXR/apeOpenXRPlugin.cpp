#include "apeOpenXRPlugin.h"

ape::OpenXRPlugin::OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
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
	APE_LOG_FUNC_LEAVE();
}

ape::OpenXRPlugin::~OpenXRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&OpenXRPlugin::eventCallBack, this, std::placeholders::_1));
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

void ape::OpenXRPlugin::openXRPollActions()
{
	if (mOpenXRSessionState != XR_SESSION_STATE_FOCUSED)
		return;
	XrSpaceLocation space_location = { XR_TYPE_SPACE_LOCATION };
	XrResult res = xrLocateSpace(mOpenXRHeadSpace, mOpenXRAppSpace, mOpenXRTime, &space_location);
	if (XR_UNQUALIFIED_SUCCESS(res) && openXRLocValid(space_location))
	{
		//TODO put it to the camera
		//memcpy(&out_pose.position, &space_location.pose.position, sizeof(vec3));
		//memcpy(&out_pose.orientation, &space_location.pose.orientation, sizeof(quat));
	}
}

void ape::OpenXRPlugin::openXRRenderFrame() 
{
	XrFrameState frame_state = { XR_TYPE_FRAME_STATE };
	xrWaitFrame(mOpenXRSession, nullptr, &frame_state);
	xrBeginFrame(mOpenXRSession, nullptr);
	mOpenXRTime = frame_state.predictedDisplayTime + frame_state.predictedDisplayPeriod;
	XrCompositionLayerBaseHeader *layer = nullptr;
	XrCompositionLayerProjection layer_proj = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	std::vector<XrCompositionLayerProjectionView> views;
	bool session_active = mOpenXRSessionState == XR_SESSION_STATE_VISIBLE || mOpenXRSessionState == XR_SESSION_STATE_FOCUSED;
	//TODO tell that you submit to texture is open
	/*if (session_active && openxr_render_layer(frame_state.predictedDisplayTime, views, layer_proj)) 
	{
		layer = (XrCompositionLayerBaseHeader*)&layer_proj;
	}*/
	XrFrameEndInfo end_info{ XR_TYPE_FRAME_END_INFO };
	end_info.displayTime = frame_state.predictedDisplayTime;
	end_info.environmentBlendMode = mOpenXRBlend;
	end_info.layerCount = layer == nullptr ? 0 : 1;
	end_info.layers = &layer;
	xrEndFrame(mOpenXRSession, &end_info);
}

void ape::OpenXRPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::OpenXRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr && mpCoreConfig->getWindowConfig().device == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
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
	d3d_binding.device = (ID3D11Device*) mpCoreConfig->getWindowConfig().device;
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
	mOpenXRConfigViews.resize(view_count, { XR_TYPE_VIEW });
	mOpenXRViewPointView.resize(view_count, { });
	mOpenXRViewPointProjection.resize(view_count, { });
	result = xrEnumerateViewConfigurationViews(mOpenXRInstance, mOpenXRSystemID, mOpenXRAppConfigView, view_count, &view_count, mOpenXRConfigViews.data());
	if (XR_FAILED(result)) 
	{
		APE_LOG_DEBUG("xrEnumerateViewConfigurationViews failed " << result);
	}
	XrViewConfigurationView &view = mOpenXRConfigViews[0];
	XrSwapchainCreateInfo swapchain_info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
	XrSwapchain handle = {};
	swapchain_info.arraySize = view_count;
	swapchain_info.mipCount = 1;
	swapchain_info.faceCount = 1;
	swapchain_info.format = color_format;
	swapchain_info.width = view.recommendedImageRectWidth;
	swapchain_info.height = view.recommendedImageRectHeight;
	swapchain_info.sampleCount = view.recommendedSwapchainSampleCount;
	swapchain_info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
	result = xrCreateSwapchain(mOpenXRSession, &swapchain_info, &handle);
	if (XR_FAILED(result)) 
	{
		APE_LOG_DEBUG("xrCreateSwapchain failed " << result);
	}
	uint32_t surface_count = 0;
	xrEnumerateSwapchainImages(handle, 0, &surface_count, nullptr);
	std::vector<XrSwapchainImageD3D11KHR> surface_images;
	surface_images.resize(surface_count, { XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR });
	result = xrEnumerateSwapchainImages(handle, surface_count, &surface_count, (XrSwapchainImageBaseHeader*)surface_images.data());
	if (XR_FAILED(result)) 
	{
		APE_LOG_DEBUG("xrEnumerateSwapchainImages failed " << result);
	}
	for (uint32_t s = 0; s < surface_count; s++)
	{
		APE_LOG_DEBUG("renderTarget: " << s);
	}
	APE_LOG_DEBUG("openxr_init OK");
}

void ape::OpenXRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_DEBUG("try to run OpenXR");
	while (mIsOpenXRRunning)
	{
		openXRPollEvents();
		openXRPollActions();
		openXRRenderFrame();
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
