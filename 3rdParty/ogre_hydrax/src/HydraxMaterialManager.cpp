/*
--------------------------------------------------------------------------------
This source file is part of Hydrax.
Visit ---

Copyright (C) 2008 Xavier Verguín González <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Contributors:
    Jose Luis Cercós Pita <jlcercos@alumnos.upm.es>
--------------------------------------------------------------------------------
*/

#include <iostream>
#include <OgreString.h>

#include <HydraxMaterialManager.h>
#include <Hydrax.h>

#define _def_Water_Material_Name  "_Hydrax_Water_Material"
#define _def_Water_Shader_VP_Name "_Hydrax_Water_VP"
#define _def_Water_Shader_FP_Name "_Hydrax_Water_FP"

#define _def_Depth_Material_Name  "_Hydrax_Depth_Material"
#define _def_Depth_Shader_VP_Name "_Hydrax_Depth_VP"
#define _def_Depth_Shader_FP_Name "_Hydrax_Depth_FP"

#define _def_Depth_Trans_Material_Name  "_Hydrax_Depth_Trans_Material"
#define _def_Depth_Trans_Shader_VP_Name "_Hydrax_Depth_Trans_VP"
#define _def_Depth_Trans_Shader_FP_Name "_Hydrax_Depth_Trans_FP"

#define _def_DepthTexture_Shader_VP_Name "_Hydrax_DepthTexture_VP"
#define _def_DepthTexture_Shader_FP_Name "_Hydrax_DepthTexture_FP"

#define _def_Underwater_Material_Name  "_Hydrax_Underwater_Material"
#define _def_Underwater_Shader_VP_Name "_Hydrax_Underwater_Shader_VP"
#define _def_Underwater_Shader_FP_Name "_Hydrax_Underwater_Shader_FP"

#define _def_Underwater_Compositor_Material_Name  "_Hydrax_Underwater_Compositor_Material"
#define _def_Underwater_Compositor_Shader_VP_Name "_Hydrax_Underwater_Compositor_Shader_VP"
#define _def_Underwater_Compositor_Shader_FP_Name "_Hydrax_Underwater_Compositor_Shader_FP"

#define _def_Underwater_Compositor_Name "_Hydrax_Underwater_Compositor_Name"

#define _def_Simple_Red_Material_Name "_Hydrax_Simple_Red_Material"
#define _def_Simple_Black_Material_Name "_Hydrax_Simple_Black_Material"

namespace Hydrax
{

	MaterialManager::MaterialManager(Hydrax* h)
	{
    mCreated = false;
		mComponents = HYDRAX_COMPONENTS_NONE;
		mHydrax = h;

		for (int k = 0; k < 6; k++)
			mMaterials[k].setNull();

		for (int k = 0; k < 1; k++)
		{
			mCompositorsEnable[k] = false;
			mCompositorsNeedToBeReloaded[k] = false;
		}

		mUnderwaterCompositorListener.mMaterialManager = this;
	}

	MaterialManager::~MaterialManager()
	{
		removeMaterials();
	}

