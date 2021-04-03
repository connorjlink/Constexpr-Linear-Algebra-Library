#define OLC_PGE_APPLICATION
#include "engine.hpp"

#define lookSpeed 2.0f * fElapsedTime

import stdex;
import core;
import vector;
import matrix;

constexpr auto screenWidth = 1000.0f, halfScreenWidth = (screenWidth / 2.0f);
constexpr auto screenHeight = 1000.0f, halfScreenHeight = (screenHeight / 2.0f);

constexpr auto fov = 90.0f;
constexpr auto aspectRatio = (screenHeight / screenWidth);
constexpr auto nearPlane = 0.1f;
constexpr auto farPlane = 100.0f;

class Renderer : public olc::PixelGameEngine
{
public:
	Renderer()
	{
		sAppName = "Software 3D Renderer";
	}

public:
	cla::float4x4 matProj, matRot, matTrans;

	std::vector<cla::tri<float>> meshLoaded, trisToRaster;

	cla::vf3d cameraPos, cameraVel, lookDir;

	olc::Renderable gfxTexture;

	float theta = 0.0f, yaw = 0.0f, pitch = 0.0f;

public:
	bool OnUserCreate() override
	{
		matProj = cla::projection(fov, aspectRatio, nearPlane, farPlane);
		matTrans = cla::translation(0.0f, 0.0f, 10.0f);

		gfxTexture.Load("./solid.png");

		meshLoaded = cla::load("./new2.obj");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		cla::vf3d target = cla::vf3d(std::cosf(pitch) * std::sinf(yaw), std::sinf(pitch), std::cosf(pitch) * std::cosf(yaw));
		cla::vf3d forward = cla::apply<std::multiplies<>>(target, 0.5f * fElapsedTime);
		cla::vf3d up = cla::vf3d(0.0f, 1.0f, 0.0f);
		cla::vf3d right = cla::cross(up, forward);
		cla::vf3d angleUp = cla::apply<std::multiplies<>>(cla::cross(cla::normalize(forward), cla::normalize(right)), 0.025f);


		if (GetKey(olc::Key::SPACE).bHeld) theta += fElapsedTime;

		if (GetKey(olc::Key::R).bHeld) pitch -= lookSpeed;
		if (GetKey(olc::Key::F).bHeld) pitch += lookSpeed;

		if (GetKey(olc::Key::LEFT).bHeld)  yaw += lookSpeed;
		if (GetKey(olc::Key::RIGHT).bHeld) yaw -= lookSpeed;

		//if (GetKey(olc::Key::UP).bHeld)   camera.y += 8.0f * fElapsedTime;
		//if (GetKey(olc::Key::DOWN).bHeld) camera.y -= 8.0f * fElapsedTime;

		if (GetKey(olc::Key::K).bPressed)
		{
			cameraPos.y = 0.01f;
			cameraVel.y += 0.02f;
		}

		(cameraPos.y > 0.0f) ? (cameraVel.y -= 0.025f * fElapsedTime) : (cameraVel.y = 0.0f);

		if (GetKey(olc::Key::UP).bHeld)   cameraPos = cla::reduce<std::plus<>>(cameraPos, angleUp);
		if (GetKey(olc::Key::DOWN).bHeld) cameraPos = cla::reduce<std::minus<>>(cameraPos, angleUp);

		if (GetKey(olc::Key::W).bHeld) cameraVel = cla::reduce<std::plus<>>(cameraVel, forward);
		if (GetKey(olc::Key::S).bHeld) cameraVel = cla::reduce<std::minus<>>(cameraVel, forward);

		if (GetKey(olc::Key::A).bHeld) cameraVel = cla::reduce<std::plus<>>(cameraVel, right);
		if (GetKey(olc::Key::D).bHeld) cameraVel = cla::reduce<std::minus<>>(cameraVel, right);

		cameraPos = cla::reduce<std::plus<>>(cameraPos, cameraVel);

		cameraVel.x /= ((fElapsedTime * 5.0f) + 1.25f);
		cameraVel.z /= ((fElapsedTime * 5.0f) + 1.25f);

		cla::float4x4 matWorld = cla::compose(cla::rotationY(theta), matTrans);

		target = cla::reduce<std::plus<>>(cameraPos, target);

		cla::float4x4 matView = cla::inverse(cla::pointAt(cameraPos, target, up));

		for (auto& t : meshLoaded)
		{
			cla::tri<float> triTransformed, triViewed, triProjected;

			triTransformed.p1 = cla::mul(t.p1, matWorld);
			triTransformed.p2 = cla::mul(t.p2, matWorld);
			triTransformed.p3 = cla::mul(t.p3, matWorld);

			cla::vf3d line1 = cla::reduce<std::minus<>>(triTransformed.p2, triTransformed.p1);
			cla::vf3d line2 = cla::reduce<std::minus<>>(triTransformed.p3, triTransformed.p1);

			cla::vf3d normal = cla::normalize(cla::cross(line1, line2));

			cla::vf3d cameraRay = cla::reduce<std::minus<>>(triTransformed.p1, cameraPos);

			if (cla::dot(normal, cameraRay) < 0.0f)
			{
				cla::vf3d light_direction = cla::normalize(cla::vf3d(1.0f, 0.0f, 0.0f));

				float dp = ((std::powf(50.0f, ((cla::dot(normal, light_direction) + 1.0f) * 0.5f)) - 1.0f) * 0.02f);

				triViewed.p1 = cla::mul(triTransformed.p1, matView);
				triViewed.p2 = cla::mul(triTransformed.p2, matView);
				triViewed.p3 = cla::mul(triTransformed.p3, matView);

				int clippedTris = 0;
				cla::tri<float> clipped[2];
				clippedTris = cla::clip(cla::vf3d(0.0f, 0.0f, 0.1f), cla::vf3d(0.0f, 0.0f, 1.0f), triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < clippedTris; ++n)
				{
					triProjected.p1 = cla::mul(clipped[n].p1, matProj);
					triProjected.p2 = cla::mul(clipped[n].p2, matProj);
					triProjected.p3 = cla::mul(clipped[n].p3, matProj);

					triProjected.p1 = cla::apply<std::divides<>>(triProjected.p1, triProjected.p1.w);
					triProjected.p2 = cla::apply<std::divides<>>(triProjected.p2, triProjected.p2.w);
					triProjected.p3 = cla::apply<std::divides<>>(triProjected.p3, triProjected.p3.w);

					triProjected.p1 = cla::apply<std::multiplies<>>(triProjected.p1, -1.0f);
					triProjected.p2 = cla::apply<std::multiplies<>>(triProjected.p2, -1.0f);
					triProjected.p3 = cla::apply<std::multiplies<>>(triProjected.p3, -1.0f);

					triProjected.p1 = cla::apply<std::plus<>>(triProjected.p1, 1.0f);
					triProjected.p2 = cla::apply<std::plus<>>(triProjected.p2, 1.0f);
					triProjected.p3 = cla::apply<std::plus<>>(triProjected.p3, 1.0f);

					triProjected.p1.x *= halfScreenWidth;  triProjected.p2.x *= halfScreenWidth;  triProjected.p3.x *= halfScreenWidth;
					triProjected.p1.y *= halfScreenHeight; triProjected.p2.y *= halfScreenHeight; triProjected.p3.y *= halfScreenHeight;

					trisToRaster.emplace_back(cla::tri<float>(triProjected.p1, triProjected.p2, triProjected.p3, olc::PixelF(dp, dp, dp, 1.0f)) );
				}
			}
		}

		std::sort(trisToRaster.begin(), trisToRaster.end(), [](cla::tri<float>& t1, cla::tri<float>& t2)
		{
			float z1 = (t1.p1.z + t1.p2.z + t1.p3.z) * 0.3333333333f;
			float z2 = (t2.p1.z + t2.p2.z + t2.p3.z) * 0.3333333333f;

			return z1 < z2;
		});


		for (auto& triToRaster : trisToRaster)
		{
			cla::tri<float> clipped[2];
			std::list<cla::tri<float>> listTriangles;

			listTriangles.push_back(triToRaster);
			std::size_t newTris = 1z;

			for (int p = 0; p < 4; p++)
			{
				int addTris = 0;
				while (newTris > 0)
				{
					cla::tri<float> test = listTriangles.front();
					listTriangles.pop_front();
					newTris--;

					switch (p)
					{
						case 0: addTris = cla::clip(cla::vf3d(0.0f, 0.0f, 0.0f), cla::vf3d(0.0f, 1.0f, 0.0f), test, clipped[0], clipped[1]); break;
						case 1: addTris = cla::clip(cla::vf3d(0.0f, screenHeight - 1.0f, 0.0f), cla::vf3d(0.0f, -1.0f, 0.0f), test, clipped[0], clipped[1]); break;
						case 2: addTris = cla::clip(cla::vf3d(0.0f, 0.0f, 0.0f), cla::vf3d(1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
						case 3: addTris = cla::clip(cla::vf3d(screenWidth - 1.0f, 0.0f, 0.0f), cla::vf3d(-1.0f, 0.0f, 0.0f), test, clipped[0], clipped[1]); break;
					}

					for (int w = 0; w < addTris; ++w)
						listTriangles.emplace_back(cla::tri<float>{ clipped[w].p1, clipped[w].p2, clipped[w].p3, triToRaster.col });
				}

				newTris = listTriangles.size();
			}

			for (auto& t : listTriangles)
			{
				DrawPolygonDecal(gfxTexture.Decal(),
				{
					{ t.p1.x, t.p1.y },
					{ t.p2.x, t.p2.y },
					{ t.p3.x, t.p3.y }
				}, { { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f } }, t.col);
			}
		}

		trisToRaster.clear();

		DrawStringDecal({ 10.0f, 10.0f }, cameraPos.str(), olc::YELLOW);

		return !(GetKey(olc::Key::ESCAPE).bPressed);
	}

	bool OnUserDestroy() override
	{
		return true;
	}
};

int main(void)
{
	Renderer app;

	if (app.Construct((int)screenWidth, (int)screenHeight, 1, 1, false, false, false)) app.Start();
	else std::exit(EXIT_FAILURE);

	return 0;
}