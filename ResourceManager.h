#pragma once

#include "Graphics.h"
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <xaudio2.h>
#include <list>

namespace Ice2D
{
	class IBasicResource;
	class ResourceManager
	{
		friend class IBasicResource;
	public:
		ResourceManager();
		ResourceManager(ID2D1RenderTarget* pRenderTarget);
		ResourceManager(Graphics& gfx);
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager& operator=(const ResourceManager& other) = delete;
		~ResourceManager();
	public:
		void FreeAll();
		ID2D1Factory* GetD2DFactory() const;
		ID2D1RenderTarget* GetRenderTarget() const;
		IWICImagingFactory* GetWICFactory();
		IDWriteFactory* GetWriteFactory();
		IXAudio2* GetXAudio();
		IXAudio2MasteringVoice* GetMasterVoice();
		void SetRenderTarget(ID2D1RenderTarget* pRenderTarget);
	private:
		std::list<IBasicResource*> m_trackers;
		ID2D1RenderTarget* m_pRenderTarget;
		static ID2D1Factory* m_pD2DFactory;
		static unsigned int instances;
		static IWICImagingFactory* m_pWICFactory;
		static IDWriteFactory* m_pWriteFactory;
		static IXAudio2* m_pXAudio2;
		static IXAudio2MasteringVoice* m_pMasterVoice;
	};

	class IBasicResource
	{
		friend class ResourceManager;
	public:
		IBasicResource();
		IBasicResource(ResourceManager* pManager);
		IBasicResource(const IBasicResource& other);
		IBasicResource& operator=(IBasicResource& other) = delete;
		~IBasicResource();
		virtual void Release() = 0;
		bool IsFree() const;
		bool IsLoaded() const;
	private:
		bool m_isFree, m_isLoaded;
		std::list<IBasicResource*>::iterator m_trackerRef;
		void RegisterTracker();
		void UnregisterTracker();
	protected:
		void OnLoad();
		void OnUnload();
		ResourceManager* m_pManager;
	};
}