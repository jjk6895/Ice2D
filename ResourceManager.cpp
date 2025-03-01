#include "pch.h"

#include "ResourceManager.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
	unsigned int ResourceManager::instances = 0;
	ID2D1Factory* ResourceManager::m_pD2DFactory = nullptr;
    IWICImagingFactory* ResourceManager::m_pWICFactory = nullptr;
    IDWriteFactory* ResourceManager::m_pWriteFactory = nullptr;
    IXAudio2* ResourceManager::m_pXAudio2 = nullptr;
    IXAudio2MasteringVoice* ResourceManager::m_pMasterVoice = nullptr;

	ResourceManager::ResourceManager() : m_pRenderTarget(nullptr)
    {
        if (instances < 1)
        {
            HRESULT hr = CoInitialize(nullptr);
            CheckHR(hr);
        }
        ++instances;
    }

	ResourceManager::ResourceManager(ID2D1RenderTarget* pRenderTarget) : ResourceManager()
    {
		SetRenderTarget(pRenderTarget);
    }

    ResourceManager::ResourceManager(Graphics& gfx) : ResourceManager(gfx.GetRT())
    {
    }

    ResourceManager::~ResourceManager()
    {
        --instances;
        FreeAll();
		SafeRelease(m_pRenderTarget);
        if (instances < 1)
        {
		    SafeRelease(m_pD2DFactory);
            SafeRelease(m_pWICFactory);
            SafeRelease(m_pWriteFactory);
            if (m_pMasterVoice)
            {
                m_pMasterVoice->DestroyVoice();
                m_pMasterVoice = nullptr;
            }
            SafeRelease(m_pXAudio2);
            CoUninitialize();
        }
    }

    void ResourceManager::FreeAll()
    {
        for (IBasicResource* resource : m_trackers)
        {
            resource->Release();
            resource->m_isFree = true;
        }
        m_trackers.clear();
    }

    ID2D1RenderTarget* ResourceManager::GetRenderTarget() const
    {
		if (!m_pRenderTarget) throw std::runtime_error("Render target is null.");
        return m_pRenderTarget;
    }

    ID2D1Factory* ResourceManager::GetD2DFactory() const
    {
		if (!m_pD2DFactory) throw std::runtime_error("Direct2D factory is null.");
        return m_pD2DFactory;
    }

    IWICImagingFactory* ResourceManager::GetWICFactory()
    {
        if (!m_pWICFactory)
        {
            HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
            CheckHR(hr);
        }
        return m_pWICFactory;
    }

    IDWriteFactory* ResourceManager::GetWriteFactory()
    {
        if (!m_pWriteFactory)
        {
            HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&m_pWriteFactory));
            CheckHR(hr);
        }

        return m_pWriteFactory;
    }

    IXAudio2* ResourceManager::GetXAudio()
    {
		if (!m_pXAudio2)
		{
            HRESULT hr = XAudio2Create(&m_pXAudio2);
            CheckHR(hr);
		}
        return m_pXAudio2;
    }

    IXAudio2MasteringVoice* ResourceManager::GetMasterVoice()
    {
        if (!m_pMasterVoice)
        {
            HRESULT hr = GetXAudio()->CreateMasteringVoice(&m_pMasterVoice);
            CheckHR(hr);
        }
        return m_pMasterVoice;
    }

    void ResourceManager::SetRenderTarget(ID2D1RenderTarget* pRenderTarget)
    {
		SafeRelease(m_pRenderTarget);
		HRESULT hr = pRenderTarget->QueryInterface(&m_pRenderTarget);
        CheckHR(hr);

        ID2D1Factory* pFactory;
        pRenderTarget->GetFactory(&pFactory);
        if (!m_pD2DFactory)
        {
            m_pD2DFactory = pFactory;
        }
        else if (m_pD2DFactory != pFactory)
        {
            throw std::runtime_error("Duplicate Direct2D factory. Factory already exists.");
        }
        else
        {
			SafeRelease(pFactory);
        }
    }

    IBasicResource::IBasicResource() : m_isFree(true), m_isLoaded(false), m_pManager(nullptr)
    {
    }

    IBasicResource::IBasicResource(ResourceManager* pManager) :
        m_pManager(pManager), m_isFree(true), m_isLoaded(false)
    {
    }

	IBasicResource::IBasicResource(const IBasicResource& other) : IBasicResource(other.m_pManager)
    {
    }

    IBasicResource::~IBasicResource()
    {
		UnregisterTracker();
    }

    bool IBasicResource::IsFree() const
    {
        return m_isFree;
    }

    bool IBasicResource::IsLoaded() const
    {
        return m_isLoaded;
    }

    void IBasicResource::RegisterTracker()
    {
        if (!m_isFree) return;
        m_trackerRef = m_pManager->m_trackers.insert(m_pManager->m_trackers.end(), this);
        m_isFree = false;
    }

    void IBasicResource::UnregisterTracker()
    {
        if (m_isFree) return;
        m_pManager->m_trackers.erase(m_trackerRef);
        m_isFree = true;
    }

    void IBasicResource::OnLoad()
    {
        RegisterTracker();
        m_isLoaded = true;
    }

    void IBasicResource::OnUnload()
    {
        m_isLoaded = false;
    }
}
