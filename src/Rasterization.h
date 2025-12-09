#pragma once
#include "Image.h"
#include "Mesh.h"

struct Rect
{
	int xMin;
	int xMax;
	int yMin;
	int yMax;
};

inline void DrawLineX(Image* image, int row, int x0, int x1, Color color)
{
	// for x in x0 to x1:
	//		set pixel at x, row
	for (int x = x0; x <= x1; x++)
		SetPixel(image, x, row, color);
}

inline void DrawLineY(Image* image, int col, int y0, int y1, Color color)
{
	// for y in y0 to y1:
	//		set pixel at col, y
	for (int y = y0; y <= y1; y++)
		SetPixel(image, col, y, color);
}

inline void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color)
{
	x0 = Clamp(x0, 0.0f, image->width - 1);
	x1 = Clamp(x1, 0.0f, image->width - 1);
	y0 = Clamp(y0, 0.0f, image->height - 1);
	y1 = Clamp(y1, 0.0f, image->height - 1);

	// let dx = x1 - x0
	// let dy = y1 - y0
	int dx = x1 - x0;
	int dy = y1 - y0;

	// let steps = abs dx if abs dx > abs dy, else abs dy
	// abs = "absolute value"
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// let xStep = dx / steps as float
	// let yStep = dy / steps as float
	float xStep = dx / (float)steps;
	float yStep = dy / (float)steps;

	// x = x0
	// y = y0
	// set pixel at x, y
	// for i in steps:
	//		set pixel at x, y
	//		x += xStep
	//		y += yStep
	float x = x0;
	float y = y0;
	for (int i = 0; i <= steps; i++)
	{
		SetPixel(image, x, y, color);
		x += xStep;
		y += yStep;
	}
}

inline void DrawRect(Image* image, int x, int y, int w, int h, Color color)
{
	// Option 1:
	// for ry in y to y + h:
	//		for rx in x to x + h:
	//			set pixel at rx, ry
	for (int ry = y; ry <= y + h; ry++)
	{
		for (int rx = x; rx <= x + w; rx++)
		{
			SetPixel(image, rx, ry, color);
		}
	}

	// Option 2:
	// Render vertical lines for all columns
	//for (int col = x; col <= x + w; col++)
	//	DrawLineY(image, col, y, y + h, color);

	// Option 3:
	// Render horizontal lines for all rows
	//for (int row = y; row <= y + h; row++)
	//	DrawLineX(image, row, x, x + w, color);
}

inline void DrawRectLines(Image* image, int x, int y, int w, int h, Color color)
{
	// Render vertical lines at x and x + w
	// Render horizontal lines at y and y + h
	DrawLineX(image, y + 0, x, x + w, color);
	DrawLineX(image, y + h, x, x + w, color);
	DrawLineY(image, x + 0, y, y + h, color);
	DrawLineY(image, x + w, y, y + h, color);
}

