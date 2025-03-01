#pragma once
#include "ResourceManager.h"
#include <d2d1.h>

namespace Ice2D
{
	class PathGeometry : public IBasicResource
	{
	public:
		PathGeometry();
		PathGeometry(ResourceManager* pManager);
		PathGeometry(const PathGeometry& other) = delete;
		PathGeometry& operator=(const PathGeometry& other) = delete;
		PathGeometry(PathGeometry&& other) noexcept;
		PathGeometry& operator=(PathGeometry&& other) noexcept;
		~PathGeometry();
		void Release() override;
		void Close();
		void BeginFigure(const D2D_POINT_2F& startPoint, D2D1_FIGURE_BEGIN figureBegin = D2D1_FIGURE_BEGIN_FILLED);
		void EndFigure(D2D1_FIGURE_END figureEnd = D2D1_FIGURE_END_CLOSED);
		void AddLine(const D2D_POINT_2F& point);
		void AddLines(const D2D_POINT_2F* points, unsigned int count);
		ID2D1PathGeometry* Get() const;
		ID2D1GeometrySink* GetSink() const;
	private:
		ID2D1PathGeometry* m_pGeometry;
		ID2D1GeometrySink* m_pSink;
	};

	class Mesh : private IBasicResource
	{
	public:
		Mesh();
		Mesh(ResourceManager* pManager);
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;
		~Mesh();
		void Release() override;
		void AddTriangles(D2D1_TRIANGLE* triangles, unsigned int count);
		void AddTriangle(const D2D_POINT_2F& pt1, const D2D_POINT_2F& pt2, const D2D_POINT_2F& pt3);
		void Close();
		ID2D1Mesh* Get() const;
	private:
		ID2D1Mesh* m_pMesh;
		ID2D1TessellationSink* m_pSink;
	};
}