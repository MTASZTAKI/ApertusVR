{
	"targets": [
		{
			"includes": [
				"auto.gypi"
			],
			"sources": [
				"bindings.cpp"
			],
			"include_dirs": [
				".",
				"<(module_root_dir)../../../../common/include",
				"<(module_root_dir)../../common",
				"<(module_root_dir)../../../scene",
				"<(module_root_dir)../../../scene/session",
				"<(module_root_dir)../../../eventManager",
				"<(module_root_dir)../../../system",
				"<(module_root_dir)../../../../3rdParty/raknet/",
				"<(module_root_dir)../../../../3rdParty/raknet/include/RakNet"
			],
			"link_settings": {
				"libraries": [
					"C:/Users/demo/.node-gyp/0.10.46/x64/node.lib",
					"<(module_root_dir)../../../../../ApertusVR-build/core/pluginManager/Release/ApePluginManager.lib",
					"<(module_root_dir)../../../../../ApertusVR-build/core/scene/Release/ApeScene.lib",
					"<(module_root_dir)../../../../../ApertusVR-build/core/jsAPI/common/Release/ApeJsAPICommon.lib",
					"<(module_root_dir)../../../../../ApertusVR-build/core/eventManager/Release/ApeEventManager.lib",
					"<(module_root_dir)../../../../../ApertusVR-build/core/systemConfig/Release/ApeSystemConfig.lib",
					"<(module_root_dir)../../../../3rdParty/raknet/Lib/LibStatic/Release/RakNetLibStatic.lib",
					"<(module_root_dir)../../../../3rdParty/curl/lib/Release/libcurl_imp.lib",
					"ws2_32.lib"
				]
			}
		}
	],
	"includes": [
		"auto-top.gypi"
	]
}
