{
  'target_defaults': {
    'sources': [
        'arraybuffer.cc',
        'asyncworker.cc',
        'basic_types/number.cc',
        'basic_types/value.cc',
        'binding.cc',
        'buffer.cc',
        'dataview/dataview.cc',
        'dataview/dataview_read_write.cc',
        'error.cc',
        'external.cc',
        'function.cc',
        'handlescope.cc',
        'name.cc',
        'object/delete_property.cc',
        'object/get_property.cc',
        'object/has_own_property.cc',
        'object/has_property.cc',
        'object/object.cc',
        'object/set_property.cc',
        'promise.cc',
        'typedarray.cc',
        'objectwrap.cc',
      ],
      'include_dirs': ["<!@(node -p \"require('../').include\")"],
      'dependencies': ["<!(node -p \"require('../').gyp\")"],
  },
  'targets': [
    {
      'target_name': 'binding',
      'defines': [ 'NAPI_CPP_EXCEPTIONS', 'NAPI_DATA_VIEW_FEATURE' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': {
          'ExceptionHandling': 1,
          'EnablePREfast': 'true',
        },
      },
      'xcode_settings': {
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      },
    },
    {
      'target_name': 'binding_noexcept',
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS', 'NAPI_DATA_VIEW_FEATURE' ],
      'cflags': [ '-fno-exceptions' ],
      'cflags_cc': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': {
          'ExceptionHandling': 0,
          'EnablePREfast': 'true',
        },
      },
      'xcode_settings': {
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',
      },
    },
  ],
}