	void MaterialManager::removeMaterials()
	{
    Ogre::MaterialManager::getSingleton().removeListener(this, "HydraxDepth");
		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Water_Material_Name))
		{
			Ogre::MaterialManager::getSingleton().remove(_def_Water_Material_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Water_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Water_Shader_FP_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Water_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Water_Shader_FP_Name);
      mMaterials[MAT_WATER].setNull();
		}

		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Depth_Material_Name))
		{
			Ogre::MaterialManager::getSingleton().remove(_def_Depth_Material_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Depth_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Depth_Shader_FP_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Depth_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Depth_Shader_FP_Name);
      mMaterials[MAT_DEPTH].setNull();
		}

    if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Depth_Trans_Material_Name))
		{
			Ogre::MaterialManager::getSingleton().remove(_def_Depth_Trans_Material_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Depth_Trans_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Depth_Trans_Shader_FP_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Depth_Trans_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Depth_Trans_Shader_FP_Name);
      mMaterials[MAT_DEPTH_TRANS].setNull();
		}

		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Underwater_Material_Name))
		{
			Ogre::MaterialManager::getSingleton().remove(_def_Underwater_Material_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Underwater_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Underwater_Shader_FP_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Underwater_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Underwater_Shader_FP_Name);
      mMaterials[MAT_UNDERWATER].setNull();
    }

		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Simple_Red_Material_Name))
    {
	  	Ogre::MaterialManager::getSingleton().remove(_def_Simple_Red_Material_Name);
      mMaterials[MAT_SIMPLE_RED].setNull();
    }

		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Simple_Black_Material_Name))
    {
			Ogre::MaterialManager::getSingleton().remove(_def_Simple_Black_Material_Name);
      mMaterials[MAT_SIMPLE_BLACK].setNull();
    }

		Ogre::String AlphaChannels[] = {"x","y","z","w",
		                                "r","g","b","a"};

		for (int k = 0; k<8; k++)
		{
			if (Ogre::HighLevelGpuProgramManager::getSingleton().resourceExists(_def_DepthTexture_Shader_VP_Name + AlphaChannels[k]))
			{
				Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_DepthTexture_Shader_VP_Name + AlphaChannels[k]);
				Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_DepthTexture_Shader_FP_Name + AlphaChannels[k]);
			  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_DepthTexture_Shader_VP_Name + AlphaChannels[k]);
				Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_DepthTexture_Shader_FP_Name + AlphaChannels[k]);
			}
		}

    mDepthTechniques.clear();
    mDepthTransTechniques.clear();

    /*std::vector<Ogre::Technique*>::iterator TechIt;
    for(TechIt = mDepthTransTechniques.begin(); TechIt != mDepthTransTechniques.end(); TechIt++)
		{
			if (!(*TechIt))
			{
				TechIt = mDepthTransTechniques.erase(TechIt);
				continue;
			}

      Ogre::Material* dmat = (*TechIt)->getParent();
      if (dmat)
      {
        Ogre::Material::TechniqueIterator schemeTechniqueIt = dmat->getTechniqueIterator();

        unsigned int count = 0;
        while(schemeTechniqueIt.hasMoreElements())
        {
          Ogre::Technique* schemeTechnique = schemeTechniqueIt.getNext();
          if(schemeTechnique->getSchemeName() == Ogre::String("HydraxDepth"))
          {
            dmat->removeTechnique(count);
            break;
          }

          count++;
		    }
      }
    }
    mDepthTransTechniques.clear();
    */

		removeCompositor();
		mCreated = false;
	}

	void MaterialManager::removeCompositor()
	{
		if (Ogre::MaterialManager::getSingleton().resourceExists(_def_Underwater_Compositor_Material_Name))
		{
      // Remove compositor
			setCompositorEnable(COMP_UNDERWATER, false);

      Hydrax::ViewportList lviewports = mHydrax->getViewports();
      Hydrax::ViewportList::iterator iViewportSearched = lviewports.begin();
      while (iViewportSearched != lviewports.end())
      {
        Ogre::Viewport* viewport = (*iViewportSearched);

        #if OGRE_VERSION < ((1 << 16) | (7 << 8) | 0)
          // Ogre 1.6 method
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, _def_Underwater_Compositor_Name, false);
  	      Ogre::CompositorManager::getSingleton().removeCompositor(viewport, _def_Underwater_Compositor_Name);
        #else
          // New Ogre 1.7+ method
          Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(viewport);
          if (chain)
          {
            Ogre::CompositorInstance* compositorInstance = chain->getCompositor(_def_Underwater_Compositor_Name);
            if (compositorInstance)
            {
              compositorInstance->removeListener(&mUnderwaterCompositorListener);

              if (compositorInstance->getEnabled())
              {
                compositorInstance->setAlive(false);
              }
              chain->_removeInstance(compositorInstance);
              chain->_markDirty();
            }

            // Destroy chain if needed.
            if(chain->getNumCompositors() == 0)
              Ogre::CompositorManager::getSingleton().removeCompositorChain(viewport);
          }
        #endif

        iViewportSearched++;
      }

      Ogre::CompositorManager::getSingleton().remove(mCompositors[COMP_UNDERWATER]->getHandle());
      mCompositors[COMP_UNDERWATER].setNull();

      // Remove material
			Ogre::MaterialManager::getSingleton().remove(_def_Underwater_Compositor_Material_Name);

      // Remove programs
			Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Underwater_Compositor_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().unload(_def_Underwater_Compositor_Shader_FP_Name);
			Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Underwater_Compositor_Shader_VP_Name);
		  Ogre::HighLevelGpuProgramManager::getSingleton().remove(_def_Underwater_Compositor_Shader_FP_Name);
      mMaterials[MAT_UNDERWATER_COMPOSITOR].setNull();
		}
	}

	bool MaterialManager::createMaterials(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		removeMaterials();
		HydraxLOG("Creating water material...");
		if (!_createWaterMaterial(Components, normalMode))
			return false;

		mHydrax->getDecalsManager()->registerAll();
		HydraxLOG("Water material created.");

		if (_isComponent(Components, HYDRAX_COMPONENT_DEPTH))
		{
			HydraxLOG("Creating depth material...");
			if(!_createDepthMaterial(Components, normalMode))
				return false;

      if(!_createDepthMaterialTrans(Components, normalMode))
				return false;

      HydraxLOG("Depth material created.");
		}

		if (_isComponent(Components, HYDRAX_COMPONENT_UNDERWATER))
		{
			HydraxLOG("Creating underwater material...");
			if(!_createUnderwaterMaterial(Components, normalMode))
				return false;

			if(!_createUnderwaterCompositor(Components, normalMode))
				return false;

      if(!_createSimpleColorMaterial(Ogre::ColourValue::Red, MAT_SIMPLE_RED, _def_Simple_Red_Material_Name, false))
				return false;

      HydraxLOG("Underwater material created.");
		}

		mComponents = Components;
		mNormalMode = normalMode;
		mCreated = true;

		std::vector<Ogre::Technique*>::iterator TechIt;
		for(TechIt = mDepthTechniques.begin(); TechIt != mDepthTechniques.end(); TechIt++)
		{
			if (!(*TechIt))
			{
				TechIt = mDepthTechniques.erase(TechIt);
				continue;
			}

			bool isTextureDepthTechnique = ((*TechIt)->getName() == "_Hydrax_Depth_Technique") ? false : true;
			if (isTextureDepthTechnique)
			{
				Ogre::String DepthTextureName = ((*TechIt)->getPass(0)->getTextureUnitState(0)->getName() == "_DetphTexture_Hydrax") ?
                                         (*TechIt)->getPass(0)->getTextureUnitState(0)->getTextureName() :
                                         (*TechIt)->getPass(0)->getTextureUnitState(1)->getTextureName();

        // Alpha channel will be stored in pass 0 name
				addDepthTextureTechnique((*TechIt), DepthTextureName, (*TechIt)->getPass(0)->getName() , false);
			}
			else
			{
				addDepthTechnique((*TechIt), false);
			}
		}

    Ogre::MaterialManager::getSingleton().addListener(this, "HydraxDepth");
		return true;
	}

	bool MaterialManager::fillGpuProgramsToPass(Ogre::Pass* Pass, const Ogre::String GpuProgramNames[2], const Ogre::String EntryPoints[2], const Ogre::String Data[2])
	{
		GpuProgram GpuPrograms[2] = {GPUP_VERTEX, GPUP_FRAGMENT};
		for (int k = 0; k < 2; k++)
		{
      if (!createGpuProgram(GpuProgramNames[k], GpuPrograms[k], EntryPoints[k], Data[k]))
        return false;
		}

		Pass->setVertexProgram(GpuProgramNames[0]);
		Pass->setFragmentProgram(GpuProgramNames[1]);
		return true;
	}

	bool MaterialManager::createGpuProgram(const Ogre::String &Name, const GpuProgram& GPUP, const Ogre::String& EntryPoint, const Ogre::String& Data)
	{
		if (Ogre::HighLevelGpuProgramManager::getSingleton().resourceExists(Name))
		{
			HydraxLOG("Error in bool MaterialManager::createGpuProgram(): "+ Name + " exists.");
			return false;
		}

		Ogre::String Profiles[2];
		Profiles[0] = "profiles";
    Ogre::String format;

    if (Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.0 RenderSystem")
    {
      Profiles[1] = "glsles";
      format = "glsles";
    }
    else
    {
		  if (GPUP == GPUP_VERTEX)
		    Profiles[1] = "vs_1_1 arbvp1";
		  else
			  Profiles[1] = "ps_2_0 arbfp1 fp20";

      format = "cg";
    }

		Ogre::GpuProgramType GpuPType;
		if (GPUP == GPUP_VERTEX)
			GpuPType = Ogre::GPT_VERTEX_PROGRAM;
		else
			GpuPType = Ogre::GPT_FRAGMENT_PROGRAM;

		Ogre::HighLevelGpuProgramPtr HLGpuProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(Name, HYDRAX_RESOURCE_GROUP, format, GpuPType);
    HLGpuProgram->setSource(Data);

    if (Ogre::Root::getSingleton().getRenderSystem()->getName() != "OpenGL ES 2.0 RenderSystem")
      HLGpuProgram->setParameter("entry_point", EntryPoint);
    
    HLGpuProgram->setParameter(Profiles[0], Profiles[1]);
		HLGpuProgram->load();
		return true;
	}

	bool MaterialManager::_createWaterMaterial(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		const bool cDepth    = _isComponent(Components, HYDRAX_COMPONENT_DEPTH   );
		const bool cSmooth   = _isComponent(Components, HYDRAX_COMPONENT_SMOOTH  );
		const bool cSun      = _isComponent(Components, HYDRAX_COMPONENT_SUN     );
		const bool cFoam     = _isComponent(Components, HYDRAX_COMPONENT_FOAM    );
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// Vertex program
		switch (normalMode)
		{
      case NM_TEXTURE:
      {
	      VertexProgramData += Ogre::String("void main_vp(\n") +
						                              // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          "float2 iUv               : TEXCOORD0,\n" +
                                          // OUT
                                          "out float4 oPosition      : POSITION,\n" +
							                            "out float4 oPosition_     : TEXCOORD0,\n" +
                                          "out float2 oUvNoise       : TEXCOORD1,\n" +
                                          "out float4 oUvProjection  : TEXCOORD2,\n";
			  if (cFoam)
          VertexProgramData += Ogre::String("out float4 oWorldPosition : TEXCOORD3,\n") +
                                            "uniform float4x4 uWorld,\n";

        VertexProgramData += Ogre::String("uniform float            uFlip,\n");
			  VertexProgramData += Ogre::String("uniform float4x4 uWorldViewProj)\n") +
                                          "{\n" +
            	                            "  oPosition_  = iPosition;\n";

			  if (cFoam)
          VertexProgramData += Ogre::String("  oWorldPosition = mul(uWorld, iPosition);\n");

        VertexProgramData += Ogre::String("  oPosition = mul(uWorldViewProj, iPosition);\n") +
           	                              // Projective texture coordinates, adjust for mapping
            	                            "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
                                          "                               0, (0.5 * (uFlip * -1))),   0, 0.5,"+
            				                      "                               0,   0, 0.5, 0.5,"+
                                          "                               0,   0,   0,   1);\n" +
                                          "  oUvProjection = mul(scalemat, oPosition);\n" +
                                          "  oUvNoise = iUv;\n" +
                                          "}\n";
      }
      break;

			case NM_VERTEX:
		  {
			  VertexProgramData += Ogre::String("void main_vp(\n") +
                                          // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          "float3 iNormal           : NORMAL,\n"+
                                          // OUT
                                          "out float4 oPosition     : POSITION,\n" +
                                          "out float4 oPosition_    : TEXCOORD0,\n" +
                                          "out float4 oUvProjection : TEXCOORD1,\n" +
                                          "out float3 oNormal       : TEXCOORD2,\n";

        if (cFoam)
          VertexProgramData += Ogre::String("out float4 oWorldPosition : TEXCOORD3,\n") +
                                            "uniform float4x4 uWorld,\n";

        VertexProgramData += Ogre::String("uniform float            uFlip,\n");
        VertexProgramData += Ogre::String("uniform float4x4 uWorldViewProj)\n") +
         	                                "{\n" +
            	                            "  oPosition_  = iPosition;\n";

			  if (cFoam)
          VertexProgramData += Ogre::String("  oWorldPosition = mul(uWorld, iPosition);\n");

			  VertexProgramData += Ogre::String("  oPosition = mul(uWorldViewProj, iPosition);\n") +
                                          // Projective texture coordinates, adjust for mapping
            	                            "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
                                          "                               0, (0.5 * (uFlip * (-1))),   0, 0.5,"+
            				                      "                               0,   0, 0.5, 0.5,"+
            				                      "                               0,   0,   0,   1);\n" +
                                          "  oUvProjection = mul(scalemat, oPosition);\n" +
                                          "  oNormal = normalize(iNormal);\n"+
                                          "}\n";
	    }
	    break;

			case NM_RTT:
		  {
				VertexProgramData += Ogre::String("void main_vp(\n") + 
                                          // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          // OUT
                                          "out float4 oPosition     : POSITION,\n" +
                                          "out float4 oPosition_    : TEXCOORD0,\n" +
                                          "out float4 oUvProjection : TEXCOORD1,\n";

			  if (cFoam)
          VertexProgramData += Ogre::String("out float4 oWorldPosition : TEXCOORD2,\n")+
                                            "uniform float4x4 uWorld,\n";
    
        VertexProgramData += Ogre::String("uniform float            uFlip,\n");
        VertexProgramData += Ogre::String("uniform float4x4 uWorldViewProj)\n") + // UNIFORM
       	                                  "{\n" +
          	                              "  oPosition_  = iPosition;\n";

				if (cFoam)
          VertexProgramData += Ogre::String("  oWorldPosition = mul(uWorld, iPosition);\n");

			  VertexProgramData += Ogre::String("  oPosition = mul(uWorldViewProj, iPosition);\n") +
                                          // Projective texture coordinates, adjust for mapping
              	                          "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
                                          "                               0, (0.5 * (uFlip * (-1))),   0, 0.5,"+
              				                    "                               0,   0, 0.5, 0.5,"+
              				                    "                               0,   0,   0,   1);\n" +
             	                            "  oUvProjection = mul(scalemat, oPosition);\n" +
           	                              "}\n";
			}
		  break;
		}

		// Fragment program
		FragmentProgramData += Ogre::String("float3 expand(float3 v)\n") +
						                            "{\n" +
	                                      "  return (v - 0.5) * 2;\n" +
							                          "}\n\n" + 
                                        "void main_fp(" +
                                        // IN
                                        "float4 iPosition      : TEXCOORD0,\n";

    int TEXCOORDNUM = 1;
		if (normalMode == NM_TEXTURE)
		{
			FragmentProgramData += Ogre::String("float2 iUvNoise     : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
			TEXCOORDNUM++;
		}

		FragmentProgramData += Ogre::String("float4 iUvProjection  : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
		TEXCOORDNUM++;
		
    if (normalMode == NM_VERTEX)
		{
			FragmentProgramData += Ogre::String("float4 iNormal      : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
			TEXCOORDNUM++;
		}

		if (cFoam)
			FragmentProgramData += Ogre::String("float4 iWorldPosition : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");

    FragmentProgramData += Ogre::String("out float4 oColor    : COLOR,\n") + // OUT
                                        // UNIFORM
                                        "uniform float3       uEyePosition,\n" +
                                        "uniform float        uFullReflectionDistance,\n" +
                                        "uniform float        uGlobalTransparency,\n" +
                                        "uniform float        uNormalDistortion,\n";

    if (cDepth)
      FragmentProgramData += Ogre::String("uniform float3       uWaterColor,\n");

    if (cSmooth)
    	FragmentProgramData += Ogre::String("uniform float        uSmoothPower,\n");

		if (cSun)
		{
			FragmentProgramData += Ogre::String("uniform float3       uSunPosition,\n") +
                                          "uniform float        uSunStrength,\n" +
                                          "uniform float        uSunArea,\n" +
                                          "uniform float3       uSunColor,\n";
		}

    if (cFoam)
		{
			FragmentProgramData += Ogre::String("uniform float        uFoamRange,\n") +
                                          "uniform float        uFoamMaxDistance,\n" +
                                          "uniform float        uFoamScale,\n" +
                                          "uniform float        uFoamStart,\n" +
                                          "uniform float        uFoamTransparency,\n";
		}

		if (cCaustics)
			FragmentProgramData += Ogre::String("uniform float        uCausticsPower,\n");

		int TexNum = 0;
		if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
		{
      FragmentProgramData += Ogre::String("uniform sampler2D    uNormalMap       : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
			TexNum++;
		}

		FragmentProgramData += Ogre::String("uniform sampler2D    uReflectionMap   : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n") +
                                        "uniform sampler2D    uRefractionMap   : register(s" + Ogre::StringConverter::toString(TexNum+1) + "),\n";
		TexNum += 2;

		if (cDepth)
		{
			FragmentProgramData += Ogre::String("uniform sampler2D    uDepthMap        : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
			TexNum++;
		}

    FragmentProgramData += Ogre::String("uniform sampler2D    uFresnelMap      : register(s" + Ogre::StringConverter::toString(TexNum) + ")");
	  TexNum++;

		if (cFoam)
			FragmentProgramData += Ogre::String(",\nuniform sampler2D    uFoamMap         : register(s" + Ogre::StringConverter::toString(TexNum) + ")\n");

		FragmentProgramData += Ogre::String(")\n") +
							                          "{\n" +
                                        "  float2 ProjectionCoord = iUvProjection.xy / iUvProjection.w;\n" +
                                        "  float3 camToSurface = iPosition.xyz - uEyePosition;\n" +
                                        "  float additionalReflection=camToSurface.x*camToSurface.x+camToSurface.z*camToSurface.z;\n";

    // Calculate the foam visibility as a function fo distance specified by user
		if (cFoam)
			FragmentProgramData += Ogre::String("  float foamVisibility=1.0f-saturate(additionalReflection/uFoamMaxDistance);\n");

		FragmentProgramData += Ogre::String("  additionalReflection/=uFullReflectionDistance;\n") +
                                        "  camToSurface=normalize(-camToSurface);\n";
						
    if (normalMode == NM_TEXTURE)
		{
			FragmentProgramData += Ogre::String("  float3 pixelNormal = tex2D(uNormalMap,iUvNoise);\n") +
			                                    // Inverte y with z, because at creation our local normal to the plane was z
				                                  "  pixelNormal.yz=pixelNormal.zy;\n" +
				                                  // Remap from [0,1] to [-1,1]
				                                  "  pixelNormal.xyz=expand(pixelNormal.xyz);\n";
		}
		else if (normalMode == NM_VERTEX)
		{
			FragmentProgramData += Ogre::String("  float3 pixelNormal = iNormal;\n");
		}
		else // NM_RTT
		{
			FragmentProgramData += Ogre::String("  float3 pixelNormal = 2.0*tex2D(uNormalMap, ProjectionCoord.xy) - 1.0;\n");
		}

    FragmentProgramData += Ogre::String("  float2 pixelNormalModified = uNormalDistortion*pixelNormal.zx;\n");

	  if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
	  {
		  FragmentProgramData += Ogre::String("  float dotProduct=dot(camToSurface,pixelNormal);\n");
	  }
	  else
	  {
		  FragmentProgramData += Ogre::String("  float dotProduct=dot(-camToSurface,pixelNormal);\n");
	  }

    FragmentProgramData += Ogre::String("  dotProduct=saturate(dotProduct);\n") +
                                        "  float fresnel = tex2D(uFresnelMap,float2(dotProduct,0));\n" +
		                                    // Add additional reflection and saturate
		                                    "  fresnel+=additionalReflection;\n" +
		                                    "  fresnel=saturate(fresnel);\n" +
		                                    // Decrease the transparency and saturate
		                                    "  fresnel-=uGlobalTransparency;\n" +
                                        "  fresnel=saturate(fresnel);\n" +
                                        // Get the reflection/refraction pixels. Make sure to disturb the texcoords by pixelnormal
							                          "  float3 reflection=tex2D(uReflectionMap,ProjectionCoord.xy+pixelNormalModified);\n" +
							                          "  float3 refraction=tex2D(uRefractionMap,ProjectionCoord.xy-pixelNormalModified);\n";

    if (cDepth)
    {
      if (cCaustics)
      {
        FragmentProgramData += Ogre::String("  float2 depth = tex2D(uDepthMap,ProjectionCoord.xy-pixelNormalModified).rg;\n") +
                                            "  refraction *= 1.0 + (depth.y * uCausticsPower * depth.x);\n" +
                                            "  refraction = lerp(uWaterColor,refraction,depth.x);\n";
      }
      else
      {
        FragmentProgramData += Ogre::String("  float depth = tex2D(uDepthMap,ProjectionCoord.xy-pixelNormalModified).r;\n") +
                                            "  refraction = lerp(uWaterColor,refraction,depth);\n";
      }
    }

		FragmentProgramData += Ogre::String("  oColor = float4(lerp(refraction,reflection,fresnel),1);\n");

		if (cSun)
			FragmentProgramData += Ogre::String("  float3 relfectedVector = normalize(reflect(-camToSurface,pixelNormal.xyz));\n") +
			                                    "  float3 surfaceToSun=normalize(uSunPosition-iPosition.xyz);\n" +
				                                  "  float3 sunlight = uSunStrength*pow(saturate(dot(relfectedVector,surfaceToSun)),uSunArea)*uSunColor;\n" +
				                                  "  oColor.xyz+=sunlight;\n";

		if (cFoam)
			FragmentProgramData += Ogre::String("  float hmap = iPosition.y/uFoamRange*foamVisibility;\n") +
				                                  "  float2 foamTex=iWorldPosition.xz*uFoamScale+pixelNormalModified;\n" +
				                                  "  float foam=tex2D(uFoamMap,foamTex).r;\n" +
				                                  "  float foamTransparency=saturate(hmap-uFoamStart)*uFoamTransparency;\n" +
				                                  "  oColor.xyz=lerp(oColor.xyz,1,foamTransparency*foam);\n";

		if (cSmooth)
			FragmentProgramData += Ogre::String("  oColor.xyz = lerp(tex2D(uRefractionMap,ProjectionCoord.xy).xyz,oColor.xyz,saturate((1-tex2D(uDepthMap,ProjectionCoord.xy).r)*uSmoothPower));\n");

    FragmentProgramData += "}\n";

		// Build our material
		Ogre::MaterialPtr &WaterMaterial = getMaterial(MAT_WATER);
		WaterMaterial = Ogre::MaterialManager::getSingleton().create(_def_Water_Material_Name, HYDRAX_RESOURCE_GROUP);
    WaterMaterial->setReceiveShadows(false);

		Ogre::Pass *WM_Technique0_Pass0 = WaterMaterial->getTechnique(0)->getPass(0);
		WM_Technique0_Pass0->setCullingMode(Ogre::CULL_NONE);
		WM_Technique0_Pass0->setDepthWriteEnabled(true);

		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_Water_Shader_VP_Name, _def_Water_Shader_FP_Name};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		fillGpuProgramsToPass(WM_Technique0_Pass0, GpuProgramNames, EntryPoints, GpuProgramsData);
		Ogre::GpuProgramParametersSharedPtr VP_Parameters = WM_Technique0_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = WM_Technique0_Pass0->getFragmentProgramParameters();
    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

    if (cFoam)
      VP_Parameters->setNamedAutoConstant("uWorld",     Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);

    FP_Parameters->setNamedAutoConstant("uEyePosition",   Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
		FP_Parameters->setNamedConstant("uFullReflectionDistance", mHydrax->getFullReflectionDistance());
		FP_Parameters->setNamedConstant("uGlobalTransparency",     mHydrax->getGlobalTransparency());
		FP_Parameters->setNamedConstant("uNormalDistortion",       mHydrax->getNormalDistortion());
    if (VP_Parameters->_findNamedConstantDefinition("uFlip"))
      VP_Parameters->setNamedAutoConstant("uFlip",             Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING);

		if (cDepth)
			FP_Parameters->setNamedConstant("uWaterColor", mHydrax->getWaterColor());

		if (cSmooth)
			FP_Parameters->setNamedConstant("uSmoothPower", mHydrax->getSmoothPower());

    if (cSun)
		{
			FP_Parameters->setNamedConstant("uSunPosition", mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getSunPosition()));
			FP_Parameters->setNamedConstant("uSunStrength", mHydrax->getSunStrength());
			FP_Parameters->setNamedConstant("uSunArea",     mHydrax->getSunArea());
			FP_Parameters->setNamedConstant("uSunColor",    mHydrax->getSunColor());
		}

    if (cFoam)
		{
			FP_Parameters->setNamedConstant("uFoamRange",        mHydrax->getMesh()->getOptions().MeshStrength);
			FP_Parameters->setNamedConstant("uFoamMaxDistance",  mHydrax->getFoamMaxDistance());
			FP_Parameters->setNamedConstant("uFoamScale",        mHydrax->getFoamScale());
			FP_Parameters->setNamedConstant("uFoamStart",        mHydrax->getFoamStart());
			FP_Parameters->setNamedConstant("uFoamTransparency", mHydrax->getFoamTransparency());
		}

    if (cCaustics)
			FP_Parameters->setNamedConstant("uCausticsPower",    mHydrax->getCausticsPower());

		if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
      WM_Technique0_Pass0->createTextureUnitState("HydraxNormalMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

    WM_Technique0_Pass0->createTextureUnitState("HydraxReflectionMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    WM_Technique0_Pass0->createTextureUnitState("HydraxRefractionMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		if (cDepth)
    	WM_Technique0_Pass0->createTextureUnitState("HydraxDepthMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    WM_Technique0_Pass0->createTextureUnitState("Fresnel.bmp")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		if (cFoam)
      WM_Technique0_Pass0->createTextureUnitState("Foam.png")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		WaterMaterial->setReceiveShadows(false);
		WaterMaterial->load();
		return true;
	}

	bool MaterialManager::_createDepthMaterial(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// No caustics
		if (!cCaustics)
		{
      VertexProgramData += Ogre::String("void main_vp(\n") +
			                                  // IN
			                                  "float4 iPosition         : POSITION,\n" +
				                                // OUT
				                                "out float4 oPosition     : POSITION,\n" +
				                                "out float  oPosition_    : TEXCOORD0,\n" +
				                                "out float2 oDistance     : TEXCOORD1,\n" +
				                                // UNIFORM
				                                "uniform float            uPlaneYPos,\n" +
                                        "uniform float            uPlanesError,\n" +
				                                "uniform float3           uCameraPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldView,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "  {\n" +
                                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                        "  float3 wPos = mul(uWorld, iPosition).xyz;\n" +
                                        "  float3 wCam = uCameraPos;\n" +
                                        "  wPos.y    -= uPlaneYPos;\n" +
                                        "  wCam.y    -= uPlaneYPos;\n" +
                                        "  oPosition_ = wPos.y;\n" +
                                        // Distance
                                        "  oDistance.x  = distance(wPos, wCam);\n" +
                                        "  oDistance.y  = 1.0;\n" +
                                        // If exist water plane between points, occlusion effect must be changed.
                                        "  if(wCam.y > 0.0)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= 1.0 - wCam.y/(wCam.y + abs(wPos.y));\n" +
                                        "  }\n" +
                                        "  if(wPos.y > uPlanesError)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= abs(wCam.y)/(abs(wCam.y) + wPos.y);\n" +
                                        "    oDistance.y = 0.0;\n" +
                                        "  }\n" +
				                                "}\n";
		}
		else // Caustics
		{
			VertexProgramData += Ogre::String("void main_vp(\n") +
			                                  // IN
			                                  "float4 iPosition         : POSITION,\n" +
				                                // OUT
				                                "out float4 oPosition     : POSITION,\n" +
				                                "out float  oPosition_    : TEXCOORD0,\n" +
				                                "out float2 oDistance     : TEXCOORD1,\n" +
					                              "out float2 oUvWorld      : TEXCOORD2,\n" +
				                                // UNIFORM
				                                "uniform float            uPlaneYPos,\n" +
                                        "uniform float            uPlanesError,\n" +
				                                "uniform float3           uCameraPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "{\n" +
                                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                        "  float3 wPos = mul(uWorld, iPosition).xyz;\n" +
                                        "  float3 wCam = uCameraPos;\n" +
                                        "  wPos.y    -= uPlaneYPos;\n" +
                                        "  wCam.y    -= uPlaneYPos;\n" +
                                        "  oPosition_ = wPos.y;\n" +
                                        // Distance
                                        "  oDistance.x  = distance(wPos, wCam);\n" +
                                        "  oDistance.y  = 1.0;\n" +
                                        // If exist water plane between points, occlusion effect must be changed.
                                        "  if(wCam.y > 0.0)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= 1.0 - wCam.y/(wCam.y + abs(wPos.y));\n" +
                                        "  }\n" +
                                        "  if(wPos.y > uPlanesError)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= abs(wCam.y)/(abs(wCam.y) + wPos.y);\n" +
                                        "    oDistance.y = 0.0;\n" +
                                        "  }\n" +
                                        "  oUvWorld = wPos.xz;\n" +
				                                "}\n";
		}

		// Fragment program
		// No caustics
		if (!cCaustics)
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
				                                  // IN
				                                  "float  iPosition     : TEXCOORD0,\n" +
				                                  "float2 iDistance     : TEXCOORD1,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform float        uDistLimit)\n" +
				                                  "{\n" +
				                                  "  float pixelYDepth = saturate(iPosition*uDepthLimit + 1.0);\n" +
				                                  "  float pixelDepth  = saturate(1.0 - iDistance.x*uDistLimit);\n" +
					                                "  pixelDepth = min(pixelYDepth, pixelDepth);\n" +
					                                "  oColor = float4(pixelDepth,0,0,0);\n" +
				                                  "}\n";
		}
		else // Caustics
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
				                                  // IN
				                                  "float  iPosition     : TEXCOORD0,\n" +
				                                  "float2 iDistance     : TEXCOORD1,\n" +
                                          "float2 iUvWorld      : TEXCOORD2,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform float        uDistLimit,\n" +
					                                "uniform float        uCausticsScale,\n" +
                                          "uniform float        uCausticsEnd,\n" +
					                                "uniform sampler2D    uCaustics : register(s0))\n" +
				                                  "{\n" +
				                                  "  float pixelYDepth = saturate(iPosition*uDepthLimit + 1.0);\n" +
				                                  "  float pixelDepth  = saturate(1.0 - iDistance.x*uDistLimit);\n" +
				                                  "  pixelDepth = min(pixelYDepth, pixelDepth);\n" +
				                                  "  oColor = float4(pixelDepth,0.0,0.0,0.0);\n" +
				                                  "  oColor.g = iDistance.y*saturate((pixelDepth-uCausticsEnd)/(1.0-uCausticsEnd)) * tex2D(uCaustics, iUvWorld/uCausticsScale).x;\n" +
				                                  "}\n";
		}

		// Build our material
		Ogre::MaterialPtr &DepthMaterial = getMaterial(MAT_DEPTH);
		DepthMaterial = Ogre::MaterialManager::getSingleton().create(_def_Depth_Material_Name, HYDRAX_RESOURCE_GROUP);
		DepthMaterial->getTechnique(0)->setSchemeName("HydraxDepth");

		Ogre::Pass *DM_Technique0_Pass0 = DepthMaterial->getTechnique(0)->getPass(0);
		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_Depth_Shader_VP_Name, _def_Depth_Shader_FP_Name};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		fillGpuProgramsToPass(DM_Technique0_Pass0, GpuProgramNames, EntryPoints, GpuProgramsData);
		Ogre::GpuProgramParametersSharedPtr VP_Parameters = DM_Technique0_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = DM_Technique0_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    // VP_Parameters->setNamedAutoConstant("uWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
    VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
    VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
		VP_Parameters->setNamedConstant("uPlaneYPos", mHydrax->getPosition().y);
		VP_Parameters->setNamedConstant("uPlanesError", (float) mHydrax->getPlanesError());

		FP_Parameters->setNamedConstant("uDepthLimit", 1.f/mHydrax->getDepthLimit());
		FP_Parameters->setNamedConstant("uDistLimit", 1.f/mHydrax->getDistLimit());

		if (cCaustics)
		{
			FP_Parameters->setNamedConstant("uCausticsScale", mHydrax->getCausticsScale());
			FP_Parameters->setNamedConstant("uCausticsEnd", 1.f - mHydrax->getCausticsEnd());
			Ogre::TextureUnitState *TUS_Caustics = DM_Technique0_Pass0->createTextureUnitState("Caustics.bmp");
			TUS_Caustics->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			TUS_Caustics->setAnimatedTextureName("Caustics.bmp", 32, 1.5);
		}

		DepthMaterial->setReceiveShadows(false);
		DepthMaterial->load();
		return true;
	}

  bool MaterialManager::_createDepthMaterialTrans(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// No caustics
		if (!cCaustics)
		{
      VertexProgramData += Ogre::String("void main_vp(\n") +
			                                  // IN
			                                  "float4 iPosition         : POSITION,\n" +
                                        "float4 iUv               : TEXCOORD0,\n" +
				                                // OUT
				                                "out float4 oPosition     : POSITION,\n" +
				                                "out float  oPosition_    : TEXCOORD0,\n" +
				                                "out float2 oDistance     : TEXCOORD1,\n" +
                                        "out float2 oUV           : TEXCOORD2,\n" +
				                                // UNIFORM
				                                "uniform float            uPlaneYPos,\n" +
                                        "uniform float            uPlanesError,\n" +
				                                "uniform float3           uCameraPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldView,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "  {\n" +
                                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                        "  oUV = iUv.xy;\n" +
                                        "  float3 wPos = mul(uWorld, iPosition).xyz;\n" +
                                        "  float3 wCam = uCameraPos;\n" +
                                        "  wPos.y    -= uPlaneYPos;\n" +
                                        "  wCam.y    -= uPlaneYPos;\n" +
                                        "  oPosition_ = wPos.y;\n" +
                                        // Distance
                                        "  oDistance.x  = distance(wPos, wCam);\n" +
                                        "  oDistance.y  = 1.0;\n" +
                                        // If exist water plane between points, occlusion effect must be changed.
                                        "  if(wCam.y > 0.0)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= 1.0 - wCam.y/(wCam.y + abs(wPos.y));\n" +
                                        "  }\n" +
                                        "  if(wPos.y > uPlanesError)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= abs(wCam.y)/(abs(wCam.y) + wPos.y);\n" +
                                        "    oDistance.y = 0.0;\n" +
                                        "  }\n" +
				                                "}\n";
		}
		else // Caustics
		{
			VertexProgramData += Ogre::String("void main_vp(\n") +
			                                  // IN
			                                  "float4 iPosition         : POSITION,\n" +
                                        "float4 iUv               : TEXCOORD0,\n" +
				                                // OUT
				                                "out float4 oPosition     : POSITION,\n" +
				                                "out float  oPosition_    : TEXCOORD0,\n" +
				                                "out float2 oDistance     : TEXCOORD1,\n" +
					                              "out float2 oUvWorld      : TEXCOORD2,\n" +
                                        "out float2 oUV           : TEXCOORD3,\n" +
				                                // UNIFORM
				                                "uniform float            uPlaneYPos,\n" +
                                        "uniform float            uPlanesError,\n" +
				                                "uniform float3           uCameraPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "{\n" +
                                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                        "  oUV = iUv.xy;\n" +
                                        "  float3 wPos = mul(uWorld, iPosition).xyz;\n" +
                                        "  float3 wCam = uCameraPos;\n" +
                                        "  wPos.y    -= uPlaneYPos;\n" +
                                        "  wCam.y    -= uPlaneYPos;\n" +
                                        "  oPosition_ = wPos.y;\n" +
                                        // Distance
                                        "  oDistance.x  = distance(wPos, wCam);\n" +
                                        "  oDistance.y  = 1.0;\n" +
                                        // If exist water plane between points, occlusion effect must be changed.
                                        "  if(wCam.y > 0.0)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= 1.0 - wCam.y/(wCam.y + abs(wPos.y));\n" +
                                        "  }\n" +
                                        "  if(wPos.y > uPlanesError)\n" +
                                        "  {\n" +
                                        "    oDistance.x *= abs(wCam.y)/(abs(wCam.y) + wPos.y);\n" +
                                        "    oDistance.y = 0.0;\n" +
                                        "  }\n" +
                                        "  oUvWorld = wPos.xz;\n" +
				                                "}\n";
		}

		// Fragment program
		// No caustics
		if (!cCaustics)
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
				                                  // IN
				                                  "float  iPosition     : TEXCOORD0,\n" +
				                                  "float2 iDistance     : TEXCOORD1,\n" +
                                          "float2 iUV           : TEXCOORD2,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform float        uDistLimit,\n" +
                                          "uniform sampler2D    uAlpha : register(s0))\n" +
				                                  "{\n" +
                                          "  float alpha = tex2D(uAlpha, iUV.xy).a;\n" +
	                                        "  if(alpha < 0.5)\n" +
	                                        "    discard;\n" +
				                                  "  float pixelYDepth = saturate(iPosition*uDepthLimit + 1.0);\n" +
				                                  "  float pixelDepth  = saturate(1.0 - iDistance.x*uDistLimit);\n" +
					                                "  pixelDepth = min(pixelYDepth, pixelDepth);\n" +
					                                "  oColor = float4(pixelDepth, 0.0, 0.0, 1.0);\n" +
				                                  "}\n";
		}
		else // Caustics
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
				                                  // IN
				                                  "float  iPosition     : TEXCOORD0,\n" +
				                                  "float2 iDistance     : TEXCOORD1,\n" +
                                          "float2 iUvWorld      : TEXCOORD2,\n" +
                                          "float2 iUV           : TEXCOORD3,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform float        uDistLimit,\n" +
					                                "uniform float        uCausticsScale,\n" +
                                          "uniform float        uCausticsEnd,\n" +
                                          "uniform sampler2D    uAlpha : register(s0),\n" +
					                                "uniform sampler2D    uCaustics : register(s1))\n" +
				                                  "{\n" +
                                          "  float alpha = tex2D(uAlpha, iUV.xy).a;\n" +
	                                        "  if(alpha < 0.5)\n" +
	                                        "    discard;\n" +
				                                  "  float pixelYDepth = saturate(iPosition*uDepthLimit + 1.0);\n" +
				                                  "  float pixelDepth  = saturate(1.0 - iDistance.x*uDistLimit);\n" +
				                                  "  pixelDepth = min(pixelYDepth, pixelDepth);\n" +
				                                  "  oColor = float4(pixelDepth,0.0,0.0,1.0);\n" +
				                                  "  oColor.g = iDistance.y*saturate((pixelDepth-uCausticsEnd)/(1.0-uCausticsEnd)) * tex2D(uCaustics, iUvWorld/uCausticsScale).x;\n" +
				                                  "}\n";
		}

		// Build our material
		Ogre::MaterialPtr &DepthMaterial = getMaterial(MAT_DEPTH_TRANS);
		DepthMaterial = Ogre::MaterialManager::getSingleton().create(_def_Depth_Trans_Material_Name, HYDRAX_RESOURCE_GROUP);
		DepthMaterial->getTechnique(0)->setSchemeName("HydraxDepth");

		Ogre::Pass *DM_Technique0_Pass0 = DepthMaterial->getTechnique(0)->getPass(0);
		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_Depth_Trans_Shader_VP_Name, _def_Depth_Trans_Shader_FP_Name};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		fillGpuProgramsToPass(DM_Technique0_Pass0, GpuProgramNames, EntryPoints, GpuProgramsData);
		Ogre::GpuProgramParametersSharedPtr VP_Parameters = DM_Technique0_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = DM_Technique0_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    // VP_Parameters->setNamedAutoConstant("uWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
    VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
    VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
		VP_Parameters->setNamedConstant("uPlaneYPos", mHydrax->getPosition().y);
		VP_Parameters->setNamedConstant("uPlanesError", (float) mHydrax->getPlanesError());

		FP_Parameters->setNamedConstant("uDepthLimit", 1.f/mHydrax->getDepthLimit());
		FP_Parameters->setNamedConstant("uDistLimit", 1.f/mHydrax->getDistLimit());

    // trans unit
    DM_Technique0_Pass0->createTextureUnitState();

		if (cCaustics)
		{
			FP_Parameters->setNamedConstant("uCausticsScale", mHydrax->getCausticsScale());
			FP_Parameters->setNamedConstant("uCausticsEnd", 1.f - mHydrax->getCausticsEnd());
			Ogre::TextureUnitState *TUS_Caustics = DM_Technique0_Pass0->createTextureUnitState("Caustics.bmp");
			TUS_Caustics->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			TUS_Caustics->setAnimatedTextureName("Caustics.bmp", 32, 1.5);
		}

		DepthMaterial->setReceiveShadows(false);
		DepthMaterial->load();
		return true;
	}

	bool MaterialManager::_createDepthTextureGPUPrograms(const HydraxComponent &Components, const NormalMode& normalMode, const Ogre::String& AlphaChannel)
	{
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// Vertex program
		// No caustics
		if (!cCaustics)
		{
	    VertexProgramData += Ogre::String("void main_vp(\n") +
		                                    // IN
		                                    "float4 iPosition         : POSITION,\n" +
				                                "float2 iUV               : TEXCOORD0,\n" +
			                                  // OUT
			                                  "out float4 oPosition     : POSITION,\n" +
			                                  "out float3 oPosition_UV  : TEXCOORD0,\n" +
			                                  // UNIFORM
			                                  "uniform float            uPlaneYPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "{\n" +
                                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                        "  oPosition_UV.x = mul(uWorld, iPosition).y;\n" +
                                        "  oPosition_UV.x-=uPlaneYPos;\n" +
                                        "  oPosition_UV.yz = iUV;\n" +
				                                "}\n";
		}
		else // Caustics
		{
			VertexProgramData += Ogre::String("void main_vp(\n") +
			                                  // IN
			                                  "float4 iPosition         : POSITION,\n" +
					                              "float2 iUV               : TEXCOORD0,\n" +
				                                // OUT
				                                "out float4 oPosition     : POSITION,\n" +
				                                "out float3 oPosition_UV  : TEXCOORD0,\n" +
					                              "out float2 oUvWorld      : TEXCOORD1,\n" +
				                                // UNIFORM
				                                "uniform float            uPlaneYPos,\n" +
                                        "uniform float4x4         uWorld,\n" +
                                        "uniform float4x4         uWorldViewProj)\n" +
			                                  "{\n" +
			                                  "  oPosition = mul(uWorldViewProj, iPosition);\n" +
				                                "  float3 wPos = mul(uWorld, iPosition);\n" +
				                                "  oPosition_UV.x = wPos.y;\n" +
				                                "  oPosition_UV.x-=uPlaneYPos;\n" +
				                                "  oPosition_UV.yz = iUV;\n" +
				                                "  oUvWorld = wPos.xz;\n" +
				                                "}\n";
		}

		// Fragment program
		// No caustics
		if (!cCaustics)
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
				                                  // IN
				                                  "float3 iPosition_UV  : TEXCOORD0,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform sampler2D    uAlphaTex : register(s0))\n" +
				                                  "{\n" +
				                                  "  float pixelYDepth = (iPosition_UV.x*uDepthLimit+1);\n" +
					                                "  pixelYDepth = saturate(pixelYDepth);\n" +
					                                "  oColor = float4(pixelYDepth,0,0,0);\n" +
					                                "  oColor.a = tex2D(uAlphaTex, iPosition_UV.yz)."+AlphaChannel+";" +
				                                  "}\n";
		}
		else // Caustics
		{
			FragmentProgramData += Ogre::String("void main_fp(\n") +
		                                      // IN
		                                      "float3 iPosition_UV  : TEXCOORD0,\n" +
                                          "float2 iUvWorld      : TEXCOORD1,\n" +
					                                // OUT
					                                "out float4 oColor    : COLOR,\n" +
					                                // UNIFORM
					                                "uniform float        uDepthLimit,\n" +
					                                "uniform float        uCausticsScale,\n" +
                                          "uniform float        uCausticsEnd,\n" +
					                                "uniform sampler2D    uCaustics : register(s0),\n" +
					                                "uniform sampler2D    uAlphaTex : register(s1))\n" +
				                                  "{\n" +
                                          "  float pixelYDepth = (iPosition_UV.x*uDepthLimit+1);\n" +
                                          "  pixelYDepth = saturate(pixelYDepth);\n" +
                                          "  oColor = float4(pixelYDepth,0,0,0);\n" +
                                          "  oColor.g = saturate(uCausticsEnd-pixelYDepth)*tex2D(uCaustics, iUvWorld/uCausticsScale).r;\n" +
                                          "  oColor.a = tex2D(uAlphaTex, iPosition_UV.yz)."+AlphaChannel+";" +
                                          "}\n";
		}

		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_DepthTexture_Shader_VP_Name+AlphaChannel, _def_DepthTexture_Shader_FP_Name+AlphaChannel};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		GpuProgram GpuPrograms[2] = {GPUP_VERTEX, GPUP_FRAGMENT};
		for (int k = 0; k < 2; k++)
		{
      if (!createGpuProgram(GpuProgramNames[k], GpuPrograms[k], EntryPoints[k], GpuProgramsData[k]))
      {
        return false;
      }
		}
		return true;
	}

	bool MaterialManager::_createUnderwaterMaterial(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		const bool cDepth    = _isComponent(Components, HYDRAX_COMPONENT_DEPTH   );
		// const bool cSmooth   = _isComponent(Components, HYDRAX_COMPONENT_SMOOTH  );      // cSmooth uneeded underwater
		const bool cSun      = _isComponent(Components, HYDRAX_COMPONENT_SUN     );
		const bool cFoam     = _isComponent(Components, HYDRAX_COMPONENT_FOAM    );
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		const bool cUReflections = _isComponent(Components, HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// Vertex program
		switch (normalMode)
		{
	    case NM_TEXTURE:
	    {
			  VertexProgramData += Ogre::String("void main_vp(\n") +
                                          // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          "float2 iUv               : TEXCOORD0,\n" +
                                          // OUT
                                          "out float4 oPosition     : POSITION,\n" +
                                          "out float4 oPosition_    : TEXCOORD0,\n" +
                                          "out float2 oUvNoise      : TEXCOORD1,\n" +
                                          "out float4 oUvProjection : TEXCOORD2,\n" +
                                          "out float2 oDistance     : TEXCOORD3,\n" +
                                          // UNIFORM
                                          "uniform float4x4         uWorldViewProj,\n" +
                                          "uniform float4x4         uWorldView,\n" +
                                          "uniform float4x4         uWorld,\n" +
                                          "uniform float            uFlip,\n" +
                                          "uniform float3           uCameraPos)\n" +
                                          "{\n" +
                                          "  oPosition_  = iPosition;\n" +
                                          "  oPosition = mul(uWorldViewProj, iPosition);\n" +
                                          // Projective texture coordinates, adjust for mapping
                                          "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
                                          "                               0, (0.5 * (uFlip * (-1))),   0, 0.5,"+
                                          "                               0,   0, 0.5, 0.5,"+
                                          "                               0,   0,   0,   1);\n" +
                                          "  oUvProjection = mul(scalemat, oPosition);\n" +
                                          "  oUvNoise = iUv;\n" +
                                          // Distance
                                          "  float4 mwPos = mul(uWorldView, iPosition);\n" +
                                          "  oDistance.x  = abs(mwPos.z);\n" +
                                          "  oDistance.y  = -mul( uWorld, float4(uCameraPos, 1.0) ).y;\n" +
                                          "}\n";
	    }
	    break;

			case NM_VERTEX:
		  {
			  VertexProgramData += Ogre::String("void main_vp(\n") +
                                          // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          "float3 iNormal           : NORMAL,\n"+
                                          // OUT
                                          "out float4 oPosition     : POSITION,\n" +
                                          "out float4 oPosition_    : TEXCOORD0,\n" +
                                          "out float4 oUvProjection : TEXCOORD1,\n" +
                                          "out float3 oNormal       : TEXCOORD2,\n" +
                                          "out float2 oDistance     : TEXCOORD3,\n" +
                                          // UNIFORM
                                          "uniform float4x4         uWorldViewProj,\n" +
                                          "uniform float4x4         uWorldView,\n" +
                                          "uniform float4x4         uWorld,\n" +
                                          "uniform float            uFlip,\n" +
                                          "uniform float3           uCameraPos)\n" +
               	                          "{\n" +
                  	                      "  oPosition_  = iPosition;\n" +
	              	                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
	               	                        // Projective texture coordinates, adjust for mapping
	                	                      "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
	                                        "                               0, (0.5 * (uFlip * (-1))),   0, 0.5,"+
              					                  "                               0,   0, 0.5, 0.5,"+
              					                  "                               0,   0,   0,   1);\n" +
	               	                        "  oUvProjection = mul(scalemat, oPosition);\n" +
							                            "  oNormal = normalize(iNormal);\n"+
                                          // Distance
                                          "  float4 mwPos = mul(uWorldView, iPosition);\n" +
                                          "  oDistance.x  = abs(mwPos.z);\n" +
                                          "  oDistance.y  = -mul( uWorld, float4(uCameraPos, 1.0) ).y;\n" +
               	                          "}\n";
	    }
	    break;

			case NM_RTT:
		  {
				VertexProgramData += Ogre::String("void main_vp(\n") +
                                          // IN
                                          "float4 iPosition         : POSITION,\n" +
                                          // OUT
                                          "out float4 oPosition     : POSITION,\n" +
                                          "out float4 oPosition_    : TEXCOORD0,\n" +
                                          "out float4 oUvProjection : TEXCOORD1,\n" +
                                          "out float2 oDistance     : TEXCOORD2,\n" +
                                          // UNIFORM
                                          "uniform float4x4         uWorldViewProj,\n" +
                                          "uniform float4x4         uWorldView,\n" +
                                          "uniform float4x4         uWorld,\n" +
                                          "uniform float            uFlip,\n" +
                                          "uniform float3           uCameraPos)\n" +
               	                          "{\n" +
                  	                      "  oPosition_ = iPosition;\n" +
	              	                        "  oPosition = mul(uWorldViewProj, iPosition);\n" +
	               	                        // Projective texture coordinates, adjust for mapping
	                	                      "  float4x4 scalemat = float4x4(0.5, 0,   0, 0.5,"+
	                                        "                               0, (0.5 * (uFlip * (-1))),   0, 0.5,"+
	                			                  "                               0,   0, 0.5, 0.5,"+
                                          "                               0,   0,   0,   1);\n" +
	               	                        "  oUvProjection = mul(scalemat, oPosition);\n" +
                                          // Distance
                                          "  float4 mwPos = mul(uWorldView, iPosition);\n" +
                                          "  oDistance.x  = abs(mwPos.z);\n" +
                                          "  oDistance.y  = -mul( uWorld, float4(uCameraPos, 1.0) ).y;\n" +
               	                          "}\n";
			}
		  break;
		}

		// Fragment program
    FragmentProgramData += Ogre::String("float3 expand(float3 v)\n") +
						                            "{\n" +
	                                      "  return (v - 0.5) * 2;\n" +
							                          "}\n\n" +
                                        "void main_fp(" +
							                          // IN
                                        "float4 iPosition     : TEXCOORD0,\n";
                           
    int TEXCOORDNUM = 1;
    if (normalMode == NM_TEXTURE)
    {
      FragmentProgramData += Ogre::String("float2 iUvNoise      : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
      TEXCOORDNUM++;
    }
    
    FragmentProgramData += Ogre::String("float4 iUvProjection : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
    TEXCOORDNUM++;
            
    if (normalMode == NM_VERTEX)
    {
      FragmentProgramData += Ogre::String("float3 iNormal       : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
      TEXCOORDNUM++;
    }
            
    FragmentProgramData += Ogre::String("float2 iDistance     : TEXCOORD" + Ogre::StringConverter::toString(TEXCOORDNUM) + ",\n");
    TEXCOORDNUM++;
            
    FragmentProgramData += Ogre::String(// OUT
                                        "out float4 oColor    : COLOR,\n") +
                                        // UNIFORM
                                        "uniform float3       uEyePosition,\n" +
                                        "uniform float        uFullReflectionDistance,\n" +
                                        "uniform float        uGlobalTransparency,\n" +
                                        "uniform float        uNormalDistortion,\n" +
                                        "uniform float        uDistLimit,\n" +
                                        "uniform float3       uWaterColor,\n";
                           
    if (cSun)
      FragmentProgramData += Ogre::String("uniform float3       uSunPosition,\n") +
                                          "uniform float        uSunStrength,\n" +
                                          "uniform float        uSunArea,\n" +
                                          "uniform float3       uSunColor,\n" +
                                          "uniform float        uDepthLimit,\n";

    if (cCaustics && cUReflections)
      FragmentProgramData += Ogre::String("uniform float        uCausticsPower,\n");
                    
    int TexNum = 0;
    if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
    {
      FragmentProgramData += Ogre::String("uniform sampler2D    uNormalMap       : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
      TexNum++;
    }

    if (cUReflections)            
    {
      FragmentProgramData += Ogre::String("uniform sampler2D    uReflectionMap   : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
      TexNum++;
    }
            
    FragmentProgramData += Ogre::String("uniform sampler2D    uRefractionMap   : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
    TexNum++;

    if (cDepth && cUReflections)
    {
      FragmentProgramData += Ogre::String("uniform sampler2D    uDepthReflectionMap : register(s" + Ogre::StringConverter::toString(TexNum) + "),\n");
      TexNum++;
    }

    FragmentProgramData += Ogre::String("uniform sampler2D    uFresnelMap      : register(s" + Ogre::StringConverter::toString(TexNum) + ")");
    TexNum++;

    FragmentProgramData += Ogre::String(")\n") +
							                          "{\n" +
							                          "  float2 ProjectionCoord = iUvProjection.xy / iUvProjection.w;\n" +                                        
                                        "  float3 camToSurface = iPosition.xyz - uEyePosition;\n" +
                                        "  float additionalReflection=camToSurface.x*camToSurface.x+camToSurface.z*camToSurface.z;\n" +
							                          "  additionalReflection/=uFullReflectionDistance;\n" +
								                        "  camToSurface=normalize(-camToSurface);\n";
            
    if (normalMode == NM_TEXTURE)
    {
      FragmentProgramData += Ogre::String("  float3 pixelNormal = tex2D(uNormalMap,iUvNoise).xyz;\n") +
                                          // Inverte y with z, because at creation our local normal to the plane was z
                                          "  pixelNormal.yz=pixelNormal.zy;\n" +
                                          // Remap from [0,1] to [-1,1]
                                          "  pixelNormal.xyz=-expand(pixelNormal.xyz);\n";
    }
    else if (normalMode == NM_VERTEX)
    {
      FragmentProgramData += Ogre::String("  float3 pixelNormal = -iNormal;\n");
    }
    else // NM_RTT
    {
      FragmentProgramData += Ogre::String("  float3 pixelNormal = -(2.0*tex2D(uNormalMap, ProjectionCoord.xy).xyz - 1.0);\n");
    }

    FragmentProgramData += Ogre::String("  float2 pixelNormalModified = uNormalDistortion*pixelNormal.zx;\n");
    if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
    {
      FragmentProgramData += Ogre::String("  float dotProduct=dot(camToSurface,pixelNormal);\n");
    }
    else
    {
      FragmentProgramData += Ogre::String("  float dotProduct=dot(-camToSurface,pixelNormal);\n");
    }

    FragmentProgramData += Ogre::String("  dotProduct=saturate(dotProduct);\n") +
                           Ogre::String("  float fresnel = tex2D(uFresnelMap,float2(dotProduct,0));\n") +
                                        // Add additional reflection and saturate
                                        "  fresnel+=additionalReflection;\n" +
				                                "  fresnel=saturate(fresnel);\n" +
				                                // Decrease the transparency and saturate
				                                "  fresnel-=uGlobalTransparency;\n" +
                                        "  fresnel=saturate(fresnel);\n" +
                                        "  float3 reflection;\n" +
                                        // Get the reflection/refraction pixels. Make sure to disturb the texcoords by pixelnormal
				                                "  float3 refraction=tex2D(uRefractionMap,ProjectionCoord.xy-pixelNormalModified).xyz;\n";

    if (cUReflections)
    {
      FragmentProgramData += Ogre::String("  reflection=tex2D(uReflectionMap,ProjectionCoord.xy+pixelNormalModified).xyz;\n");
    }
    else
    {
      FragmentProgramData += Ogre::String("  reflection=uWaterColor;\n");
    }

    if (cDepth && cUReflections)
    {
      if (cCaustics)
      {
        FragmentProgramData += Ogre::String("  float2 depth = tex2D(uDepthReflectionMap,ProjectionCoord.xy+pixelNormalModified).rg;\n") +
                                            "  reflection *= 1.0 + (depth.y * uCausticsPower * depth.x);\n" +
                                            "  reflection = lerp(uWaterColor, reflection, depth.x);\n";
      }
      else
      {
        FragmentProgramData += Ogre::String("  float depth = tex2D(uDepthReflectionMap,ProjectionCoord.xy+pixelNormalModified).r;\n") +
                                            "  reflection = lerp(uWaterColor,reflection,depth);\n";                      
      }
    }
               
    FragmentProgramData += Ogre::String("  float4 Color = float4(lerp(refraction,reflection,fresnel),1);\n" ) +
								                        "  float Distance = saturate(1.0 - iDistance.x*uDistLimit);\n" +
								                        "  Color.xyz = lerp(uWaterColor, Color.xyz, Distance);\n" +
								                        "  Color.xyz = lerp(Color.xyz, uWaterColor, uGlobalTransparency);\n";

    if (cSun)
      FragmentProgramData += Ogre::String("  float3 refractedVector = normalize(reflect(camToSurface, pixelNormal.xyz));\n") +
                                          "  float3 surfaceToSun=normalize(uSunPosition-iPosition.xyz);\n" +
                                          // Temporally solution, fix this
                                          "  surfaceToSun.xz = -surfaceToSun.xz;" +
                                          "  float3 sunlight = uSunStrength*pow(saturate(dot(refractedVector,surfaceToSun)),uSunArea)*uSunColor;\n" +
                                          "  Distance  = saturate(1.0 - iDistance.y*uDepthLimit);\n" +
                                          "  Color.xyz+=Distance*sunlight*saturate(1.0-additionalReflection);\n"+
                                          "  oColor = Color;\n" +
                                          "}\n";

		// Second: build our material
		Ogre::MaterialPtr &UnderwaterMaterial = getMaterial(MAT_UNDERWATER);
		UnderwaterMaterial = Ogre::MaterialManager::getSingleton().create(_def_Underwater_Material_Name, HYDRAX_RESOURCE_GROUP);

		Ogre::Pass *UM_Technique0_Pass0 = UnderwaterMaterial->getTechnique(0)->getPass(0);
		UM_Technique0_Pass0->setDepthWriteEnabled(true);
    UM_Technique0_Pass0->setCullingMode(Ogre::CULL_NONE);

		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_Underwater_Shader_VP_Name, _def_Underwater_Shader_FP_Name};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		fillGpuProgramsToPass(UM_Technique0_Pass0, GpuProgramNames, EntryPoints, GpuProgramsData);
		Ogre::GpuProgramParametersSharedPtr VP_Parameters = UM_Technique0_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = UM_Technique0_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    VP_Parameters->setNamedAutoConstant("uWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
    VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
    VP_Parameters->setNamedAutoConstant("uWorld",     Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
		FP_Parameters->setNamedAutoConstant("uEyePosition",   Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);

		FP_Parameters->setNamedConstant("uFullReflectionDistance", mHydrax->getFullReflectionDistance());
		FP_Parameters->setNamedConstant("uGlobalTransparency",     mHydrax->getGlobalTransparency());
		FP_Parameters->setNamedConstant("uNormalDistortion",       mHydrax->getNormalDistortion());
    if (VP_Parameters->_findNamedConstantDefinition("uFlip"))
      VP_Parameters->setNamedAutoConstant("uFlip",             Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING);
		FP_Parameters->setNamedConstant("uDistLimit",              1.f/mHydrax->getDistLimit());
    FP_Parameters->setNamedConstant("uWaterColor",             mHydrax->getWaterColor());

		if (cSun)
		{
			FP_Parameters->setNamedConstant("uSunPosition", mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getSunPosition()));
			FP_Parameters->setNamedConstant("uSunStrength", mHydrax->getSunStrength());
			FP_Parameters->setNamedConstant("uSunArea",     mHydrax->getSunArea());
			FP_Parameters->setNamedConstant("uSunColor",    mHydrax->getSunColor());
      FP_Parameters->setNamedConstant("uDepthLimit",  1.f/mHydrax->getDepthLimit());
		}

		if (cCaustics && cDepth && cUReflections)
			FP_Parameters->setNamedConstant("uCausticsPower", mHydrax->getCausticsPower());

		if (normalMode == NM_TEXTURE || normalMode == NM_RTT)
      UM_Technique0_Pass0->createTextureUnitState("HydraxNormalMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		if (cUReflections)
		  UM_Technique0_Pass0->createTextureUnitState("HydraxReflectionMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    UM_Technique0_Pass0->createTextureUnitState("HydraxRefractionMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		if (cDepth && cUReflections)
		  UM_Technique0_Pass0->createTextureUnitState()->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    UM_Technique0_Pass0->createTextureUnitState("Fresnel.bmp")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		UnderwaterMaterial->setReceiveShadows(false);
		UnderwaterMaterial->load();
		return true;
	}

	bool MaterialManager::_createUnderwaterCompositor(const HydraxComponent &Components, const NormalMode& normalMode)
	{
		const bool cCaustics = _isComponent(Components, HYDRAX_COMPONENT_CAUSTICS);
		const bool cDepth    = _isComponent(Components, HYDRAX_COMPONENT_DEPTH);
		const bool cGodRays  = _isComponent(Components, HYDRAX_COMPONENT_UNDERWATER_GODRAYS);
		Ogre::String VertexProgramData, FragmentProgramData;

		// Vertex program
    VertexProgramData += Ogre::String("void main_vp(\n") + 
                                      // IN 
                                      "float4 iPosition      : POSITION,\n" +
						                          // OUT
					                            "out float4 oPosition  : POSITION,\n" +
					                            "out float3 oPosition_ : TEXCOORD0,\n" +
					                            "out float2 oUV        : TEXCOORD1,\n";
    // UNIFORM
    if (cGodRays)
    {
      VertexProgramData += Ogre::String("uniform float3   uCorner0,\n") +
                                        "uniform float3   uCorner01,\n" +
                                        "uniform float3   uCorner02,\n";
    }
				
    VertexProgramData += Ogre::String("uniform float4x4 uWorldViewProj)\n") +
					                            "{\n" +
					                            "  oPosition = mul(uWorldViewProj, iPosition);\n"+
						                          "  iPosition.xy = sign(iPosition.xy);\n"+
						                          "  oUV = (float2(iPosition.x, -iPosition.y) + 1.0f) * 0.5f;";
    if (cGodRays)
		{
		  VertexProgramData += Ogre::String("  uCorner01 *= oUV.x;\n")+
                                        "  uCorner02 *= oUV.y;\n"+
                                        "  oPosition_ = uCorner0+uCorner01+uCorner02;";
		}
		VertexProgramData += "}\n";

		// Fragment program
		FragmentProgramData += Ogre::String("void main_fp(\n") +
                                        // IN
                                        "float3  iPosition    : TEXCOORD0,\n" +
                                        "float2  iUV          : TEXCOORD1,\n" +
                                        // OUT
                                        "out float4 oColor    : COLOR,\n";
                                        // UNIFORM
                      
    if (cCaustics)
      FragmentProgramData += Ogre::String("uniform float        uCausticsPower,\n");

    if (cGodRays)
      FragmentProgramData += Ogre::String("uniform float3  uSunColor,\n") +
                                          "uniform float3  uLightDirection,\n"+
                                          "uniform float   uIntensity,\n"+
                                          "uniform float3  uHGg,\n"+
                                          "uniform float3  uCameraPos,\n";

    if (cDepth)
      FragmentProgramData += Ogre::String("uniform float3        uWaterColor,\n");

    FragmentProgramData += Ogre::String("uniform float        uTime,\n") +
                                        "uniform sampler2D    uOriginalMap   : register(s0),\n"  +
                                        "uniform sampler2D    uDistortMap    : register(s1)\n";

    if (cDepth)
      FragmentProgramData += Ogre::String(",\nuniform sampler2D    uDepthMap      : register(s2)");

    FragmentProgramData += Ogre::String(")\n" ) +
                                        "{\n" +
                                        "  float2 distortUV = (tex2D(uDistortMap, float2(iUV.x + uTime, iUV.y + uTime)).xy - 0.5)/200.0;\n";

    if (cDepth)
    {
      FragmentProgramData += Ogre::String("  distortUV *= 1.0 - tex2D(uDepthMap, iUV).xy;\n");
    }
    
    FragmentProgramData += Ogre::String("  iUV += distortUV;\n");

    if (cDepth)
    {
       FragmentProgramData += Ogre::String("  float2 depth = tex2D(uDepthMap, iUV).xy;\n");
    }

    if (cCaustics && cDepth) // Depth, caustics
    {
      FragmentProgramData += Ogre::String("  float4 Color = float4(lerp(uWaterColor, tex2D(uOriginalMap, iUV) * (1.0 + (depth.y * uCausticsPower * depth.x)), depth.x), 1.0);\n");

      if (cGodRays)
      {
        FragmentProgramData += Ogre::String("  float3 view_vector = normalize(iPosition-uCameraPos);\n") +
                                            "  float dot_product = dot(view_vector, -uLightDirection);\n"+
                                            "  float num = uHGg.x;\n"+
                                            "  float den = (uHGg.y - uHGg.z*dot_product);\n"+
                                            "  den = rsqrt(den);\n"+
                                            "  float phase = num * (den*den*den);\n" +
                                            "  Color.xyz += pow(1.0 - (tex2D(uDepthMap, iUV).z)*uSunColor, phase) * uIntensity;\n";                
      }
    }
    else if (cDepth) // Depth, no caustics
    {
      FragmentProgramData += Ogre::String("  float4 Color = float4(lerp(uWaterColor,tex2D(uOriginalMap, iUV).xyz,tex2D(uDepthMap, iUV).r),1.0);\n");
      if (cGodRays)
      {
        FragmentProgramData += Ogre::String("  float3 view_vector = normalize(iPosition-uCameraPos);\n") +
                                            "  float dot_product = dot(view_vector, -uLightDirection);\n"+
                                            "  float num = uHGg.x;\n"+
                                            "  float den = (uHGg.y - uHGg.z*dot_product);\n"+
                                            "  den = rsqrt(den);\n"+
                                            "  float phase = num * (den*den*den);\n" +
                                            "  Color.xyz += pow(1.0 -  (tex2D(uDepthMap, iUV).z)*uSunColor, phase) * uIntensity;\n";       
      }
    }
    else // No depth, no caustics
    {
      FragmentProgramData += Ogre::String("  float4 Color = tex2D(uOriginalMap, iUV);");
    }

    // debug depth
    //FragmentProgramData += Ogre::String("  oColor = Color * 0.01 + tex2D(uDepthMap, iUV);\n") +
    FragmentProgramData += Ogre::String("  oColor = Color;\n") +
                                        "}\n";

    
    //Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LML_CRITICAL, FragmentProgramData);

		// Build our material
		Ogre::MaterialPtr &UnderwaterCompositorMaterial = getMaterial(MAT_UNDERWATER_COMPOSITOR);
		UnderwaterCompositorMaterial = Ogre::MaterialManager::getSingleton().create(_def_Underwater_Compositor_Material_Name, HYDRAX_RESOURCE_GROUP);

		Ogre::Pass *DM_Technique0_Pass0 = UnderwaterCompositorMaterial->getTechnique(0)->getPass(0);
		DM_Technique0_Pass0->setCullingMode(Ogre::CULL_NONE);
		DM_Technique0_Pass0->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);

		Ogre::String GpuProgramsData[2] = {VertexProgramData, FragmentProgramData};
		Ogre::String GpuProgramNames[2] = {_def_Underwater_Compositor_Shader_VP_Name, _def_Underwater_Compositor_Shader_FP_Name};
    Ogre::String EntryPoints[2];
    EntryPoints[0] = Ogre::String("main_vp");
    EntryPoints[1] = Ogre::String("main_fp");

		fillGpuProgramsToPass(DM_Technique0_Pass0, GpuProgramNames, EntryPoints, GpuProgramsData);
		Ogre::GpuProgramParametersSharedPtr VP_Parameters = DM_Technique0_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = DM_Technique0_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		FP_Parameters->setNamedConstantFromTime("uTime", 0.1f);

    if (cDepth)
      FP_Parameters->setNamedConstant("uWaterColor", mHydrax->getWaterColor());

		if (cDepth && cCaustics)
			FP_Parameters->setNamedConstant("uCausticsPower", mHydrax->getCausticsPower());

		if (cGodRays)
		{
			FP_Parameters->setNamedConstant("uSunColor", mHydrax->getSunColor());
			FP_Parameters->setNamedConstant("uLightDirection", (mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getCamera()->getDerivedPosition()) - mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getSunPosition())).normalisedCopy());
      FP_Parameters->setNamedConstant("uIntensity", mHydrax->getGodRaysIntensity());
		  FP_Parameters->setNamedConstant("uHGg", mHydrax->getGodRaysExposure());
		  FP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
		}

		// From compositor, original scene
		DM_Technique0_Pass0->createTextureUnitState()->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    DM_Technique0_Pass0->createTextureUnitState("UnderwaterDistortion.jpg")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

    if (cDepth)
		{
      DM_Technique0_Pass0->createTextureUnitState("HydraxDepthMap")->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		}

		UnderwaterCompositorMaterial->setReceiveShadows(false);
		UnderwaterCompositorMaterial->load();

		Ogre::CompositorPtr &UnderwaterCompositor = getCompositor(COMP_UNDERWATER);
    //assert(!UnderwaterCompositor.isNull());
		UnderwaterCompositor = Ogre::CompositorManager::getSingleton().create(_def_Underwater_Compositor_Name, HYDRAX_RESOURCE_GROUP);
		Ogre::CompositionTechnique* UnderWaterComp_Technique = UnderwaterCompositor->createTechnique();

		Ogre::PixelFormatList l;
		l.push_back(Ogre::PF_A8R8G8B8);

		Ogre::CompositionTechnique::TextureDefinition* PDef = UnderWaterComp_Technique->createTextureDefinition("PreviousScene");
    PDef->width = 0;
    PDef->height = 0;
		PDef->formatList = l;

    Ogre::CompositionTargetPass* CPPass = UnderWaterComp_Technique->createTargetPass();
    CPPass->setShadowsEnabled(false);
		CPPass->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
    CPPass->setOutputName("PreviousScene");

		// Build the output target pass
    Ogre::CompositionTargetPass* CTOutputPass = UnderWaterComp_Technique->getOutputTargetPass();
    CTOutputPass->setShadowsEnabled(false);
    CTOutputPass->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

		// Final composition pass
    Ogre::CompositionPass* COutputPass = CTOutputPass->createPass();
    COutputPass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
		COutputPass->setMaterial(getMaterial(MAT_UNDERWATER_COMPOSITOR));
    COutputPass->setInput(0, "PreviousScene");
		COutputPass->setLastRenderQueue(0);

    Hydrax::ViewportList lviewports = mHydrax->getViewports();
    Hydrax::ViewportList::iterator iViewportSearched = lviewports.begin();
    while (iViewportSearched != lviewports.end())
    {
      Ogre::Viewport* viewport = (*iViewportSearched);

      // Add compositor
      Ogre::CompositorInstance* compositorInstance = Ogre::CompositorManager::getSingleton().addCompositor(viewport, _def_Underwater_Compositor_Name);
      compositorInstance->addListener(&mUnderwaterCompositorListener);
      iViewportSearched++;
    }

		return true;
	}

  void MaterialManager::addCompositor(Ogre::Viewport* viewport)
  {
    Ogre::CompositorPtr &Comp = mCompositors[0];
		if (Comp.isNull())
			return;

    Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(viewport);
    if (!chain->getCompositor(Comp->getName()))
    {
      Ogre::CompositorInstance* compositorInstance = Ogre::CompositorManager::getSingleton().addCompositor(viewport, Comp->getName());
      compositorInstance->addListener(&mUnderwaterCompositorListener);
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, Comp->getName(), mCompositorsEnable[0]); 
    }
  }

  void MaterialManager::removeCompositor(Ogre::Viewport* viewport)
  {
    Ogre::CompositorPtr &Comp = mCompositors[0];
		if (Comp.isNull())
			return;

    Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(viewport);
    if (chain)
    {
      Ogre::CompositorInstance* compositorInstance = chain->getCompositor(_def_Underwater_Compositor_Name);
      if (compositorInstance)
      {
        compositorInstance->removeListener(&mUnderwaterCompositorListener);

        if (compositorInstance->getEnabled())
        {
          compositorInstance->setAlive(false);
        }
        chain->_removeInstance(compositorInstance);
        chain->_markDirty();
      }
    }
  }

	bool MaterialManager::_createSimpleColorMaterial(const Ogre::ColourValue& Color, const MaterialType& MT, const Ogre::String& Name, const bool& DepthCheck, const bool& DepthWrite)
	{
		Ogre::MaterialPtr &SimpleColorMaterial = getMaterial(MT);
		SimpleColorMaterial = Ogre::MaterialManager::getSingleton().create(Name, HYDRAX_RESOURCE_GROUP);

		Ogre::Pass *SCM_T0_Pass0 = SimpleColorMaterial->getTechnique(0)->getPass(0);
		SCM_T0_Pass0->setLightingEnabled(false);
		SCM_T0_Pass0->setDepthCheckEnabled(DepthCheck);
		SCM_T0_Pass0->setDepthWriteEnabled(DepthWrite);
		SCM_T0_Pass0->setCullingMode(Ogre::CULL_NONE);
		SCM_T0_Pass0->createTextureUnitState()->setColourOperationEx(Ogre::LBX_MODULATE,Ogre::LBS_MANUAL,Ogre::LBS_CURRENT, Color);

		SimpleColorMaterial->setReceiveShadows(false);
		SimpleColorMaterial->load();
		return true;
	}

	void MaterialManager::reload(const MaterialType &Material)
	{
		Ogre::MaterialPtr &Mat = getMaterial(Material);
		if (!Mat.isNull())
			return;

		Mat->reload();
		const bool cDepth    = _isComponent(mComponents, HYDRAX_COMPONENT_DEPTH   );
		const bool cSmooth   = _isComponent(mComponents, HYDRAX_COMPONENT_SMOOTH  );
		const bool cSun      = _isComponent(mComponents, HYDRAX_COMPONENT_SUN     );
		const bool cFoam     = _isComponent(mComponents, HYDRAX_COMPONENT_FOAM    );
		const bool cCaustics = _isComponent(mComponents, HYDRAX_COMPONENT_CAUSTICS);
		const bool cUReflections = _isComponent(mComponents, HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS);

		switch (Material)
		{
      case MAT_WATER:
			{
				Ogre::Pass *M_Technique0_Pass0 = Mat->getTechnique(0)->getPass(0);
				switch (mNormalMode)
				{
          case NM_TEXTURE:
          case NM_RTT:
					{
						M_Technique0_Pass0->getTextureUnitState(0)->setTextureName("HydraxNormalMap");
						M_Technique0_Pass0->getTextureUnitState(1)->setTextureName("HydraxReflectionMap");
						M_Technique0_Pass0->getTextureUnitState(2)->setTextureName("HydraxRefractionMap");
						if (cDepth)
              M_Technique0_Pass0->getTextureUnitState(3)->setTextureName("HydraxDepthMap");
					}
					break;

					case NM_VERTEX:
					{
						M_Technique0_Pass0->getTextureUnitState(0)->setTextureName("HydraxReflectionMap");
						M_Technique0_Pass0->getTextureUnitState(1)->setTextureName("HydraxRefractionMap");
						if (cDepth)
						    M_Technique0_Pass0->getTextureUnitState(2)->setTextureName("HydraxDepthMap");
					}
					break;
				}

				Ogre::GpuProgramParametersSharedPtr VP_Parameters = M_Technique0_Pass0->getVertexProgramParameters();
				Ogre::GpuProgramParametersSharedPtr FP_Parameters = M_Technique0_Pass0->getFragmentProgramParameters();

        VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        if (cFoam)
          VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);

				FP_Parameters->setNamedAutoConstant("uEyePosition",   Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
				FP_Parameters->setNamedConstant("uFullReflectionDistance", mHydrax->getFullReflectionDistance());
				FP_Parameters->setNamedConstant("uGlobalTransparency",     mHydrax->getGlobalTransparency());
				FP_Parameters->setNamedConstant("uNormalDistortion",       mHydrax->getNormalDistortion());
        if (VP_Parameters->_findNamedConstantDefinition("uFlip"))
          VP_Parameters->setNamedAutoConstant("uFlip",             Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING);

				if (cDepth)
					FP_Parameters->setNamedConstant("uWaterColor", mHydrax->getWaterColor());

				if (cSmooth)
					FP_Parameters->setNamedConstant("uSmoothPower", mHydrax->getSmoothPower());

        if (cSun)
				{
					FP_Parameters->setNamedConstant("uSunPosition", mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getSunPosition()));
					FP_Parameters->setNamedConstant("uSunStrength", mHydrax->getSunStrength());
					FP_Parameters->setNamedConstant("uSunArea",     mHydrax->getSunArea());
					FP_Parameters->setNamedConstant("uSunColor",    mHydrax->getSunColor());
				}

				if (cFoam)
				{
          FP_Parameters->setNamedConstant("uFoamRange",        mHydrax->getMesh()->getOptions().MeshStrength);
					FP_Parameters->setNamedConstant("uFoamMaxDistance",  mHydrax->getFoamMaxDistance());
					FP_Parameters->setNamedConstant("uFoamScale",        mHydrax->getFoamScale());
					FP_Parameters->setNamedConstant("uFoamStart",        mHydrax->getFoamStart());
					FP_Parameters->setNamedConstant("uFoamTransparency", mHydrax->getFoamTransparency());
				}

				if (cCaustics)
				  FP_Parameters->setNamedConstant("uCausticsPower",    mHydrax->getCausticsPower());
			}
			break;

			case MAT_DEPTH:
      case MAT_DEPTH_TRANS:
			{
				Ogre::Pass *M_Technique0_Pass0 = Mat->getTechnique(0)->getPass(0);
				Ogre::GpuProgramParametersSharedPtr VP_Parameters = M_Technique0_Pass0->getVertexProgramParameters();
				Ogre::GpuProgramParametersSharedPtr FP_Parameters = M_Technique0_Pass0->getFragmentProgramParameters();

        VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        // VP_Parameters->setNamedAutoConstant("uWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
        VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

        VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
				VP_Parameters->setNamedConstant("uPlaneYPos", mHydrax->getPosition().y);
        VP_Parameters->setNamedConstant("uPlanesError", (float) mHydrax->getPlanesError());

				FP_Parameters->setNamedConstant("uDepthLimit", 1.f/mHydrax->getDepthLimit());
				FP_Parameters->setNamedConstant("uDistLimit", 1.f/mHydrax->getDistLimit());

				if (cCaustics)
				{
					FP_Parameters->setNamedConstant("uCausticsScale", mHydrax->getCausticsScale());
					FP_Parameters->setNamedConstant("uCausticsEnd",   mHydrax->getCausticsEnd());
				}
			}
			break;

			case MAT_UNDERWATER:
			{
				Ogre::Pass *M_Technique0_Pass0 = Mat->getTechnique(0)->getPass(0);
				switch (mNormalMode)
				{
          case NM_TEXTURE:
          case NM_RTT:
					{
						M_Technique0_Pass0->getTextureUnitState(0)->setTextureName("HydraxNormalMap");
						int Index = 1;
						if (cUReflections)
						{
              M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxReflectionMap");
							Index++;
						}

						M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxRefractionMap");
						Index++;
						if (cDepth && cUReflections)
              M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxDepthReflectionMap");
					}
					break;

					case NM_VERTEX:
					{
						int Index = 0;
						if (cUReflections)
						{
              M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxReflectionMap");
							Index++;
						}

						M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxRefractionMap");
						Index++;
						if (cDepth && cUReflections)
              M_Technique0_Pass0->getTextureUnitState(Index)->setTextureName("HydraxDepthReflectionMap");
					}
					break;
				}

				Ogre::GpuProgramParametersSharedPtr VP_Parameters = M_Technique0_Pass0->getVertexProgramParameters();
				Ogre::GpuProgramParametersSharedPtr FP_Parameters = M_Technique0_Pass0->getFragmentProgramParameters();

        VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        VP_Parameters->setNamedAutoConstant("uWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
        VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);

        VP_Parameters->setNamedAutoConstant("uWorld",     Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
				FP_Parameters->setNamedAutoConstant("uEyePosition",   Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);

				FP_Parameters->setNamedConstant("uFullReflectionDistance", mHydrax->getFullReflectionDistance());
				FP_Parameters->setNamedConstant("uGlobalTransparency",     mHydrax->getGlobalTransparency());
				FP_Parameters->setNamedConstant("uNormalDistortion",       mHydrax->getNormalDistortion());
        VP_Parameters->setNamedAutoConstant("uFlip",               Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING);
        FP_Parameters->setNamedConstant("uDistLimit",              1.f/mHydrax->getDistLimit());

        FP_Parameters->setNamedConstant("uWaterColor", mHydrax->getWaterColor());

				if (cSun)
				{
					FP_Parameters->setNamedConstant("uSunPosition", mHydrax->getMesh()->getObjectSpacePosition(mHydrax->getSunPosition()));
					FP_Parameters->setNamedConstant("uSunStrength", mHydrax->getSunStrength());
					FP_Parameters->setNamedConstant("uSunArea",     mHydrax->getSunArea());
					FP_Parameters->setNamedConstant("uSunColor",    mHydrax->getSunColor());
          FP_Parameters->setNamedConstant("uDepthLimit",  1.f/mHydrax->getDistLimit());
				}

				if (cCaustics && cDepth && cUReflections)
					FP_Parameters->setNamedConstant("uCausticsPower",    mHydrax->getCausticsPower());
			}
			break;

			case MAT_UNDERWATER_COMPOSITOR:
			{
				mCompositorsNeedToBeReloaded[COMP_UNDERWATER] = true;
			}
			break;

			case MAT_SIMPLE_RED:
			{
			}
			break;

			case MAT_SIMPLE_BLACK:
			{
			}
			break;
		}
	}

	void MaterialManager::addDepthTechnique(Ogre::Technique *Technique, const bool& AutoUpdate)
	{
		if (!Ogre::MaterialManager::getSingleton().resourceExists(_def_Depth_Material_Name))
    {
			_createDepthMaterial(mComponents, mNormalMode);
      _createDepthMaterialTrans(mComponents, mNormalMode);
    }

		Technique->removeAllPasses();
		Technique->createPass();
		Technique->setName("_Hydrax_Depth_Technique");
		Technique->setSchemeName("HydraxDepth");

		Ogre::Pass *DM_Technique_Pass0 = Technique->getPass(0);
		DM_Technique_Pass0->setVertexProgram(_def_Depth_Shader_VP_Name);
		DM_Technique_Pass0->setFragmentProgram(_def_Depth_Shader_FP_Name);

		Ogre::GpuProgramParametersSharedPtr VP_Parameters = DM_Technique_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = DM_Technique_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

    VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
    VP_Parameters->setNamedConstant("uPlaneYPos", mHydrax->getPosition().y);
    VP_Parameters->setNamedConstant("uPlanesError", (float) mHydrax->getPlanesError());

		FP_Parameters->setNamedConstant("uDepthLimit", 1/mHydrax->getDepthLimit());
		FP_Parameters->setNamedConstant("uDistLimit", 1/mHydrax->getDistLimit());

		if (_isComponent(mComponents, HYDRAX_COMPONENT_CAUSTICS))
		{
			FP_Parameters->setNamedConstant("uCausticsScale", mHydrax->getCausticsScale());
			FP_Parameters->setNamedConstant("uCausticsEnd",   mHydrax->getCausticsEnd());

			Ogre::TextureUnitState *TUS_Caustics = DM_Technique_Pass0->createTextureUnitState("Caustics.bmp");
			TUS_Caustics->setName("Caustics");
			TUS_Caustics->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			TUS_Caustics->setAnimatedTextureName("Caustics.bmp", 32, 1.5);
		}

		if (AutoUpdate)
		  mDepthTechniques.push_back(Technique);
	}

	void MaterialManager::addDepthTextureTechnique(Ogre::Technique *Technique, const Ogre::String& TextureName, const Ogre::String& AlphaChannel, const bool& AutoUpdate)
	{
		if (!Ogre::HighLevelGpuProgramManager::getSingleton().resourceExists(_def_DepthTexture_Shader_VP_Name+AlphaChannel))
			_createDepthTextureGPUPrograms(mComponents, mNormalMode, AlphaChannel);

		Technique->removeAllPasses();
		Technique->createPass();
		Technique->setName("_Hydrax_DepthTexture_Technique");
		Technique->setSchemeName("HydraxDepth");

		Ogre::Pass *DM_Technique_Pass0 = Technique->getPass(0);

		// Alpha channel will be stored in pass 0 name:
		DM_Technique_Pass0->setName(AlphaChannel);

		DM_Technique_Pass0->setVertexProgram(_def_DepthTexture_Shader_VP_Name+AlphaChannel);
		DM_Technique_Pass0->setFragmentProgram(_def_DepthTexture_Shader_FP_Name+AlphaChannel);

		DM_Technique_Pass0->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		DM_Technique_Pass0->setDepthCheckEnabled(true);
		DM_Technique_Pass0->setDepthWriteEnabled(false);

		Ogre::GpuProgramParametersSharedPtr VP_Parameters = DM_Technique_Pass0->getVertexProgramParameters();
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = DM_Technique_Pass0->getFragmentProgramParameters();

    VP_Parameters->setNamedAutoConstant("uWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    VP_Parameters->setNamedAutoConstant("uCameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

    VP_Parameters->setNamedAutoConstant("uWorld", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
    VP_Parameters->setNamedConstant("uPlaneYPos", mHydrax->getPosition().y);
    VP_Parameters->setNamedConstant("uPlanesError", (float) mHydrax->getPlanesError());

		FP_Parameters->setNamedConstant("uDepthLimit", 1/mHydrax->getDepthLimit());
		FP_Parameters->setNamedConstant("uDistLimit", 1/mHydrax->getDistLimit());

		if (_isComponent(mComponents, HYDRAX_COMPONENT_CAUSTICS))
		{
			FP_Parameters->setNamedConstant("uCausticsScale", mHydrax->getCausticsScale());
			FP_Parameters->setNamedConstant("uCausticsEnd",   mHydrax->getCausticsEnd());

			Ogre::TextureUnitState *TUS_Caustics = DM_Technique_Pass0->createTextureUnitState("Caustics.bmp");
			TUS_Caustics->setName("Caustics");
			TUS_Caustics->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			TUS_Caustics->setAnimatedTextureName("Caustics.bmp", 32, 1.5);
		}

		Ogre::TextureUnitState *TUS_AlphaTex = DM_Technique_Pass0->createTextureUnitState(TextureName);
		TUS_AlphaTex->setName("_DetphTexture_Hydrax");
		TUS_AlphaTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		if (AutoUpdate)
      mDepthTechniques.push_back(Technique);
	}

	void MaterialManager::setCompositorEnable(const CompositorType &Compositor, const bool &Enable)
	{
		Ogre::CompositorPtr &Comp = mCompositors[static_cast<int>(Compositor)];
		std::cout << "HydraxMaterialManager::setCompositorEnable begin " << Comp->getName() << std::endl;
		if (Comp.isNull())
		{
			std::cout << "HydraxMaterialManager::setCompositorEnable returned " << Comp->getName() << std::endl;
			return;
		}

    Hydrax::ViewportList lviewports = mHydrax->getViewports();
    Hydrax::ViewportList::iterator iViewportSearched = lviewports.begin();
    while (iViewportSearched != lviewports.end())
    {
      Ogre::Viewport* viewport = (*iViewportSearched);
      Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(viewport);
      Ogre::CompositorInstance* compositorInstance = chain->getCompositor(Comp->getName());

    	if (compositorInstance->getEnabled() && !Enable)
      {
        compositorInstance->setEnabled(false);
		std::cout << "HydraxMaterialManager::setCompositorEnable  compositorInstance->setEnabled(false);" << std::endl;
      }
      else if (!compositorInstance->getEnabled() && Enable)
      {
        compositorInstance->setEnabled(true);
		std::cout << "HydraxMaterialManager::setCompositorEnable  compositorInstance->setEnabled(true);" << std::endl;
      }
      iViewportSearched++;
    }
		mCompositorsEnable[static_cast<int>(Compositor)] = Enable;
	}

	bool MaterialManager::_isComponent(const HydraxComponent &List, const HydraxComponent &ToCheck) const
	{
		if (List & ToCheck)
      return true;
      
    if (ToCheck == HYDRAX_COMPONENTS_NONE && List == HYDRAX_COMPONENTS_NONE)
      return true;

    if (ToCheck == HYDRAX_COMPONENTS_ALL && List == HYDRAX_COMPONENTS_ALL)
      return true;
      
    return false;
	}

	void MaterialManager::setGpuProgramParameter(const GpuProgram &GpuP, const MaterialType &MType, const Ogre::String &Name, const Ogre::Real &Value)
	{
		if (!mCreated)
			return;

		Ogre::GpuProgramParametersSharedPtr Parameters;
		switch (GpuP)
		{
      case GPUP_VERTEX:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getVertexProgramParameters();
			}
			break;

			case GPUP_FRAGMENT:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			}
			break;
		}

		Parameters->setNamedConstant(Name, Value);
		if (MType == MAT_DEPTH)
		{
			std::vector<Ogre::Technique*>::iterator TechIt;
			for(TechIt = mDepthTechniques.begin(); TechIt != mDepthTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
          case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
        }

				Parameters->setNamedConstant(Name, Value);
			}
		}
    else if (MType == MAT_DEPTH_TRANS)
    {
      std::vector<Ogre::Technique*>::iterator TechIt;
      for(TechIt = mDepthTransTechniques.begin(); TechIt != mDepthTransTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTransTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
          case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
        }

				Parameters->setNamedConstant(Name, Value);
			}
    }
	}

	void MaterialManager::setGpuProgramParameter(const GpuProgram &GpuP, const MaterialType &MType, const Ogre::String &Name, const Ogre::Vector2 &Value)
	{
		if (!mCreated)
			return;

		float Value_[2] = {Value.x, Value.y};

		Ogre::GpuProgramParametersSharedPtr Parameters;
		switch (GpuP)
		{
		  case GPUP_VERTEX:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getVertexProgramParameters();
			}
			break;

			case GPUP_FRAGMENT:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			}
			break;
		}

    Parameters->setNamedConstant(Name, Value_, 1, 2);
		if (MType == MAT_DEPTH)
		{
			std::vector<Ogre::Technique*>::iterator TechIt;
			for(TechIt = mDepthTechniques.begin(); TechIt != mDepthTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
	  		  case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
        }

				Parameters->setNamedConstant(Name, Value_, 1, 2);
			}
		}

    if (MType == MAT_DEPTH_TRANS)
    {
      std::vector<Ogre::Technique*>::iterator TechIt;
			for(TechIt = mDepthTransTechniques.begin(); TechIt != mDepthTransTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTransTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
	  		  case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
        }

				Parameters->setNamedConstant(Name, Value_, 1, 2);
			}
    }
	}

	void MaterialManager::setGpuProgramParameter(const GpuProgram &GpuP, const MaterialType &MType, const Ogre::String &Name, const Ogre::Vector3 &Value)
	{
		if (!mCreated)
			return;

		Ogre::GpuProgramParametersSharedPtr Parameters;
		switch (GpuP)
		{
      case GPUP_VERTEX:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getVertexProgramParameters();
			}
			break;

			case GPUP_FRAGMENT:
			{
				Parameters = getMaterial(MType)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			}
			break;
		}

		Parameters->setNamedConstant(Name, Value);
		if (MType == MAT_DEPTH)
		{
			std::vector<Ogre::Technique*>::iterator TechIt;
			for(TechIt = mDepthTechniques.begin(); TechIt != mDepthTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
          case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
		    }

				Parameters->setNamedConstant(Name, Value);
			}
		}
    else if (MType == MAT_DEPTH_TRANS)
    {
			std::vector<Ogre::Technique*>::iterator TechIt;
			for(TechIt = mDepthTransTechniques.begin(); TechIt != mDepthTransTechniques.end(); TechIt++)
			{
				if (!(*TechIt))
				{
					TechIt = mDepthTransTechniques.erase(TechIt);
					continue;
				}

				switch (GpuP)
				{
          case GPUP_VERTEX:
			    {
				    Parameters = (*TechIt)->getPass(0)->getVertexProgramParameters();
			    }
			    break;

			    case GPUP_FRAGMENT:
			    {
				    Parameters = (*TechIt)->getPass(0)->getFragmentProgramParameters();
			    }
			    break;
		    }

				Parameters->setNamedConstant(Name, Value);
			}
		}
	}

	void MaterialManager::UnderwaterCompositorListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if (mMaterialManager->_isComponent(mMaterialManager->mComponents, HYDRAX_COMPONENT_DEPTH))
    {
		  Ogre::Pass* DM_Technique0_Pass0 = mat->getTechnique(0)->getPass(0);
		  DM_Technique0_Pass0->getTextureUnitState(2)->setTextureName("HydraxDepthMap");
    }
	}

	void MaterialManager::UnderwaterCompositorListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		Ogre::GpuProgramParametersSharedPtr FP_Parameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
		if (mMaterialManager->mHydrax->isComponent(HYDRAX_COMPONENT_DEPTH))
      FP_Parameters->setNamedConstant("uWaterColor", mMaterialManager->mHydrax->getWaterColor());

		if (mMaterialManager->mHydrax->isComponent(HYDRAX_COMPONENT_CAUSTICS))
			FP_Parameters->setNamedConstant("uCausticsPower", mMaterialManager->mHydrax->getCausticsPower());

		if (mMaterialManager->mHydrax->isComponent(HYDRAX_COMPONENT_UNDERWATER_GODRAYS))
		{
			FP_Parameters->setNamedConstant("uSunColor", mMaterialManager->mHydrax->getSunColor());
			FP_Parameters->setNamedConstant("uLightDirection",(mMaterialManager->mHydrax->getMesh()->getObjectSpacePosition(mMaterialManager->mHydrax->getCamera()->getDerivedPosition()) - mMaterialManager->mHydrax->getMesh()->getObjectSpacePosition(mMaterialManager->mHydrax->getSunPosition())).normalisedCopy());
			FP_Parameters->setNamedConstant("uIntensity", mMaterialManager->mHydrax->getGodRaysIntensity());
      FP_Parameters->setNamedConstant("uHGg", mMaterialManager->mHydrax->getGodRaysExposure());

			Ogre::GpuProgramParametersSharedPtr VP_Parameters = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	    // FAR_LEFT_TOP
	    VP_Parameters->setNamedConstant( "uCorner0", mMaterialManager->mHydrax->getCamera()->getWorldSpaceCorners()[5] );
	    // FAR_RIGHT_TOP - FAR_LEFT_TOP
	    VP_Parameters->setNamedConstant( "uCorner01",  mMaterialManager->mHydrax->getCamera()->getWorldSpaceCorners()[4] -  mMaterialManager->mHydrax->getCamera()->getWorldSpaceCorners()[5]);
	    // FAR_LEFT_BOTTOM - FAR_LEFT_TOP
	    VP_Parameters->setNamedConstant( "uCorner02",  mMaterialManager->mHydrax->getCamera()->getWorldSpaceCorners()[6] - mMaterialManager->mHydrax->getCamera()->getWorldSpaceCorners()[5]);
		}

		if (mMaterialManager->mCompositorsNeedToBeReloaded[COMP_UNDERWATER])
		{
			if (mMaterialManager->_isComponent(mMaterialManager->mComponents, HYDRAX_COMPONENT_DEPTH))
	    {
        Ogre::Pass* DM_Technique0_Pass0 = mat->getTechnique(0)->getPass(0);
			  DM_Technique0_Pass0->getTextureUnitState(2)->setTextureName("HydraxDepthMap");
	    }
			mMaterialManager->mCompositorsNeedToBeReloaded[COMP_UNDERWATER] = false;
		}
	}

  void MaterialManager::UnderwaterCompositorListener::notifyResourcesCreated(bool forResizeOnly)
  {
    //register the listener again
    if (forResizeOnly)
    {
      Hydrax::ViewportList lviewports = mMaterialManager->mHydrax->getViewports();
      Hydrax::ViewportList::iterator iViewportSearched = lviewports.begin();
      while (iViewportSearched != lviewports.end())
      {
        Ogre::Viewport* viewport = (*iViewportSearched);
        Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(viewport);
        Ogre::CompositorInstance* compositorInstance = chain->getCompositor(_def_Underwater_Compositor_Name);

        iViewportSearched++;
      }
    }
  }

  Ogre::Technique* MaterialManager::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
  {
    Ogre::String targetScheme = schemeName;
    Ogre::MaterialManager::getSingleton().setActiveScheme("Default");
    Ogre::Technique* ogreTechnique = originalMaterial->getBestTechnique();
    Ogre::MaterialManager::getSingleton().setActiveScheme(targetScheme);

    if(schemeName == (Ogre::String("HydraxDepth")))
    {
      const bool cCaustics = _isComponent(mComponents, HYDRAX_COMPONENT_CAUSTICS);

      // If the technique is transparent, and not used by skyx then ignore for rendering
      if ((ogreTechnique) && ((ogreTechnique->getNumPasses() > 0) && (originalMaterial->getName().find("SkyX_") == Ogre::String::npos) && !ogreTechnique->getPass(0)->getDepthWriteEnabled() && (ogreTechnique->getPass(0)->getAlphaRejectValue()) < 120.0f))
      {
        return ogreTechnique;
      }
      else
      {
        Ogre::MaterialPtr matDepthTrans = getMaterial(MaterialManager::MAT_DEPTH_TRANS);
        Ogre::MaterialPtr matDepth = getMaterial(MaterialManager::MAT_DEPTH);

        if (!matDepthTrans.isNull())
        {
          if (matDepthTrans->getTechnique(Ogre::String("HydraxDepthTrans_") + originalMaterial->getName()))
          {
            return matDepthTrans->getTechnique(Ogre::String("HydraxDepthTrans_") + originalMaterial->getName());
          }

          //look to alpha rejection pass > 120
          Ogre::TexturePtr alphaTex;
          Ogre::Pass* alphaPass = 0;

          if (ogreTechnique != 0)
          {
            if(ogreTechnique->getNumPasses() > 0)
            {
              Ogre::Pass* pass = ogreTechnique->getPass(0);
              if (pass && Ogre::Real(pass->getAlphaRejectValue()) > 120.0f)
              {
                unsigned short nbTex = pass->getNumTextureUnitStates();
                for (unsigned short t = 0; t < nbTex && !alphaTex.isNull(); t++)
                {
                  Ogre::TextureUnitState* ustate = pass->getTextureUnitState(t);
                  Ogre::TexturePtr texptr = static_cast<Ogre::TexturePtr>(Ogre::TextureManager::getSingleton().getByName(ustate->getTextureName()));
                  Ogre::Texture* tex = static_cast<Ogre::Texture*>(texptr.getPointer());
                  if (tex && tex->hasAlpha() && (tex->getTextureType() == Ogre::TEX_TYPE_2D))
                  {
                    alphaTex = texptr;
                    alphaPass = pass;
                  }
                }
              }
            }
          }

          if (!alphaTex.isNull())
          {
            Ogre::Technique* baseTech = matDepthTrans->getTechnique(0);
            if (baseTech)
            {
              Ogre::Pass* basePass = baseTech->getPass(0);
              Ogre::Technique* nTech = matDepthTrans->createTechnique();
              nTech->setName(Ogre::String("HydraxDepthTrans_") + originalMaterial->getName());
              nTech->setSchemeName(Ogre::String("HydraxDepthTrans_") + originalMaterial->getName());

              Ogre::Pass* nPass = nTech->createPass();
              nPass->setLightingEnabled(false);
              nPass->setVertexProgram(basePass->getVertexProgramName());
              nPass->setVertexProgramParameters(basePass->getVertexProgramParameters());

              nPass->setFragmentProgram(basePass->getFragmentProgramName());
              nPass->setFragmentProgramParameters(basePass->getFragmentProgramParameters());

              nPass->setColourWriteEnabled(alphaPass->getColourWriteEnabled());
              nPass->setCullingMode(alphaPass->getCullingMode());
              nPass->setManualCullingMode(alphaPass->getManualCullingMode());
              nPass->setAmbient(alphaPass->getAmbient());
              nPass->setAlphaRejectFunction(alphaPass->getAlphaRejectFunction());
              nPass->setAlphaRejectValue(alphaPass->getAlphaRejectValue());
              nPass->setDepthWriteEnabled(alphaPass->getDepthWriteEnabled());
              nPass->setDepthCheckEnabled(alphaPass->getDepthCheckEnabled());

              Ogre::TextureUnitState* nTunitState = nPass->createTextureUnitState();
              nTunitState->setTexture(alphaTex);

              if (cCaustics)
              {
                Ogre::TextureUnitState *TUS_Caustics = nPass->createTextureUnitState("Caustics.bmp");
			          TUS_Caustics->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			          TUS_Caustics->setAnimatedTextureName("Caustics.bmp", 32, 1.5);
              }

              mDepthTransTechniques.push_back(nTech);
              return nTech;
            }
          }
          else
          {
            if (!matDepth.isNull())
              return matDepth->getBestTechnique();
          }
        }
        else
        {
          if (!matDepth.isNull())
            return matDepth->getBestTechnique();
        }

        // default
        return ogreTechnique;
      }
    }
    return ogreTechnique;
  }

  }