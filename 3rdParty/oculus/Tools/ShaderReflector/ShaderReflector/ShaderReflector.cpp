/************************************************************************************

Filename    :   ShaderReflector.cpp
Content     :   Utility for serializing out shader variables to be used by the OVR SDK
Created     :   September 10, 2012
Authors     :   Volga Aksoy

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "stdafx.h"
#include <d3d11.h>
#include <d3dcompiler.h>

int LoadFile( const char* filename, char*& charBuffer, size_t& charBufferSize )
{
    FILE* fp = 0;

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    errno_t err = fopen_s(&fp, filename, "rb" );
    if ( !fp || err)
    {
#else
    fp = fopen( filename, "rb" );
    if ( !fp)
    {
#endif
        printf("ERROR: File not found: %s\n", filename);
        return 1;
    }

    // get pointer and size to char buffer
    {
        fseek( fp, 0, SEEK_END );
        size_t size = ftell( fp );
        fseek( fp, 0, SEEK_SET );

        if ( size == 0 ) {
            return 1;
        }

        charBuffer = new char[size+1];
        charBufferSize = fread( charBuffer, 1, size, fp );
        if ( charBufferSize != size ) {
            printf("ERROR: Could not read file\n");
            return 1;
        }

        charBuffer[size] = 0;
    }
    fclose( fp );

    return 0;
}

int OpenForWrite(const char* filename, FILE*& fp)
{
    fp = NULL;

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    errno_t err = fopen_s(&fp, filename, "w+" );
    if ( !fp || err)
    {
#else
    fp = fopen( filename, "w+" );
    if ( !fp)
    {
#endif
        printf("ERROR: Failed to create file: %s\n", filename);
        return 1;
    }

    return 0;
}

std::string GetFilenameWithoutExtension(const char *filename)
{
    std::string fName(filename);
    size_t pos = fName.rfind(".");
    if(pos == std::string::npos)  //No extension.
        return fName;

    if(pos == 0)    //. is at the front. Not an extension.
        return fName;

    return fName.substr(0, pos);
}

int main( int argc, char *argv[] )
{
//#define TEST
#ifndef TEST
    if(argc != 3)
    {
        printf("ShaderReflector serializes out a binary file that contains OVR specific data for a given HLSL shader\n\n");
        printf("Usage:\n\n\tShaderReflector [compiled_shader_byte_code_file_name] [output_shader_reflection_file_name]\n\n");
        return 1;
    }

    const char* filename = argv[1];
    const char* outFilename = argv[2];

#else
    const char* filename = "DistortionTimewarpChroma_vs";
    const char* outFilename = "DistortionTimewarpChroma_vs_refl.h";
#endif

    char* shaderBuffer;
    size_t shaderBufferSize;

    if(LoadFile(filename, shaderBuffer, shaderBufferSize))
    {
        return 1;
    }

    {
        // Comes from 'dxguid.lib'
        ID3D11ShaderReflection* ref = NULL;
        D3DReflect(shaderBuffer, shaderBufferSize, IID_ID3D11ShaderReflection, (void**) &ref);

        D3D11_SHADER_BUFFER_DESC bufd;
        ID3D11ShaderReflectionConstantBuffer* buf = ref->GetConstantBufferByIndex(0);

        FILE* outFile = NULL;
        if(OpenForWrite(outFilename, outFile))
        {
            return 1;
        }
        
        std::string outFilenameNoExtStr = GetFilenameWithoutExtension(outFilename);
        const char* outFilenameNoExt = outFilenameNoExtStr.c_str();

        if (FAILED(buf->GetDesc(&bufd)))
        {
            printf("Did not get a constant buffer from shader: %s", filename);
            fprintf_s( outFile, "// No data available for shader reflection %s", outFilenameNoExt);
            fclose(outFile);
            return 0;
        }
        
        fprintf_s( outFile, "#ifndef %s\n\n", outFilenameNoExt);
        fprintf_s( outFile, "const OVR::CAPI::D3D_NS::ShaderBase::Uniform %s[] =\n{\n", outFilenameNoExt);
        
        for(unsigned i = 0; i < bufd.Variables; i++)
        {
            ID3D11ShaderReflectionVariable* var = buf->GetVariableByIndex(i);
            if (var)
            {
                D3D11_SHADER_VARIABLE_DESC vd;
                if (SUCCEEDED(var->GetDesc(&vd)))
                {
                    ID3D11ShaderReflectionType* varType = var->GetType();
                    D3D11_SHADER_TYPE_DESC vt;
                    varType->GetDesc(&vt);
                    switch(vt.Type)
                    {
                    case D3D_SVT_FLOAT:
                        fprintf_s(outFile, "\t{ \"%s\", \tOVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, %d, %d },\n", vd.Name, vd.StartOffset, vd.Size);
                        break;
                    case D3D_SVT_BOOL:
                        fprintf_s(outFile, "\t{ \"%s\", \tOVR::CAPI::D3D_NS::ShaderBase::VARTYPE_BOOL, %d, %d },\n", vd.Name, vd.StartOffset, vd.Size);
                        break;
                    default:
                        printf("Unknown variable type for variable: '%s' used in shader '%s'", vd.Name, filename);
                        return 1;
                    }
                }
            }
        }

        fprintf_s( outFile, "};\n\n#endif\n");
        
        fclose(outFile);

        printf("Successfully serialized shader reflection info: %s\n", outFilename);

    }

	return 0;
}

