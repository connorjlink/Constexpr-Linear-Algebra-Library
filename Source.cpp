#define OLC_PGE_APPLICATION
#include "engine.hpp"

#include <fstream>
#include <sstream>

import stdex;
import core;
import vector;
import matrix;
import trig;
import mesh;

constexpr auto screenWidth = 1000.0f;
constexpr auto screenHeight = 1000.0f;

constexpr auto halfScreenWidth = screenWidth / 2.0f;
constexpr auto halfScreenHeight = screenHeight / 2.0f;

constexpr auto fov = 90.0f;
constexpr auto aspectRatio = (screenHeight / screenWidth);
constexpr auto nearPlane = 0.1f;
constexpr auto farPlane = 1000.0f;

template<typename T = float>
cla::v3d_generic<T> intersect(const cla::v3d_generic<T>& plane_p, const cla::v3d_generic<T>& plane_n, const cla::v3d_generic<T>& lineStart, const cla::v3d_generic<T>& lineEnd) noexcept
{
	auto planeN = cla::normalize(plane_n);
	float plane_d = -cla::dot(planeN, plane_p);

	float ad = cla::dot(lineStart, planeN);
	float bd = cla::dot(lineEnd, planeN);

	float t = (-plane_d - ad) / (bd - ad);

	cla::v3d_generic<T> lineStartToEnd = cla::reduce<std::minus<>>(lineEnd, lineStart);
	cla::v3d_generic<T> lineToIntersect = cla::apply<std::multiplies<>>(lineStartToEnd, t);

	return cla::reduce<std::plus<>>(lineStart, lineToIntersect);
}

int clip(cla::vf3d plane_p, cla::vf3d plane_n, cla::tri<float>& in_tri, cla::tri<float>& out_tri1, cla::tri<float>& out_tri2) noexcept
{
	plane_n = cla::normalize(plane_n);

	auto dist = [&](const cla::vf3d& p)
	{
		cla::vf3d n = cla::normalize(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - cla::dot(plane_n, plane_p));
	};

	cla::vf3d* inside_points[3];  int nInsidePointCount = 0;
	cla::vf3d* outside_points[3]; int nOutsidePointCount = 0;

	float d0 = dist(in_tri.p1);
	float d1 = dist(in_tri.p2);
	float d2 = dist(in_tri.p3);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p1; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p1; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p2; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p2; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p3; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p3; }


	if (nInsidePointCount == 3)
	{
		out_tri1 = in_tri;

		return 1;
	}

	else if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		out_tri1.p1 = *inside_points[0];
		out_tri1.p2 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p3 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1;
	}

	else if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		out_tri1.p1 = *inside_points[0];
		out_tri1.p2 = *inside_points[1];
		out_tri1.p3 = intersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		out_tri2.p1 = *inside_points[1];
		out_tri2.p2 = out_tri1.p3;
		out_tri2.p3 = intersect(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2;
	}

	else return 0;
}

cla::float4x4 inverse(const cla::float4x4& m) noexcept
{
	cla::float4x4 matrix;
	matrix[0][0] = m[0][0]; matrix[0][1] = m[1][0]; matrix[0][2] = m[2][0]; matrix[0][3] = 0.0f;
	matrix[1][0] = m[0][1]; matrix[1][1] = m[1][1]; matrix[1][2] = m[2][1]; matrix[1][3] = 0.0f;
	matrix[2][0] = m[0][2]; matrix[2][1] = m[1][2]; matrix[2][2] = m[2][2]; matrix[2][3] = 0.0f;
	matrix[3][0] = -(m[3][0] * matrix[0][0] + m[3][1] * matrix[1][0] + m[3][2] * matrix[2][0]);
	matrix[3][1] = -(m[3][0] * matrix[0][1] + m[3][1] * matrix[1][1] + m[3][2] * matrix[2][1]);
	matrix[3][2] = -(m[3][0] * matrix[0][2] + m[3][1] * matrix[1][2] + m[3][2] * matrix[2][2]);
	matrix[3][3] = 1.0f;
	return matrix;
}

std::vector<cla::tri<float>> load(const std::string& filename) noexcept
{
	std::ifstream f(filename);
	if (!f.is_open()) return {};

	std::vector<cla::tri<float>> tris;
	std::vector<cla::v3d_generic<float>> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::stringstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			cla::v3d_generic<float> v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			tris.emplace_back(cla::tri<float>(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
		}
	}

	return tris;
}

class Renderer : public olc::PixelGameEngine
{
public:
	Renderer()
	{
		sAppName = "Renderer";
	}

public:
	cla::float4x4 matProj, matRot;
	
	std::vector<cla::tri<float>> meshLoaded;

	float theta = 0.0f;

