#include "pch.h"

#include "Geometry.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    PathGeometry::PathGeometry() : m_pGeometry(nullptr), m_pSink(nullptr)
    {
    }

    PathGeometry::PathGeometry(ResourceManager* pManager) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetD2DFactory()->CreatePathGeometry(&m_pGeometry);
        CheckHR(hr);
        hr = m_pGeometry->Open(&m_pSink);
        CheckHR(hr);

        OnLoad();
    }

    PathGeometry::PathGeometry(PathGeometry&& other) noexcept : 
        IBasicResource(other), m_pGeometry(other.m_pGeometry), m_pSink(other.m_pSink)
    {
        other.m_pGeometry = nullptr;
        other.m_pSink = nullptr;
        OnLoad();
    }

    PathGeometry& PathGeometry::operator=(PathGeometry&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pGeometry = other.m_pGeometry;
        other.m_pGeometry = nullptr;
        m_pSink = other.m_pSink;
		other.m_pSink = nullptr;

        OnLoad();
        return *this;
    }

    PathGeometry::~PathGeometry()
    {
        Release();
    }

    void PathGeometry::Release()
    {
        if (m_pSink)
        {
            HRESULT hr = m_pSink->Close();
            CheckHR(hr);
        }
        SafeRelease(m_pSink);
        SafeRelease(m_pGeometry);
        OnUnload();
    }

    void PathGeometry::Close()
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null. Cannot close.");
        HRESULT hr = m_pSink->Close();
        CheckHR(hr);
        SafeRelease(m_pSink);
    }

    void PathGeometry::BeginFigure(const D2D_POINT_2F& startPoint, D2D1_FIGURE_BEGIN figureBegin)
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null. Cannot begin figure.");
        m_pSink->BeginFigure(startPoint, figureBegin);
    }

    void PathGeometry::EndFigure(D2D1_FIGURE_END figureEnd)
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null. Cannot end figure.");
        m_pSink->EndFigure(figureEnd);
    }

    void PathGeometry::AddLine(const D2D_POINT_2F& point)
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null. Cannot add line.");
        m_pSink->AddLine(point);
    }

    void PathGeometry::AddLines(const D2D_POINT_2F* points, unsigned int count)
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null. Cannot add lines.");
        m_pSink->AddLines(points, count);
    }

    ID2D1PathGeometry* PathGeometry::Get() const
    {
        if (m_pSink) throw std::runtime_error("Geometry has not been closed, cannot retrieve.");
        if (!m_pGeometry) throw std::runtime_error("Path geometry is null.");
        return m_pGeometry;
    }

    ID2D1GeometrySink* PathGeometry::GetSink() const
    {
        if (!m_pSink) throw std::runtime_error("Geometry sink is null, cannot retrieve.");
        return m_pSink;
    }

    Mesh::Mesh() : m_pMesh(nullptr), m_pSink(nullptr)
    {
    }

    Mesh::Mesh(ResourceManager* pManager) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateMesh(&m_pMesh);
        CheckHR(hr);
        hr = m_pMesh->Open(&m_pSink);
        CheckHR(hr);

        OnLoad();
    }

    Mesh::Mesh(Mesh&& other) noexcept : IBasicResource(other), m_pMesh(other.m_pMesh), m_pSink(other.m_pSink)
    {
        other.m_pMesh = nullptr;
        other.m_pSink = nullptr;
        OnLoad();
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pMesh = other.Get();
        other.m_pMesh = nullptr;

        OnLoad();
        return *this;
    }

    Mesh::~Mesh()
    {
        Release();
    }

    void Mesh::Release()
    {
        if (m_pSink)
        {
            HRESULT hr = m_pSink->Close();
            CheckHR(hr);
        }
        SafeRelease(m_pSink);
        SafeRelease(m_pMesh);

        OnUnload();
    }

    void Mesh::AddTriangles(D2D1_TRIANGLE* triangles, unsigned int count)
    {
        if (!m_pSink) throw std::runtime_error("Tessellation sink is null. Cannot add triangles.");
        m_pSink->AddTriangles(triangles, count);
    }

    void Mesh::AddTriangle(const D2D_POINT_2F& pt1, const D2D_POINT_2F& pt2, const D2D_POINT_2F& pt3)
    {
        if (!m_pSink) throw std::runtime_error("Tessellation sink is null. Cannot add triangle.");
        D2D1_TRIANGLE tri = { pt1, pt2, pt3 };
        m_pSink->AddTriangles(&tri, 1u);
    }

    void Mesh::Close()
    {
        if (!m_pSink) throw std::runtime_error("Tessellation sink is null. Cannot close.");
        HRESULT hr = m_pSink->Close();
        CheckHR(hr);
        SafeRelease(m_pSink);
    }

    ID2D1Mesh* Mesh::Get() const
    {
        if (m_pSink) throw std::runtime_error("Mesh has not been closed, cannot retrieve.");
        if (!m_pMesh) throw std::runtime_error("Mesh is null.");
        return m_pMesh;
    }
}