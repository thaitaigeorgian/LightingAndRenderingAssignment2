#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Window.h"
#include "Camera.h"
#include "Time.h"
#include "Mesh.h"
#include "Math.h"


void Render1();
void Render2();
void Render3();
void Render4();
void Render5();
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
	//Render1();
	//Render2();
	//Render3();
	Render4();
};

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


Vector3 ShaderWhite(const VertexAttributes& atr, const UniformData& data)
{
	return { 1.0f, 1.0f, 1.0f };
}

void Render2()
{
	ClearColor(&gImageCPU, WHITE);
	float tt = TotalTime();

	Matrix view = LookAt({ 0, 0, 10 }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);

	Matrix world = RotateY(tt);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;

	data.lightColor = { 1.0f, 1.0f, 1.0f };
	data.lightDirection = Normalize(Vector3{ -1.0f, -1.0f, -1.0f });
	data.tex = nullptr;

	DrawMesh(&gImageCPU, gMeshCube, data, ShaderDiffuse);
}


Vector3 ShaderPhong(const VertexAttributes& atr, const UniformData& data)
{
	Vector3 fragPos = atr.p;

	Vector3 N;
	float lenN = sqrtf(atr.n.x * atr.n.x + atr.n.y * atr.n.y + atr.n.z * atr.n.z);
	N.x = atr.n.x / lenN;
	N.y = atr.n.y / lenN;
	N.z = atr.n.z / lenN;

	Vector3 L;
	L.x = data.lightDirection.x - fragPos.x;
	L.y = data.lightDirection.y - fragPos.y;
	L.z = data.lightDirection.z - fragPos.z;
	float lenL = sqrtf(L.x * L.x + L.y * L.y + L.z * L.z);
	L.x /= lenL; L.y /= lenL; L.z /= lenL;

	Vector3 V;
	V.x = 0.0f - fragPos.x;
	V.y = 0.0f - fragPos.y;
	V.z = 10.0f - fragPos.z;
	float lenV = sqrtf(V.x * V.x + V.y * V.y + V.z * V.z);
	V.x /= lenV; V.y /= lenV; V.z /= lenV;

	float NdotL = N.x * L.x + N.y * L.y + N.z * L.z;
	Vector3 R;
	R.x = 2.0f * NdotL * N.x - L.x;
	R.y = 2.0f * NdotL * N.y - L.y;
	R.z = 2.0f * NdotL * N.z - L.z;

	float ambientStrength = 0.25f;
	float diffuseStrength = 0.75f;
	float specularStrength = 1.0f;
	int shininess = 32;

	float diff = NdotL;
	if (diff < 0.0f) diff = 0.0f;

	float RdotV = R.x * V.x + R.y * V.y + R.z * V.z;
	if (RdotV < 0.0f) RdotV = 0.0f;
	float spec = powf(RdotV, shininess);

	Vector3 ambient;
	ambient.x = ambientStrength;
	ambient.y = ambientStrength;
	ambient.z = ambientStrength;

	Vector3 diffuse;
	diffuse.x = diffuseStrength * diff;
	diffuse.y = diffuseStrength * diff;
	diffuse.z = diffuseStrength * diff;

	Vector3 specular;
	specular.x = specularStrength * spec;
	specular.y = specularStrength * spec;
	specular.z = specularStrength * spec;

	Vector3 color;
	color.x = ambient.x + diffuse.x + specular.x;
	color.y = ambient.y + diffuse.y + specular.y;
	color.z = ambient.z + diffuse.z + specular.z;

	if (color.x > 1.0f) color.x = 1.0f;
	if (color.y > 1.0f) color.y = 1.0f;
	if (color.z > 1.0f) color.z = 1.0f;

	return color;
}

void Render3()
{
	float tt = TotalTime();

	Matrix view = LookAt({ 0,0,10 }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);

	float py = 5.0f * cosf(tt);
	Matrix world = Translate(0.0f, py, 5.0f);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;
	data.tex = nullptr;
	data.lightColor = { 1,1,1 };
	data.lightDirection = { 0, 10, 5 };

	
	DrawMesh(&gImageCPU, gMeshHead, data, ShaderPhong);
}