inline void DrawCircle(Image* image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	auto line = [&](int lx, int ly, int l)
		{
			for (int dx = -l; dx <= l; dx++)
				SetPixel(image, lx + dx, ly, color);
		};

	while (y >= x)
	{
		line(cx, cy + y, x);
		line(cx, cy - y, x);
		line(cx, cy + x, y);
		line(cx, cy - x, y);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

inline void DrawEllipse(Image* img, int xc, int yc, int a, int b, Color color) {
	int x = 0;
	int y = b;

	// Region 1: where slope > -1
	int a2 = a * a;
	int b2 = b * b;
	int fa2 = 4 * a2;
	int fb2 = 4 * b2;

	// Initial decision parameter for region 1
	int d1 = b2 - (a2 * b) + (0.25f * a2);
	int dx = 2 * b2 * x;
	int dy = 2 * a2 * y;

	// Store previous points for line drawing
	int prevX = x, prevY = y;

	// Region 1: Connect points horizontally as we step x
	while (dx < dy) {
		// Draw horizontal line segments connecting symmetric points
		DrawLine(img, xc - x, yc + y, xc + x, yc + y, color);
		DrawLine(img, xc - x, yc - y, xc + x, yc - y, color);

		if (d1 < 0) {
			x++;
			dx += fb2;
			d1 += dx + b2;
		}
		else {
			x++;
			y--;
			dx += fb2;
			dy -= fa2;
			d1 += dx - dy + b2;
		}
	}

	// Region 2: where slope <= -1
	int d2 = b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2 * b2;

	// Region 2: Connect points horizontally as we step y
	while (y >= 0) {
		// Draw horizontal line segments connecting symmetric points
		DrawLine(img, xc - x, yc + y, xc + x, yc + y, color);
		DrawLine(img, xc - x, yc - y, xc + x, yc - y, color);

		if (d2 > 0) {
			y--;
			dy -= fa2;
			d2 += a2 - dy;
		}
		else {
			x++;
			y--;
			dx += fb2;
			dy -= fa2;
			d2 += dx - dy + a2;
		}
	}
}

inline void DrawCircleLines(Image* image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	while (y >= x)
	{
		SetPixel(image, cx + x, cy + y, color);
		SetPixel(image, cx - x, cy + y, color);
		SetPixel(image, cx + x, cy - y, color);
		SetPixel(image, cx - x, cy - y, color);
		SetPixel(image, cx + y, cy + x, color);
		SetPixel(image, cx - y, cy + x, color);
		SetPixel(image, cx + y, cy - x, color);
		SetPixel(image, cx - y, cy - x, color);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

inline void DrawFaceWireframes(Image* image, Vector3* positions, size_t face, Color color = WHITE)
{
	size_t vertex = face * 3;
	for (size_t i = 0; i < 3; i++)
	{
		Vector3 v0 = positions[vertex + i];
		Vector3 v1 = positions[vertex + ((i + 1) % 3)];

		v0.x = Remap(v0.x, -1.0f, 1.0f, 0, image->width  - 1);
		v0.y = Remap(v0.y, -1.0f, 1.0f, 0, image->height - 1);
		v1.x = Remap(v1.x, -1.0f, 1.0f, 0, image->width  - 1);
		v1.y = Remap(v1.y, -1.0f, 1.0f, 0, image->height - 1);

		// Toggle far-plane clipping (approximation):
		//if (v0.z > 1.0f || v1.z > 1.0) return;

		DrawLine(image, v0.x, v0.y, v1.x, v1.y, color);
	}
}

// Data that is constant between vertex & fragment processing
struct UniformData
{
	Matrix world;
	Matrix mvp;

	Image* tex;
	Vector3 lightColor;
	Vector3 lightDirection;
	// (Add any additional "constant data" you like here)
};

// Data that is unique for each vertex and interpolated across fragments
struct VertexAttributes
{
	Vector3 p;	// Position
	Vector3 n;	// Normal
	Vector2 t;	// Tcoord
	// (Add any additional "unique data" you like here)
};

// "A fragment shader is any function that inputs vertex attributes & uniform data, and outputs a vec3"
using FragmentShader = Vector3(*)(const VertexAttributes& atr, const UniformData& data);

// Render filled-polygon to image buffer
inline void DrawMesh(Image* image, Mesh mesh, const UniformData& data, FragmentShader fs)
{
	// Vertex attributes interpolated and passed to fragment shader
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> tcoords;
	positions.resize(mesh.count);
	normals.resize(mesh.count);
	tcoords.resize(mesh.count);

	// Internal vertex attributes used for rasterization & face culling
	std::vector<Vector3> ndcs;
	std::vector<Vector2> pixels;
	ndcs.resize(mesh.count);
	pixels.resize(mesh.count);

	Matrix normal_matrix = NormalMatrix(data.world);
	for (size_t i = 0; i < mesh.count; i++)
	{
		// Apply space-transformations and forward to fragment-shader
		int index = mesh.indices.empty() ? i : mesh.indices[i];
		Vector3 position = data.world * mesh.positions[index];
		Vector3 normal = normal_matrix * mesh.normals[index];
		Vector3 tcoord = mesh.tcoords[index];
		positions[i] = position;
		normals[i] = normal;
		tcoords[i] = tcoord;

		// Preserve normalized-device coordinates and screen-coordinates for face-culling, and depth-reconstruction
		Vector3 ndc = Clip(data.mvp, mesh.positions[index]);
		Vector2 screen = ndc;
		screen.x = Remap(screen.x, -1.0f, 1.0f, 0.0f, image->width - 1.0f);
		screen.y = Remap(screen.y, -1.0f, 1.0f, 0.0f, image->height - 1.0f);

		ndcs[i] = ndc;		// Resolution-agnostic vertex-positions xy[-1, 1] & vertex-depths z[0, 1]
		pixels[i] = screen;	// Screen-space vertex-positions [0, wh]
	}

	// Triangle AABBs
	std::vector<Rect> rects;
	rects.resize(mesh.count / 3);
	for (size_t face = 0; face < mesh.count / 3; face++)
	{
		// Ensure min & max get overwritten
		int xMin = image->width - 1;
		int yMin = image->height - 1;
		int xMax = 0;
		int yMax = 0;

		// Determine min & max of face, ensure its on-screen
		const size_t vertex = face * 3;
		for (size_t i = 0; i < 3; i++)
		{
			int x = pixels[vertex + i].x;
			int y = pixels[vertex + i].y;
			xMin = std::max(0, std::min(xMin, x));
			yMin = std::max(0, std::min(yMin, y));
			xMax = std::min(image->width - 1, std::max(xMax, x));
			yMax = std::min(image->height - 1, std::max(yMax, y));
		}

		// Determine rectangle bounds to optimize per-pixel point-in-triangle tests
		rects[face].xMin = xMin;
		rects[face].xMax = xMax;
		rects[face].yMin = yMin;
		rects[face].yMax = yMax;

		// Easiest to do face-culling in normalized-device-coordinates (NDC) since vertices are aligned with world axes
		Vector3 ndc0 = ndcs[vertex + 0];
		Vector3 ndc1 = ndcs[vertex + 1];
		Vector3 ndc2 = ndcs[vertex + 2];

		// Let N = unit vector perpendicular to triangle face
		Vector3 AB = Normalize(ndc1 - ndc0);
		Vector3 AC = Normalize(ndc2 - ndc0);
		Vector3 N = Normalize(Cross(AB, AC));
		bool frontFacing = Dot(N, V3_FORWARD) > 0.0f;

		// Modify our bounding-boxes such that they won't be evaluated per-pixel if the triangle is back-facing (invisible)
		if (!frontFacing)
		{
			rects[face].xMin = 1;
			rects[face].xMax = 0;
			rects[face].yMin = 1;
			rects[face].yMax = 0;
		}
	}

	// Shade all pixels of every triangle in the mesh
	for (size_t face = 0; face < mesh.count / 3; face++)
	{
		for (int x = rects[face].xMin; x <= rects[face].xMax; x++)
		{
			for (int y = rects[face].yMin; y <= rects[face].yMax; y++)
			{
				size_t vertex = face * 3;

				// Use barycentric coordinates to perform point-in-triangle test, test passed if uvw between [0, 1]
				Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f }, pixels[vertex + 0], pixels[vertex + 1], pixels[vertex + 2]);

				// Discard if pixel not in triangle
				bool low = bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f;
				bool high = bc.x > 1.0f || bc.y > 1.0f || bc.z > 1.0f;
				if (low || high)
					continue;

				// Determine pixel-depth via tri-linear interpolation
				float depth = ndcs[vertex + 0].z * bc.x + ndcs[vertex + 1].z * bc.y + ndcs[vertex + 2].z * bc.z;

				// Perform depth-test and update depth-buffer
				if (depth > GetDepth(*image, x, y))
					continue;
				SetDepth(image, x, y, depth);

				// Interpolate vertex positions --> "fragment positions"
				Vector3 p0 = positions[vertex + 0];
				Vector3 p1 = positions[vertex + 1];
				Vector3 p2 = positions[vertex + 2];
				Vector3 p = p0 * bc.x + p1 * bc.y + p2 * bc.z;

				// Interpolate vertex normals --> "fragment normals"
				Vector3 n0 = normals[vertex + 0];
				Vector3 n1 = normals[vertex + 1];
				Vector3 n2 = normals[vertex + 2];
				Vector3 n = n0 * bc.x + n1 * bc.y + n2 * bc.z;

				// Interpolate vertex tcoords --> "fragment tcoords"
				Vector2 t0 = tcoords[vertex + 0];
				Vector2 t1 = tcoords[vertex + 1];
				Vector2 t2 = tcoords[vertex + 2];
				Vector2 t = t0 * bc.x + t1 * bc.y + t2 * bc.z;

				// Store interpolated vertex attributes, forward to fragment shader
				VertexAttributes va;
				va.p = p;
				va.n = n;
				va.t = t;

				// Color based on fragment shader output
				Vector3 rgb = fs(va, data);
				Color color = Float3ToColor(&rgb.x);
				SetPixel(image, x, y, color);
			}
		}
	}
}
