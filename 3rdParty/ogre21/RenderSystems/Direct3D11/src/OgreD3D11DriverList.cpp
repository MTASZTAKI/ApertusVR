/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgreD3D11DriverList.h"
#include "OgreLogManager.h"
#include "OgreD3D11Device.h"
#include "OgreD3D11Driver.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    D3D11DriverList::D3D11DriverList( IDXGIFactoryN* pDXGIFactory ) 
    {
        mHiddenDriversCount = 0;
        enumerate(pDXGIFactory);
    }
    //-----------------------------------------------------------------------
    D3D11DriverList::~D3D11DriverList(void)
    {
        for(unsigned i = 0; i < mDriverList.size(); i++)
        {
            delete (mDriverList[i]);
        }
        mDriverList.clear();

    }
    //-----------------------------------------------------------------------
    BOOL D3D11DriverList::enumerate(IDXGIFactoryN*  pDXGIFactory)
    {
        assert(mDriverList.empty() && mHiddenDriversCount == 0);

        LogManager::getSingleton().logMessage( "D3D11: Driver Detection Starts" );

        for( UINT iAdapter=0; ; iAdapter++ )
        {
            IDXGIAdapterN* pDXGIAdapter = NULL;
            HRESULT hr = pDXGIFactory->EnumAdapters1( iAdapter, &pDXGIAdapter );
            if( DXGI_ERROR_NOT_FOUND == hr )
            {
                hr = S_OK;
                break;
            }
            if( FAILED(hr) )
            {
                SAFE_RELEASE(pDXGIAdapter);
                return false;
            }

            // we don't want NVIDIA PerfHUD in the list, so place it to the hidden part of drivers list
            D3D11Driver* driver = new D3D11Driver(iAdapter, pDXGIAdapter);

            const bool isHidden = wcscmp(driver->getAdapterIdentifier().Description, L"NVIDIA PerfHUD") == 0;
            if(isHidden)
            {
                mDriverList.push_back(driver);
                ++mHiddenDriversCount;
            }
            else
            {
                mDriverList.insert(mDriverList.end() - mHiddenDriversCount, driver);
            }

            SAFE_RELEASE(pDXGIAdapter);
        }

        LogManager::getSingleton().logMessage( "D3D11: Driver Detection Ends" );

        return TRUE;
    }
    //-----------------------------------------------------------------------
    size_t D3D11DriverList::count() const 
    {
        return mDriverList.size() - mHiddenDriversCount;
    }
    //-----------------------------------------------------------------------
    D3D11Driver* D3D11DriverList::item( size_t index )
    {
        return mDriverList.at( index );
    }
    //-----------------------------------------------------------------------
    D3D11Driver* D3D11DriverList::item( const String &name )
    {
        vector<D3D11Driver*>::type::iterator it = mDriverList.begin();
        if (it == mDriverList.end())
            return NULL;

        for (;it != mDriverList.end(); ++it)
        {
            if ((*it)->DriverDescription() == name)
                return (*it);
        }

        return NULL;
    }
    //-----------------------------------------------------------------------
}
