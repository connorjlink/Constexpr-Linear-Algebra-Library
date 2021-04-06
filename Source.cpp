#define OLC_PGE_APPLICATION
#include "engine.hpp"

#define lookSpeed 2.0f * fElapsedTime

#undef min
#undef max

import stdex;
import core;
import vector;
import matrix;
import trig;

constexpr auto screenWidth = 600.0f, halfScreenWidth = (screenWidth / 2.0f);
constexpr auto screenHeight = 600.0f, halfScreenHeight = (screenHeight / 2.0f);

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

	cla::vf3d cameraAcc, cameraVel, cameraPos, lookDir;

	olc::Renderable gfxTexture;

	float theta = 0.0f, yaw = 0.0f, pitch = 0.0f;

	//θωερψτυιοπλκςηγφδσαζχξωβνμ
	//ΘΩΕΡΤΨΥΙΟΠΑΣΔΦΓΗςΚΛΖΧΞΩΒΝΜ ∫

	float m = 1.0f;
	float g = -10.0f;
	float µ = 100.0f;
	float λ = 50.0f;

	float α = (µ * g);
	float ω = (λ * g);

public:
	bool OnUserCreate() override
	{
		matProj = cla::projection(fov, aspectRatio, nearPlane, farPlane);
		matTrans = cla::translation(0.0f, 0.0f, 10.0f);

		gfxTexture.Load("./wireframe.png");

		meshLoaded = cla::loadOBJ("./chess.obj");

		for (auto& t : meshLoaded) t.texture = gfxTexture.Decal();


		trisToRaster.reserve(meshLoaded.size());
		trisToRaster.resize(meshLoaded.size());

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		auto applyForce = [&](cla::vf3d force, cla::vf3d& accVec)
		{
			auto f = (force / m);

			accVec += (f * fElapsedTime);
		};

		auto update = [&]()
		{
			cameraVel += (cameraAcc * fElapsedTime);
			cameraPos += (cameraVel * fElapsedTime);

			cameraAcc ^ 0.0f;
		};

		float mouseSens = 0.002f;

		olc::vf2d mouseDifference;
		olc::vf2d mouseNow = { (float)GetMouseX(), (float)GetMouseY() };
		olc::vf2d screenCenter = { halfScreenWidth, halfScreenHeight };

		if (mouseNow != screenCenter)
		{
			mouseDifference = screenCenter - mouseNow;

			yaw += mouseSens * mouseDifference.x;
			pitch += mouseSens * mouseDifference.y;

			SetCursorPos(int(338.0f + halfScreenWidth), int(362.0f + halfScreenHeight));

			if (pitch >= cla::radians(+50.0f)) pitch = cla::radians(+50.0f);
			if (pitch <= cla::radians(-50.0f)) pitch = cla::radians(-50.0f);
		}

		DrawStringDecal({ 10.0f, 55.0f }, mouseNow.str(), olc::YELLOW);

		cla::vf3d target = { std::cosf(pitch) * std::sinf(yaw), std::sinf(pitch), std::cosf(pitch) * std::cosf(yaw) };
		cla::vf3d forward = { (target.x * (10000.0f * fElapsedTime)), 0.0f, (target.z * (10000.0f * fElapsedTime)) };

		cla::vf3d up = { 0.0f, 1.0f, 0.0f };
		cla::vf3d right = cla::cross(up, forward);


		if (GetKey(olc::Key::SPACE).bHeld and cameraPos.y < 0.02f)
		{
			cameraPos.y = 0.01f;

			cameraVel.y = 7.0f;
		}

		if (GetKey(olc::Key::W).bHeld) cameraAcc += forward;
		if (GetKey(olc::Key::S).bHeld) cameraAcc -= forward;

		if (GetKey(olc::Key::A).bHeld) cameraAcc += right;
		if (GetKey(olc::Key::D).bHeld) cameraAcc -= right;


		if (cameraPos.y <= 0.02f) applyForce({ cameraVel.x * α, 0.0f, cameraVel.z * α }, cameraAcc);
		else applyForce({ cameraVel.x * ω, 0.0f, cameraVel.z * ω }, cameraAcc);

		if (cameraPos.y > 0.0f) cameraVel.y += (g * fElapsedTime);
		else cameraVel.y = 0.0f, cameraAcc.y = 0.0f;
		

		update();

		cla::float4x4 matWorld = cla::rotationY(theta) * matTrans;

		target += cameraPos;

		cla::float4x4 matView = ~cla::pointAt(cameraPos, target, up);

		for (auto& t : meshLoaded)
		{
			cla::tri<float> triTransformed, triViewed, triProjected;

			triTransformed.p1 = t.p1 * matWorld;
			triTransformed.p2 = t.p2 * matWorld;
			triTransformed.p3 = t.p3 * matWorld;

			cla::vf3d line1 = triTransformed.p2 - triTransformed.p1;
			cla::vf3d line2 = triTransformed.p3 - triTransformed.p1;

			cla::vf3d normal = cla::normalize(cla::cross(line1, line2));

			cla::vf3d cameraRay = triTransformed.p1 - cameraPos;

			if (cla::dot(normal, cameraRay) < 0.0f)
			{
				cla::vf3d light_direction = cla::normalize({ 0.0f, 1.0f, 0.0f });

				float dp = ((std::powf(50.0f, ((cla::dot(normal, light_direction) + 1.0f) * 0.5f)) - 1.0f) * 0.02f);

				triViewed.p1 = cla::mul(triTransformed.p1, matView);
				triViewed.p2 = cla::mul(triTransformed.p2, matView);
				triViewed.p3 = cla::mul(triTransformed.p3, matView);

				triViewed.p1 = triTransformed.p1 * matView;
				triViewed.p2 = triTransformed.p2 * matView;
				triViewed.p3 = triTransformed.p3 * matView;

				int clippedTris = 0;
				cla::tri<float> clipped[2];
				clippedTris = cla::clip({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < clippedTris; ++n)
				{
					triProjected.p1 = clipped[n].p1 * matProj;
					triProjected.p2 = clipped[n].p2 * matProj;
					triProjected.p3 = clipped[n].p3 * matProj;


					triProjected.p1 = (triProjected.p1 / triProjected.p1.w);
					triProjected.p2 = (triProjected.p2 / triProjected.p2.w);
					triProjected.p3 = (triProjected.p3 / triProjected.p3.w);


					triProjected.p1 = -triProjected.p1;
					triProjected.p2 = -triProjected.p2;
					triProjected.p3 = -triProjected.p3;


					triProjected.p1 = (triProjected.p1 + 1.0f);
					triProjected.p2 = (triProjected.p2 + 1.0f);
					triProjected.p3 = (triProjected.p3 + 1.0f);


					triProjected.p1.x *= halfScreenWidth;  triProjected.p2.x *= halfScreenWidth;  triProjected.p3.x *= halfScreenWidth;
					triProjected.p1.y *= halfScreenHeight; triProjected.p2.y *= halfScreenHeight; triProjected.p3.y *= halfScreenHeight;

					trisToRaster.emplace_back(cla::tri<float>(triProjected.p1, triProjected.p2, triProjected.p3, olc::PixelF(dp, dp, dp, 1.0f), t.texture) );
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
						listTriangles.emplace_back(cla::tri{ clipped[w].p1, clipped[w].p2, clipped[w].p3, triToRaster.lightVal, triToRaster.texture });
				}

				newTris = listTriangles.size();
			}

			for (auto& t : listTriangles)
			{
				DrawPolygonDecal(t.texture,
				//std::array<olc::vf2d, 3>
				{
					olc::vf2d{ t.p1.x, t.p1.y },
					olc::vf2d{ t.p2.x, t.p2.y },
					olc::vf2d{ t.p3.x, t.p3.y }
				}, t.lightVal);
			}
		}

		trisToRaster.clear();

		DrawStringDecal({ 10.0f, 10.0f }, cameraPos.str(), olc::YELLOW);
		DrawStringDecal({ 10.0f, 25.0f }, cameraVel.str(), olc::YELLOW);
		DrawStringDecal({ 10.0f, 40.0f }, cameraAcc.str(), olc::YELLOW);

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

	if (app.Construct((int)screenWidth, (int)screenHeight, 2, 2, false, false, false)) app.Start();
	else std::exit(EXIT_FAILURE);

	return 0;
}