	cla::vf3d camera;
	cla::vf3d lookDir;
	
	float yaw, pitch;

	olc::Renderable gfxWireframe;

public:
	bool OnUserCreate() override
	{
		matProj = cla::projection(fov, aspectRatio, nearPlane, farPlane);

		gfxWireframe.Load("./solid.png");

		meshLoaded = load("./newnew.obj");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		if (GetKey(olc::Key::SPACE).bHeld) theta += fElapsedTime;


		if (GetKey(olc::Key::UP).bHeld)
			camera.y += 8.0f * fElapsedTime;

		if (GetKey(olc::Key::DOWN).bHeld)
			camera.y -= 8.0f * fElapsedTime;

		if (GetKey(olc::Key::R).bHeld)
			pitch -= 2.0f * fElapsedTime;

		if (GetKey(olc::Key::F).bHeld)
			pitch += 2.0f * fElapsedTime;


		
		if (GetKey(olc::Key::LEFT).bHeld)
			yaw -= 2.0f * fElapsedTime;

		if (GetKey(olc::Key::RIGHT).bHeld)
			yaw += 2.0f * fElapsedTime;


		cla::vf3d forward = cla::apply<std::multiplies<>>(lookDir, 8.0f * fElapsedTime);
		cla::vf3d up = cla::vf3d(0.0f, 1.0f, 0.0f);
		cla::vf3d right = cla::cross(up, forward);

		if (GetKey(olc::Key::W).bHeld)
			camera = cla::reduce<std::plus<>>(camera, forward);

		if (GetKey(olc::Key::S).bHeld)
			camera = cla::reduce<std::minus<>>(camera, forward);

		if (GetKey(olc::Key::A).bHeld)
			camera = cla::reduce<std::plus<>>(camera, right);

		if (GetKey(olc::Key::D).bHeld)
			camera = cla::reduce<std::minus<>>(camera, right);

		matRot = cla::rotationY(theta);

		cla::float4x4 matTrans = cla::translation(0.0f, 0.0f, 10.0f);

		cla::float4x4 matWorld;
		matWorld = cla::identity<4, float>();
		matWorld = cla::compose(matRot, matTrans);

	
		cla::vf3d target = cla::vf3d(0.0f, 0.0f, 1.0f);

		cla::float4x4 rotY = cla::rotationY(yaw);
		cla::float4x4 rotX = cla::rotationX(pitch);

		cla::float4x4 matCameraRot = cla::compose(rotY, rotX);
		lookDir = cla::mul(target, matCameraRot);
		target = cla::reduce<std::plus<>>(camera, lookDir);
		cla::float4x4 matCamera = cla::pointAt(camera, target, up);

		cla::float4x4 matView = inverse(matCamera);


		std::vector<std::pair<cla::tri<float>, float>> trisToRaster;

		for (auto& t : meshLoaded)
		{
			cla::tri<float> triTransformed, triViewed, triProjected;

			triTransformed.p1 = cla::mul(t.p1, matWorld);
			triTransformed.p2 = cla::mul(t.p2, matWorld);
			triTransformed.p3 = cla::mul(t.p3, matWorld);


			cla::vf3d normal, line1, line2;
			line1.x = triTransformed.p2.x - triTransformed.p1.x;
			line1.y = triTransformed.p2.y - triTransformed.p1.y;
			line1.z = triTransformed.p2.z - triTransformed.p1.z;

			line2.x = triTransformed.p3.x - triTransformed.p1.x;
			line2.y = triTransformed.p3.y - triTransformed.p1.y;
			line2.z = triTransformed.p3.z - triTransformed.p1.z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			// It's normally normal to normalise the normal
			float l = std::sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;
			
			cla::vf3d cameraRay = cla::reduce<std::minus<>>(triTransformed.p1, camera);

			if (normal.x * (triTransformed.p1.x - camera.x) +
				normal.y * (triTransformed.p1.y - camera.y) +
				normal.z * (triTransformed.p1.z - camera.z) < 0.0f)
			{
				// Illumination
				cla::vf3d light_direction = cla::vf3d(1.0f, 0.0f, 0.0f);
				float l = std::sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				float dp = cla::dot<true>(normal, light_direction);
				(((dp += 1.0f) *= 0.5f) *= 0.75f);

				triViewed.p1 = cla::mul(triTransformed.p1, matView);
				triViewed.p2 = cla::mul(triTransformed.p2, matView);
				triViewed.p3 = cla::mul(triTransformed.p3, matView);

				// additional triangles. 
				int clippedTris = 0;
				cla::tri<float> clipped[2];
				clippedTris = clip(cla::vf3d(0.0f, 0.0f, 0.1f), cla::vf3d(0.0f, 0.0f, 1.0f), triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < clippedTris; ++n)
				{

					triProjected.p1 = cla::mul(clipped[n].p1, matProj);
					triProjected.p2 = cla::mul(clipped[n].p2, matProj);
					triProjected.p3 = cla::mul(clipped[n].p3, matProj);


					triProjected.p1.x /= triProjected.p1.w;
					triProjected.p1.y /= triProjected.p1.w;
					triProjected.p1.z /= triProjected.p1.w;

					triProjected.p2.x /= triProjected.p2.w;
					triProjected.p2.y /= triProjected.p2.w;
					triProjected.p2.z /= triProjected.p2.w;

					triProjected.p3.x /= triProjected.p3.w;
					triProjected.p3.y /= triProjected.p3.w;
					triProjected.p3.z /= triProjected.p3.w;

					triProjected.p1.x *= -1.0f;
					triProjected.p2.x *= -1.0f;
					triProjected.p3.x *= -1.0f;

					triProjected.p1.y *= -1.0f;
					triProjected.p2.y *= -1.0f;
					triProjected.p3.y *= -1.0f;


					triProjected.p1.x += 1.0f; triProjected.p1.y += 1.0f;
					triProjected.p2.x += 1.0f; triProjected.p2.y += 1.0f;
					triProjected.p3.x += 1.0f; triProjected.p3.y += 1.0f;

					triProjected.p1.x *= halfScreenWidth;
					triProjected.p1.y *= halfScreenHeight;

					triProjected.p2.x *= halfScreenWidth;
					triProjected.p2.y *= halfScreenHeight;

					triProjected.p3.x *= halfScreenWidth;
					triProjected.p3.y *= halfScreenHeight;


					trisToRaster.emplace_back(std::make_pair(triProjected, dp));
				}
			}

			//std::sort(trisToRaster.begin(),trisToRaster.end(), [](std::pair<cla::tri<float>, float>& t1, std::pair<cla::tri<float>, float>& t2)
			//{
			//	float z1 = (t1.first.p1.z + t1.first.p2.z + t1.first.p3.z) * 0.3333333333f;
			//	float z2 = (t2.first.p1.z + t2.first.p2.z + t2.first.p3.z) * 0.3333333333f;
			//
			//	return z1 < z2;
			//});


			for (auto& triToRaster : trisToRaster)
			{
				cla::tri<float> clipped[2];
				std::list<std::pair<cla::tri<float>, float>> listTriangles;

				listTriangles.push_back(triToRaster);
				int newTris = 1;

				for (int p = 0; p < 4; p++)
				{
					int addTris = 0;
					while (newTris > 0)
					{
						cla::tri<float> test = listTriangles.front().first;
						listTriangles.pop_front();
						newTris--;

						switch (p)
						{
							case 0:	addTris = clip(cla::vf3d(0.0f, 0.0f, 0.0f               ), cla::vf3d(0.0f, 1.0f, 0.0f  ), test, clipped[0], clipped[1]); break;
							case 1:	addTris = clip(cla::vf3d(0.0f, screenHeight - 1.0f, 0.0f), cla::vf3d(0.0f, -1.0f, 0.0f ), test, clipped[0], clipped[1]); break;
							case 2:	addTris = clip(cla::vf3d(0.0f, 0.0f, 0.0f               ), cla::vf3d(1.0f, 0.0f, 0.0f  ), test, clipped[0], clipped[1]); break;
							case 3:	addTris = clip(cla::vf3d(screenWidth - 1.0f, 0.0f, 0.0f ), cla::vf3d(-1.0f, 0.0f, 0.0f ), test, clipped[0], clipped[1]); break;
						}

						for (int w = 0; w < addTris; ++w)
							listTriangles.push_back(std::make_pair(clipped[w], triToRaster.second));
					}

					newTris = static_cast<int>(listTriangles.size());
				}

				for (auto& t : listTriangles)
				{
					DrawPolygonDecal(gfxWireframe.Decal(),
					{
						{ t.first.p1.x, t.first.p1.y },
						{ t.first.p2.x, t.first.p2.y },
						{ t.first.p3.x, t.first.p3.y }
					}, { { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f } }, olc::PixelF(t.second, t.second, t.second, 1.0f));
				}
			}

			//DrawStringDecal({ 25.0f, 25.0f }, "FPS: " + std::to_string(1.0f / fElapsedTime), olc::YELLOW);

			trisToRaster.clear();
		}

		return true;
	}

	bool OnUserDestroy() override
	{
		return true;
	}
};

int main(void)
{
	Renderer app;

	if (app.Construct(static_cast<std::int32_t>(screenWidth), static_cast<std::int32_t>(screenHeight), 1, 1, false, false, false)) app.Start();
	else std::exit(EXIT_FAILURE);

	return 0;
}