Vector3 ShaderSpotlight(const VertexAttributes& atr, const UniformData& data)
{
	Vector3 fragPos = atr.p;

	Vector3 N;
	float lenN = sqrtf(atr.n.x * atr.n.x + atr.n.y * atr.n.y + atr.n.z * atr.n.z);
	N.x = atr.n.x / lenN; N.y = atr.n.y / lenN; N.z = atr.n.z / lenN;

	Vector3 lightPos = { 0.0f, 8.0f, 0.0f };
	Vector3 lightDir = { 0.0f, -1.0f, 0.0f };
	float radius = 10.0f;
	float cutoff = cosf(45.0f * 3.14159f / 180.0f);

	Vector3 L;
	L.x = lightPos.x - fragPos.x;
	L.y = lightPos.y - fragPos.y;
	L.z = lightPos.z - fragPos.z;
	float dist = sqrtf(L.x * L.x + L.y * L.y + L.z * L.z);
	L.x /= dist; L.y /= dist; L.z /= dist;

	float theta = L.x * lightDir.x + L.y * lightDir.y + L.z * lightDir.z;
	if (theta < cutoff || dist > radius)
		return Vector3{ 0.0f, 0.0f, 0.0f };

	float diff = N.x * L.x + N.y * L.y + N.z * L.z;
	if (diff < 0.0f) diff = 0.0f;

	float ambientStrength = 0.1f;
	float diffuseStrength = 1.0f;

	Vector3 ambient = { ambientStrength, ambientStrength, ambientStrength };

	Vector3 diffuse;
	diffuse.x = diffuseStrength * diff;
	diffuse.y = diffuseStrength * diff;
	diffuse.z = diffuseStrength * diff;

	Vector3 color;
	color.x = ambient.x + diffuse.x;
	color.y = ambient.y + diffuse.y;
	color.z = ambient.z + diffuse.z;

	if (color.x > 1.0f) color.x = 1.0f;
	if (color.y > 1.0f) color.y = 1.0f;
	if (color.z > 1.0f) color.z = 1.0f;

	return color;
}

void Render4()
{
	ClearColor(&gImageCPU, WHITE);
	ClearDepth(&gImageCPU, 1.0f);

	float tt = TotalTime();

	Matrix view = LookAt({ 0, 0, 10 }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);

	Matrix world = RotateY(tt);

	UniformData data;
	data.world = world;
	data.mvp = world * view * proj;
	data.lightDirection = { 0.0f, 8.0f, 0.0f };
	data.tex = nullptr;

	DrawMesh(&gImageCPU, gMeshHexagon, data, ShaderSpotlight);
}



void Render5()
{
	ClearColor(&gImageCPU, WHITE);
	float tt = TotalTime();

	Matrix view = LookAt({ 0, 0, 10 }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);

	Matrix cubeWorld = RotateY(tt);
	UniformData cubeData;
	cubeData.world = cubeWorld;
	cubeData.mvp = cubeWorld * view * proj;
	cubeData.lightColor = { 1.0f, 1.0f, 1.0f };
	cubeData.lightDirection = Normalize(Vector3{ -1.0f, -1.0f, -1.0f });
	cubeData.tex = nullptr;
	DrawMesh(&gImageCPU, gMeshCube, cubeData, ShaderDiffuse);

	float radius = 4.0f;
	float angle = tt * 2.0f;
	float px = radius * cosf(angle);
	float py = radius * sinf(angle);
	Matrix sphereWorld = Translate(px, py, 0.0f);
	UniformData sphereData;
	sphereData.world = sphereWorld;
	sphereData.mvp = sphereWorld * view * proj;
	sphereData.tex = nullptr;
	sphereData.lightColor = { 1.0f, 1.0f, 1.0f };
	sphereData.lightDirection = Normalize(Vector3{ -1.0f, -1.0f, -1.0f });
	DrawMesh(&gImageCPU, gMeshSphere, sphereData, ShaderTcoords);
}
