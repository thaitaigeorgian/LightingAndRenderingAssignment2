#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Window.h"
#include "Camera.h"
#include "Time.h"
#include "Mesh.h"

void Example1();

static Image fTexHead;

void RasterizationScene::OnLoad()
{
	CreateImageFromFile(&fTexHead, "./assets/textures/african_head_diffuse.png", true);
}

void RasterizationScene::OnUnload()
{
	DestroyImage(&fTexHead);
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	ClearDepth(&gImageCPU, 1.0f);
	Render1();
}

Vector3 ShaderPositions(const VertexAttributes& atr, const UniformData& data)
{
	return atr.p;
}

Vector3 ShaderNormals(const VertexAttributes& atr, const UniformData& data)
{
	return atr.n;
}

Vector3 ShaderTcoords(const VertexAttributes& atr, const UniformData& data)
{
	return atr.t;
}

Vector3 ShaderTexture(const VertexAttributes& atr, const UniformData& data)
{
	int px = atr.t.x * (data.tex->width - 1);
	int py = atr.t.y * (data.tex->height - 1);
	Color tex_col = GetPixel(*data.tex, px, py);
	Vector3 tex_rgb = { tex_col.r / 255.0f, tex_col.g / 255.0f, tex_col.b / 255.0f };
	return tex_rgb;
}

Vector3 ShaderDiffuse(const VertexAttributes& atr, const UniformData& data)
{
	// "Fake" diffuse lighting by comparing similarity with normals along the z-axis:
	float dotNL = fmaxf(Dot(atr.n, data.lightDirection), 0.0f); // Effectively diffuse lighting (enjoy your A2 solution)
	Vector3 lighting = data.lightColor * dotNL;
	return lighting;
}

void Render1()
{
	float tt = fmodf(TotalTime(), 3.0f);  // reset every 3s

	float radius = 8.0f * (1.0f - tt / 3.0f);
	float angle = tt * 6.0f;
	float px = radius * cosf(angle);
	float py = radius * sinf(angle);

	Matrix view = LookAt({ 0, 0, 10 }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90 * DEG2RAD, 1.0f, 0.1f, 100.0f);

	Matrix world = Translate(px, py, 0.0f);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;
	data.tex = nullptr;
	data.lightColor = V3_ZERO;
	data.lightDirection = V3_ZERO;

	DrawMesh(&gImageCPU, gMeshSphere, data, ShaderTcoords);
}
