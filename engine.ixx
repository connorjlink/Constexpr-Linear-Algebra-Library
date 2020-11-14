module;
#include "stb_image.hpp"
#include <tuple>
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <compare>
#include <cstddef>
#include <type_traits>
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef NOMINMAX
#undef VC_EXTRALEAN
#undef WIN32_LEAN_AND_MEAN
#include <Shlwapi.h>
#include <windows.h>
#include <dwmapi.h>
#include <GL/gl.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "opengl32.lib")
export module engine;

//import stdex;
//import core;
import vector;

namespace cla
{
	using int2 = std::tuple<int, int>;
	using float2 = std::tuple<float, float>;
}

export namespace core
{
	constexpr std::uint8_t nMouseButtons = 5;
	constexpr std::uint8_t nDefaultAlpha = 0xFF;
	constexpr std::uint32_t nDefaultPixel = (nDefaultAlpha << 24);
	enum rcode { FAIL = 0, OK = 1, NO_FILE = -1 };

	struct Pixel
	{
		union
		{
			std::uint32_t n = nDefaultPixel;

			struct
			{ 
				std::uint8_t r, g, b, a; 
			};
		};

		enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

		constexpr Pixel() noexcept : r(), g(), b(), a(nDefaultAlpha) {}
		constexpr Pixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = nDefaultAlpha) noexcept : n(red | (green << 8) | (blue << 16) | (alpha << 24)) {}
		constexpr Pixel(std::uint32_t p) noexcept : n(p) {}
		constexpr bool operator==(const Pixel& p) const noexcept { return n == p.n; }
		constexpr bool operator!=(const Pixel& p) const noexcept { return n != p.n; }
	};

	[[nodiscard]]
	constexpr Pixel PixelF(float red, float green, float blue, float alpha = 1.0f) noexcept 
	{
		return Pixel{ std::uint8_t(red * 255.f), std::uint8_t(green * 255.f), std::uint8_t(blue * 255.f), std::uint8_t(alpha * 255.f) };
	}


	constexpr Pixel
		GREY(192, 192, 192),  DARK_GREY(128, 128, 128),  VERY_DARK_GREY(64, 64, 64),
		RED(255, 0, 0),       DARK_RED(128, 0, 0),       VERY_DARK_RED(64, 0, 0),
		YELLOW(255, 255, 0),  DARK_YELLOW(128, 128, 0),  VERY_DARK_YELLOW(64, 64, 0),
		GREEN(0, 255, 0),     DARK_GREEN(0, 128, 0),     VERY_DARK_GREEN(0, 64, 0),
		CYAN(0, 255, 255),    DARK_CYAN(0, 128, 128),    VERY_DARK_CYAN(0, 64, 64),
		BLUE(0, 0, 255),      DARK_BLUE(0, 0, 128),      VERY_DARK_BLUE(0, 0, 64),
		MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
		WHITE(255, 255, 255), BLACK(0, 0, 0),            BLANK(0, 0, 0, 0);

	enum Key
	{
		NONE,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		UP, DOWN, LEFT, RIGHT,
		SPACE, TAB, SHIFT, CTRL, INS, DEL, HOME, END, PGUP, PGDN,
		BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
		NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
		NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
		PLUS, MINUS, ALT
	};

	struct HWButton
	{
		bool bPressed = false, bReleased = false, bHeld = false;
	};

	class Sprite
	{
	public:
		constexpr Sprite() noexcept : pColData(), width(), height() {}

		Sprite(const std::string& sImageFile);

		Sprite(std::int32_t w, std::int32_t h) : width(w), height(h), pColData{ new Pixel[w * h] } {}
		
		Sprite(const core::Sprite&) = delete;
		
		constexpr Sprite(Sprite&& spr) noexcept : width(spr.width), height(spr.height), pColData(spr.pColData) 
		{
			spr.pColData = nullptr;
		}
		
		~Sprite() noexcept { delete[] pColData; }

	public:
		std::int32_t width = 0;
		std::int32_t height = 0;
		enum Mode { NORMAL, PERIODIC };
		enum Flip { NONE = 0, HORIZ = 1, VERT = 2 };

		Pixel* pColData = nullptr;

		auto GetData() const
		{
			return pColData;
		}

		bool SetPixel(const std::int32_t& x, const std::int32_t& y, core::Pixel&& p)
		{
			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				pColData[y * width + x] = p;
				return true;
			}
			else
				return false;
		}
	};

	class Decal
	{
	public:
		Decal(core::Sprite* spr);
		virtual ~Decal();
		void Update();

	public:
		std::int32_t id = -1;
		core::Sprite* sprite = nullptr;
		cla::float2 vUVScale = { 1.0f, 1.0f };
	};

	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;
		core::rcode Load(const std::string& sFile);
		void Create(std::uint32_t width, std::uint32_t height);
		core::Decal* Decal() const;
		core::Sprite* Sprite() const;

	private:
		std::unique_ptr<core::Sprite> pSprite = nullptr;
		std::unique_ptr<core::Decal> pDecal = nullptr;
	};

	struct DecalInstance
	{
		core::Decal* decal = nullptr;
		cla::float2 pos[4] = { { 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
		cla::float2 uv[4] = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		float w[4] = { 1, 1, 1, 1 };
		core::Pixel tint[4] = { core::WHITE, core::WHITE, core::WHITE, core::WHITE };;
	};


	struct LayerDesc
	{
		cla::float2 vOffset = { 0, 0 };
		cla::float2 vScale = { 1, 1 };
		bool bShow = false, bUpdate = false;
		core::Sprite* pDrawTarget = nullptr;
		std::uint32_t nResID = 0;
		std::vector<DecalInstance> vecDecalInstance;
		core::Pixel tint = core::WHITE;
		std::function<void()> funcHook = nullptr;
	};

	std::map<std::size_t, std::uint8_t> mapKeys;

	core::rcode Construct(std::int32_t screen_w, std::int32_t screen_h, std::int32_t pixel_w, std::int32_t pixel_h, bool full_screen = false, bool vsync = false, bool cohesion = false);
	core::rcode Start();

	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
	bool OnUserDestroy();

	bool IsFocused() noexcept;
	HWButton GetKey(Key k) noexcept;
	HWButton GetMouse(std::uint32_t b) noexcept;
	std::int32_t GetMouseX() noexcept;
	std::int32_t GetMouseY() noexcept;
	std::int32_t GetMouseWheel() noexcept;
	const cla::int2& GetWindowMouse() noexcept;
	const cla::int2& GetMousePos() noexcept;
	void SetMousePos(cla::int2&& pos) noexcept;
	std::int32_t ScreenWidth() noexcept;
	std::int32_t ScreenHeight() noexcept;
	std::int32_t GetDrawTargetWidth() noexcept;
	std::int32_t GetDrawTargetHeight() noexcept;
	core::Sprite* GetDrawTarget() noexcept;
	void SetScreenSize(int w, int h);
	void SetDrawTarget(Sprite* target) noexcept;
	std::uint32_t GetFPS() noexcept;
	float GetElapsedTime() noexcept;
	const cla::int2& GetWindowSize() noexcept;
	const cla::int2& GetPixelSize() noexcept;
	const cla::int2& GetScreenPixelSize() noexcept;
	void SetDrawTarget(std::uint8_t layer) noexcept;
	void SetLayerOffset(std::uint8_t layer, const cla::float2& offset) noexcept;
	void SetLayerOffset(std::uint8_t layer, float x, float y) noexcept;
	void SetLayerScale(std::uint8_t layer, const cla::float2& scale) noexcept;
	void SetLayerScale(std::uint8_t layer, float x, float y) noexcept;
	void SetLayerTint(std::uint8_t layer, const core::Pixel& tint) noexcept;
	void SetLayerCustomRenderFunction(std::uint8_t layer, std::function<void()> f) noexcept;
	std::vector<LayerDesc>& GetLayers()  noexcept;
	std::uint32_t CreateLayer();
	void SetPixelMode(Pixel::Mode m);
	Pixel::Mode GetPixelMode();
	void SetPixelMode(std::function<core::Pixel(const int x, const int y, const core::Pixel& pSource, const core::Pixel& pDest)> pixelMode);
	void SetPixelBlend(float fBlend);
	void DrawDecal(const cla::float2& pos, core::Decal* decal, const cla::float2& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialDecal(const cla::float2& pos, core::Decal* decal, const cla::float2& source_pos, const cla::float2& source_size, const cla::float2& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialDecal(const cla::float2& pos, const cla::float2& size, core::Decal* decal, const cla::float2& source_pos, const cla::float2& source_size, const core::Pixel& tint = core::WHITE);
	void DrawExplicitDecal(core::Decal* decal, const cla::float2* pos, const cla::float2* uv, const core::Pixel* col);
	void DrawWarpedDecal(core::Decal* decal, const std::array<cla::float2, 4>& pos, const core::Pixel& tint = core::WHITE);
	void DrawPartialWarpedDecal(core::Decal* decal, const std::array<cla::float2, 4>& pos, const cla::float2& source_pos, const cla::float2& source_size, const core::Pixel& tint);
	void DrawRotatedDecal(const cla::float2& pos, core::Decal* decal, const float fAngle, const cla::float2& center = { 0.0f, 0.0f }, const cla::float2& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialRotatedDecal(const cla::float2& pos, core::Decal* decal, const float fAngle, const cla::float2& center, const cla::float2& source_pos, const cla::float2& source_size, const cla::float2& scale = { 1.0f, 1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawStringDecal(const cla::float2& pos, const std::string& sText, const Pixel col = core::WHITE, const cla::float2& scale = { 1.0f, 1.0f });
	void FillRectDecal(const cla::float2& pos, const cla::float2& size, const core::Pixel col = core::WHITE);
	void GradientFillRectDecal(const cla::float2& pos, const cla::float2& size, const core::Pixel&& colTL, const core::Pixel&& colBL, const core::Pixel&& colBR, const core::Pixel&& colTR);
	cla::int2 GetTextSize(const std::string& s);
	void Clear(Pixel p);
	void ClearBuffer(Pixel p, bool bDepth = true);

	std::string sAppName;
	Sprite* pDrawTarget = nullptr;
	Pixel::Mode nPixelMode = Pixel::NORMAL;
	float fBlendFactor = 1.0f;
	cla::int2 vScreenSize = { 256, 240 };
	cla::float2 vInvScreenSize = { 1.0f / 256.0f, 1.0f / 240.0f };
	cla::int2 vPixelSize = { 4, 4 };
	cla::int2 vScreenPixelSize = { 4, 4 };
	cla::int2 vMousePos = { 0, 0 };
	std::int32_t nMouseWheelDelta = 0;
	cla::int2 vMousePosCache = { 0, 0 };
	cla::int2 vMouseWindowPos = { 0, 0 };
	std::int32_t nMouseWheelDeltaCache = 0;
	cla::int2 vWindowSize = { 0, 0 };
	cla::int2 vViewPos = { 0, 0 };
	cla::int2 vViewSize = { 0,0 };
	cla::float2 vPixel = { 1.0f, 1.0f };
	bool bHasInputFocus = false, bHasMouseFocus = false, bEnableVSYNC = false, bPixelCohesion = false, bFullScreen = false;
	float fFrameTimer = 1.0f, fLastElapsed = 0.0f;
	int nFrameCount = 0;
	Sprite* fontSprite = nullptr;
	Decal* fontDecal = nullptr;
	Sprite* pDefaultDrawTarget = nullptr;
	std::vector<LayerDesc> vLayers;
	std::uint8_t nTargetLayer = 0;
	std::uint32_t nLastFPS = 0;
	std::function<core::Pixel(const int x, const int y, const core::Pixel&, const core::Pixel&)> funcPixelMode;
	std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;
	HWND core_hWnd = nullptr;

	bool pKeyNewState[256] = { 0 };
	bool pKeyOldState[256] = { 0 };
	HWButton pKeyboardState[256] = { 0 };

	bool pMouseNewState[nMouseButtons] = { 0 };
	bool pMouseOldState[nMouseButtons] = { 0 };
	HWButton pMouseState[nMouseButtons] = { 0 };

	std::atomic<bool> bAtomActive;

	LRESULT CALLBACK core_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void EngineThread();
	void core_UpdateMouse(std::int32_t x, std::int32_t y);
	void core_UpdateMouseWheel(std::int32_t delta);
	void core_UpdateWindowSize(std::int32_t x, std::int32_t y);
	void core_UpdateViewport();
	void core_ConstructFontSheet();
	void core_CoreUpdate();
	void core_PrepareEngine();
	void core_UpdateMouseState(std::int32_t button, bool state);
	void core_UpdateKeyState(std::int32_t key, bool state);
	void core_UpdateMouseFocus(bool state);
	void core_UpdateKeyFocus(bool state);
	void core_Terminate();
	void core_SetMousePos(std::int32_t x, std::int32_t y) noexcept;
	cla::int2 core_nextMousePos;
	bool core_hideCursor = false;

	core::rcode LoadImageResource(core::Sprite* spr, const std::string& sImageFile);
}

typedef bool(WINAPI wglSwapInterval_t) (int interval);
static wglSwapInterval_t* wglSwapInterval = nullptr;

using glDeviceContext_t = HDC;
using glRenderContext_t = HGLRC;

namespace core
{
	glDeviceContext_t glDeviceContext = 0;
	glRenderContext_t glRenderContext = 0;

	bool bSync = false;

	core::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC)
	{
		glDeviceContext = GetDC((HWND)(params[0]));
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = 0;
		if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd))) return core::FAIL;
		SetPixelFormat(glDeviceContext, pf, &pfd);

		if (!(glRenderContext = wglCreateContext(glDeviceContext))) return core::FAIL;
		wglMakeCurrent(glDeviceContext, glRenderContext);

		wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
		if (wglSwapInterval && !bVSYNC) wglSwapInterval(0);
		bSync = bVSYNC;

		glEnable(GL_TEXTURE_2D);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		return core::rcode::OK;
	}

	core::rcode DestroyDevice()
	{
		wglDeleteContext(glRenderContext);

		return core::rcode::OK;
	}

	void DisplayFrame()
	{
		SwapBuffers(glDeviceContext);
		if (bSync) DwmFlush();
	}

	void PrepareDrawing()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void DrawLayerQuad(const cla::float2& offset, const cla::float2& scale, const core::Pixel tint)
	{
		glBegin(GL_QUADS);
		glColor4ub(tint.r, tint.g, tint.b, tint.a);
		glTexCoord2f(0.0f * std::get<0>(scale) + std::get<0>(offset), 1.0f * std::get<1>(scale) + std::get<1>(offset));
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(0.0f * std::get<0>(scale) + std::get<0>(offset), 0.0f * std::get<1>(scale) + std::get<1>(offset));
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * std::get<0>(scale) + std::get<0>(offset), 0.0f * std::get<1>(scale) + std::get<1>(offset));
		glVertex3f(1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * std::get<0>(scale) + std::get<0>(offset), 1.0f * std::get<1>(scale) + std::get<1>(offset));
		glVertex3f(1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glEnd();
	}

	void DrawDecalQuad(const core::DecalInstance& decal)
	{
		if (decal.decal == nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glBegin(GL_QUADS);
			glColor4ub(decal.tint[0].r, decal.tint[0].g, decal.tint[0].b, decal.tint[0].a);
			glTexCoord4f(std::get<0>(decal.uv[0]), std::get<1>(decal.uv[0]), 0.0f, decal.w[0]); glVertex2f(std::get<0>(decal.pos[0]), std::get<1>(decal.pos[0]));
			glColor4ub(decal.tint[1].r, decal.tint[1].g, decal.tint[1].b, decal.tint[1].a);
			glTexCoord4f(std::get<0>(decal.uv[1]), std::get<1>(decal.uv[1]), 0.0f, decal.w[1]); glVertex2f(std::get<0>(decal.pos[1]), std::get<1>(decal.pos[1]));
			glColor4ub(decal.tint[2].r, decal.tint[2].g, decal.tint[2].b, decal.tint[2].a);
			glTexCoord4f(std::get<0>(decal.uv[2]), std::get<1>(decal.uv[2]), 0.0f, decal.w[2]); glVertex2f(std::get<0>(decal.pos[2]), std::get<1>(decal.pos[2]));
			glColor4ub(decal.tint[3].r, decal.tint[3].g, decal.tint[3].b, decal.tint[3].a);
			glTexCoord4f(std::get<0>(decal.uv[3]), std::get<1>(decal.uv[3]), 0.0f, decal.w[3]); glVertex2f(std::get<0>(decal.pos[3]), std::get<1>(decal.pos[3]));
			glEnd();
		}

		else
		{
			glBindTexture(GL_TEXTURE_2D, decal.decal->id);
			glBegin(GL_QUADS);
			glColor4ub(decal.tint[0].r, decal.tint[0].g, decal.tint[0].b, decal.tint[0].a);
			glTexCoord4f(std::get<0>(decal.uv[0]), std::get<1>(decal.uv[0]), 0.0f, decal.w[0]); glVertex2f(std::get<0>(decal.pos[0]), std::get<1>(decal.pos[0]));
			glTexCoord4f(std::get<0>(decal.uv[1]), std::get<1>(decal.uv[1]), 0.0f, decal.w[1]); glVertex2f(std::get<0>(decal.pos[1]), std::get<1>(decal.pos[1]));
			glTexCoord4f(std::get<0>(decal.uv[2]), std::get<1>(decal.uv[2]), 0.0f, decal.w[2]); glVertex2f(std::get<0>(decal.pos[2]), std::get<1>(decal.pos[2]));
			glTexCoord4f(std::get<0>(decal.uv[3]), std::get<1>(decal.uv[3]), 0.0f, decal.w[3]); glVertex2f(std::get<0>(decal.pos[3]), std::get<1>(decal.pos[3]));
			glEnd();
		}
	}

	std::uint32_t CreateTexture(const std::uint32_t width, const std::uint32_t height)
	{
		static_cast<void>(width);
		static_cast<void>(height);
		std::uint32_t id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		return id;
	}

	std::uint32_t DeleteTexture(const std::uint32_t id)
	{
		glDeleteTextures(1, &id);
		return id;
	}

	void UpdateTexture(std::uint32_t id, core::Sprite* spr)
	{
		static_cast<void>(id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->width, spr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
	}

	void ApplyTexture(std::uint32_t id)
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void ClearBuffer(core::Pixel p, bool bDepth)
	{
		glClearColor(float(p.r) / 255.0f, float(p.g) / 255.0f, float(p.b) / 255.0f, float(p.a) / 255.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (bDepth) glClear(GL_DEPTH_BUFFER_BIT);
	}

	void UpdateViewport(const cla::int2& pos, const cla::int2& size)
	{
		glViewport(std::get<0>(pos), std::get<1>(pos), std::get<0>(size), std::get<1>(size));
	}

	core::rcode LoadImageResource(core::Sprite* spr, const std::string& sImageFile)
	{
		if (!std::filesystem::exists(sImageFile)) return core::rcode::NO_FILE;

		if (spr->pColData != nullptr) delete[] spr->pColData;

		stbi_uc* bytes = nullptr;
		int w = 0, h = 0, cmp = 0;

		bytes = stbi_load(sImageFile.c_str(), &w, &h, &cmp, 4);

		if (!bytes) return core::rcode::FAIL;
		spr->width = w; spr->height = h;
		spr->pColData = new Pixel[spr->width * spr->height];
		std::memcpy(spr->pColData, bytes, spr->width * spr->height * 4);
		delete[] bytes;
		return core::rcode::OK;
	}

	std::wstring ConvertS2W(std::string s)
	{
		int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t* buffer = new wchar_t[count];
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);

		std::wstring w(buffer);
		delete[] buffer;
		return w;
	}


	core::rcode ThreadCleanUp()
	{
		DestroyDevice();

		PostMessageW(core_hWnd, WM_DESTROY, 0, 0);
		return core::OK;
	}

	core::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const cla::int2& vViewPos, const cla::int2& vViewSize)
	{
		if (CreateDevice({ core_hWnd }, bFullScreen, bEnableVSYNC) == core::rcode::OK)
		{
			UpdateViewport(vViewPos, vViewSize);
			return core::rcode::OK;
		}

		else return core::rcode::FAIL;
	}

	core::rcode CreateWindowPane(const cla::int2& vWindowPos, cla::int2& vWindowSize, bool bFullScreen)
	{
		WNDCLASS wc;
		wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandleW(nullptr);
		wc.lpfnWndProc = core_WindowEvent;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = L"core_PIXEL_GAME_ENGINE";
		RegisterClassW(&wc);

		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;

		cla::int2 vTopLeft = vWindowPos;

		if (bFullScreen)
		{
			dwExStyle = 0;
			dwStyle = WS_VISIBLE | WS_POPUP;
			HMONITOR hmon = MonitorFromWindow(core_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			if (!GetMonitorInfoW(hmon, &mi)) return core::rcode::FAIL;
			vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
			std::get<0>(vTopLeft) = 0;
			std::get<1>(vTopLeft) = 0;
		}

		RECT rWndRect = { 0, 0, std::get<0>(vWindowSize), std::get<1>(vWindowSize) };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		core_hWnd = CreateWindowExW(dwExStyle, L"core_PIXEL_GAME_ENGINE", L"OpenGL Game Engine", dwStyle,
			std::get<0>(vTopLeft), std::get<1>(vTopLeft), width, height, NULL, NULL, GetModuleHandleW(nullptr), NULL);

		mapKeys[0x00] = Key::NONE;
		mapKeys[0x41] = Key::A; mapKeys[0x42] = Key::B; mapKeys[0x43] = Key::C; mapKeys[0x44] = Key::D; mapKeys[0x45] = Key::E;
		mapKeys[0x46] = Key::F; mapKeys[0x47] = Key::G; mapKeys[0x48] = Key::H; mapKeys[0x49] = Key::I; mapKeys[0x4A] = Key::J;
		mapKeys[0x4B] = Key::K; mapKeys[0x4C] = Key::L; mapKeys[0x4D] = Key::M; mapKeys[0x4E] = Key::N; mapKeys[0x4F] = Key::O;
		mapKeys[0x50] = Key::P; mapKeys[0x51] = Key::Q; mapKeys[0x52] = Key::R; mapKeys[0x53] = Key::S; mapKeys[0x54] = Key::T;
		mapKeys[0x55] = Key::U; mapKeys[0x56] = Key::V; mapKeys[0x57] = Key::W; mapKeys[0x58] = Key::X; mapKeys[0x59] = Key::Y;
		mapKeys[0x5A] = Key::Z;

		mapKeys[VK_F1] = Key::F1; mapKeys[VK_F2] = Key::F2; mapKeys[VK_F3] = Key::F3; mapKeys[VK_F4] = Key::F4;
		mapKeys[VK_F5] = Key::F5; mapKeys[VK_F6] = Key::F6; mapKeys[VK_F7] = Key::F7; mapKeys[VK_F8] = Key::F8;
		mapKeys[VK_F9] = Key::F9; mapKeys[VK_F10] = Key::F10; mapKeys[VK_F11] = Key::F11; mapKeys[VK_F12] = Key::F12;

		mapKeys[VK_DOWN] = Key::DOWN; mapKeys[VK_LEFT] = Key::LEFT; mapKeys[VK_RIGHT] = Key::RIGHT; mapKeys[VK_UP] = Key::UP;
		mapKeys[VK_RETURN] = Key::ENTER;

		mapKeys[VK_BACK] = Key::BACK; mapKeys[VK_ESCAPE] = Key::ESCAPE; mapKeys[VK_RETURN] = Key::ENTER; mapKeys[VK_PAUSE] = Key::PAUSE;
		mapKeys[VK_SCROLL] = Key::SCROLL; mapKeys[VK_TAB] = Key::TAB; mapKeys[VK_DELETE] = Key::DEL; mapKeys[VK_HOME] = Key::HOME;
		mapKeys[VK_END] = Key::END; mapKeys[VK_PRIOR] = Key::PGUP; mapKeys[VK_NEXT] = Key::PGDN; mapKeys[VK_INSERT] = Key::INS;
		mapKeys[VK_SHIFT] = Key::SHIFT; mapKeys[VK_CONTROL] = Key::CTRL;
		mapKeys[VK_SPACE] = Key::SPACE;

		mapKeys[0x30] = Key::K0; mapKeys[0x31] = Key::K1; mapKeys[0x32] = Key::K2; mapKeys[0x33] = Key::K3; mapKeys[0x34] = Key::K4;
		mapKeys[0x35] = Key::K5; mapKeys[0x36] = Key::K6; mapKeys[0x37] = Key::K7; mapKeys[0x38] = Key::K8; mapKeys[0x39] = Key::K9;

		mapKeys[VK_NUMPAD0] = Key::NP0; mapKeys[VK_NUMPAD1] = Key::NP1; mapKeys[VK_NUMPAD2] = Key::NP2; mapKeys[VK_NUMPAD3] = Key::NP3; mapKeys[VK_NUMPAD4] = Key::NP4;
		mapKeys[VK_NUMPAD5] = Key::NP5; mapKeys[VK_NUMPAD6] = Key::NP6; mapKeys[VK_NUMPAD7] = Key::NP7; mapKeys[VK_NUMPAD8] = Key::NP8; mapKeys[VK_NUMPAD9] = Key::NP9;
		mapKeys[VK_MULTIPLY] = Key::NP_MUL; mapKeys[VK_ADD] = Key::NP_ADD; mapKeys[VK_DIVIDE] = Key::NP_DIV; mapKeys[VK_SUBTRACT] = Key::NP_SUB; mapKeys[VK_DECIMAL] = Key::NP_DECIMAL;

		return core::OK;
	}

	core::rcode SetWindowTitle(const std::string& s)
	{
		SetWindowTextW(core_hWnd, ConvertS2W(s).c_str());

		return core::OK;
	}

	core::rcode StartSystemEventLoop()
	{
		MSG msg;

		while (GetMessageW(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return core::OK;
	}

	core::rcode HandleSystemEvent() { return core::rcode::FAIL; }

	LRESULT CALLBACK core_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_MOUSEMOVE:
			{
				std::uint16_t x = lParam & 0xFFFF; std::uint16_t y = (lParam >> 16) & 0xFFFF;
				std::int16_t ix = *(std::int16_t*) & x;   std::int16_t iy = *(std::int16_t*) & y;
				core_UpdateMouse(ix, iy);
				return 0;
			}

			case WM_SIZE:       core_UpdateWindowSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF); return 0;
			case WM_MOUSEWHEEL:	core_UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));           return 0;
			case WM_MOUSELEAVE: core_UpdateMouseFocus(false);                                    return 0;
			case WM_SETFOCUS:	core_UpdateKeyFocus(true);                                       return 0;
			case WM_KILLFOCUS:	core_UpdateKeyFocus(false);                                      return 0;
			case WM_KEYDOWN:	core_UpdateKeyState(mapKeys[wParam], true);                      return 0;
			case WM_KEYUP:		core_UpdateKeyState(mapKeys[wParam], false);                     return 0;
			case WM_LBUTTONDOWN:core_UpdateMouseState(0, true);                                  return 0;
			case WM_LBUTTONUP:	core_UpdateMouseState(0, false);                                 return 0;
			case WM_RBUTTONDOWN:core_UpdateMouseState(1, true);                                  return 0;
			case WM_RBUTTONUP:	core_UpdateMouseState(1, false);                                 return 0;
			case WM_MBUTTONDOWN:core_UpdateMouseState(2, true);                                  return 0;
			case WM_MBUTTONUP:	core_UpdateMouseState(2, false);                                 return 0;
			case WM_CLOSE:		core_Terminate();                                                return 0;
			case WM_DESTROY:	PostQuitMessage(0); DestroyWindow(hWnd);                             return 0;
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	Sprite::Sprite(const std::string& sImageFile)
	{
		LoadImageResource(this, sImageFile);
	}

	Decal::Decal(core::Sprite* spr)
	{
		id = -1;
		if (spr == nullptr) return;
		sprite = spr;
		id = CreateTexture(sprite->width, sprite->height);
		Update();
	}

	void Decal::Update()
	{
		if (sprite == nullptr) return;
		vUVScale = { 1.0f / float(sprite->width), 1.0f / float(sprite->height) };
		ApplyTexture(id);
		UpdateTexture(id, sprite);
	}

	Decal::~Decal()
	{
		if (id != -1)
		{
			DeleteTexture(id);
			id = -1;
		}
	}

	void Renderable::Create(std::uint32_t width, std::uint32_t height)
	{
		pSprite = std::make_unique<core::Sprite>(width, height);
		pDecal = std::make_unique<core::Decal>(pSprite.get());
	}
	
	core::rcode Renderable::Load(const std::string& sFile)
	{
		pSprite = std::make_unique<core::Sprite>(sFile);
		pDecal = std::make_unique<core::Decal>(pSprite.get());
		
		return core::rcode::OK;
	}
	
	core::Decal* Renderable::Decal() const
	{
		return pDecal.get();
	}
	
	core::Sprite* Renderable::Sprite() const
	{
		return pSprite.get();
	}


	core::rcode Construct(std::int32_t screen_w, std::int32_t screen_h, std::int32_t pixel_w, std::int32_t pixel_h, bool full_screen, bool vsync, bool cohesion)
	{
		bPixelCohesion = cohesion;
		vScreenSize = { screen_w, screen_h };
		vInvScreenSize = { 1.0f / float(screen_w), 1.0f / float(screen_h) };
		vPixelSize = { pixel_w, pixel_h };
		vWindowSize = cla::reduce<std::multiplies<>>(vScreenSize, vPixelSize);
		bFullScreen = full_screen;
		bEnableVSYNC = vsync;
		std::get<0>(vPixel) = 2.0f / static_cast<float>(std::get<0>(vScreenSize));
		std::get<1>(vPixel) = 2.0f / static_cast<float>(std::get<1>(vScreenSize));

		if (std::get<0>(vPixelSize) <= 0 || std::get<1>(vPixelSize) <= 0 || std::get<0>(vScreenSize) <= 0 || std::get<1>(vScreenSize) <= 0) return core::FAIL;

		return core::rcode::OK;
	}



	void SetScreenSize(int w, int h)
	{
		vScreenSize = { w, h };
		vInvScreenSize = { 1.0f / float(w), 1.0f / float(h) };
		for (auto& layer : vLayers)
		{
			delete layer.pDrawTarget;
			layer.pDrawTarget = new Sprite(std::get<0>(vScreenSize), std::get<1>(vScreenSize));
			layer.bUpdate = true;
		}

		SetDrawTarget(nullptr);

		ClearBuffer(core::BLACK, true);
		DisplayFrame();
		ClearBuffer(core::BLACK, true);
		UpdateViewport(vViewPos, vViewSize);
	}

	core::rcode Start()
	{
		if (CreateWindowPane({ 30,30 }, vWindowSize, bFullScreen) != core::OK) return core::FAIL;
		core_UpdateWindowSize(std::get<0>(vWindowSize), std::get<1>(vWindowSize));

		bAtomActive = true;
		//test this later
		std::thread t = std::thread(&EngineThread);

		StartSystemEventLoop();

		t.join();

		return core::OK;
	}



	void SetDrawTarget(Sprite* target) noexcept
	{
		if (target)
		{
			pDrawTarget = target;
		}

		else
		{
			nTargetLayer = 0;
			pDrawTarget = vLayers[0].pDrawTarget;
		}
	}
	
	void SetDrawTarget(std::uint8_t layer) noexcept
	{
		if (layer < vLayers.size())
		{
			pDrawTarget = vLayers[layer].pDrawTarget;
			vLayers[layer].bUpdate = true;
			nTargetLayer = layer;
		}
	}
	
	void SetLayerOffset(std::uint8_t layer, const cla::float2& offset) noexcept
	{
		SetLayerOffset(layer, std::get<0>(offset), std::get<1>(offset));
	}
	
	void SetLayerOffset(std::uint8_t layer, float x, float y) noexcept
	{
		if (layer < vLayers.size()) vLayers[layer].vOffset = { x, y };
	}
	
	void SetLayerScale(std::uint8_t layer, const cla::float2& scale) noexcept
	{
		SetLayerScale(layer, std::get<0>(scale), std::get<1>(scale));
	}
	
	void SetLayerScale(std::uint8_t layer, float x, float y) noexcept
	{
		if (layer < vLayers.size()) vLayers[layer].vScale = { x, y };
	}
	
	void SetLayerTint(std::uint8_t layer, const core::Pixel& tint) noexcept
	{
		if (layer < vLayers.size()) vLayers[layer].tint = tint;
	}
	
	void SetLayerCustomRenderFunction(std::uint8_t layer, std::function<void()> f) noexcept
	{
		if (layer < vLayers.size()) vLayers[layer].funcHook = f;
	}
	
	std::vector<LayerDesc>& GetLayers() noexcept
	{
		return vLayers;
	}
	
	std::uint32_t CreateLayer()
	{
		LayerDesc ld;
		ld.pDrawTarget = new core::Sprite(std::get<0>(vScreenSize), std::get<1>(vScreenSize));
		ld.nResID = CreateTexture(std::get<0>(vScreenSize), std::get<1>(vScreenSize));
		UpdateTexture(ld.nResID, ld.pDrawTarget);
		vLayers.push_back(ld);
		return std::uint32_t(vLayers.size()) - 1;
	}
	
	Sprite* GetDrawTarget() noexcept
	{
		return pDrawTarget;
	}
	
	std::int32_t GetDrawTargetWidth() noexcept
	{
		if (pDrawTarget) return pDrawTarget->width;
		else return 0;
	}
	
	std::int32_t GetDrawTargetHeight() noexcept
	{
		if (pDrawTarget) return pDrawTarget->height;
		else return 0;
	}
	
	std::uint32_t GetFPS() noexcept
	{
		return nLastFPS;
	}
	
	bool IsFocused() noexcept
	{
		return bHasInputFocus;
	}
	
	HWButton GetKey(Key k) noexcept
	{
		return pKeyboardState[k];
	}
	
	HWButton GetMouse(std::uint32_t b) noexcept
	{
		return pMouseState[b];
	}
	
	std::int32_t GetMouseX() noexcept
	{
		return std::get<0>(vMousePos);
	}
	
	std::int32_t GetMouseY() noexcept
	{
		return std::get<1>(vMousePos);
	}
	
	const cla::int2& GetMousePos() noexcept
	{
		return vMousePos;
	}
	
	std::int32_t GetMouseWheel() noexcept
	{
		return nMouseWheelDelta;
	}
	
	std::int32_t ScreenWidth() noexcept
	{
		return std::get<0>(vScreenSize);
	}
	
	std::int32_t ScreenHeight() noexcept
	{
		return std::get<1>(vScreenSize);
	}
	
	float GetElapsedTime() noexcept
	{
		return fLastElapsed;
	}
	
	const cla::int2& GetWindowSize() noexcept
	{
		return vWindowSize;
	}
	
	const cla::int2& GetPixelSize() noexcept
	{
		return vPixelSize;
	}
	
	const cla::int2& GetScreenPixelSize() noexcept
	{
		return vScreenPixelSize;
	}
	
	const cla::int2& GetWindowMouse() noexcept
	{
		return vMouseWindowPos;
	}
	
	void SetMousePos(cla::int2&& pos) noexcept
	{
		core_SetMousePos(std::get<0>(pos), std::get<1>(pos));
	}
	
	void Clear(Pixel p)
	{
		int pixels = GetDrawTargetWidth() * GetDrawTargetHeight();
		Pixel* m = GetDrawTarget()->GetData();
		for (int i = 0; i < pixels; i++) m[i] = p;
	}

	void DrawPartialDecal(const cla::float2& pos, core::Decal* decal, const cla::float2& source_pos, const cla::float2& source_size, const cla::float2& scale, const core::Pixel& tint)
	{
		cla::float2 vScreenSpacePos =
		{
			(std::get<0>(pos) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f,
			((std::get<1>(pos) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f
		};

		cla::float2 vScreenSpaceDim =
		{
			std::get<0>(vScreenSpacePos) + (2.0f * std::get<0>(source_size) * std::get<0>(vInvScreenSize)) * std::get<0>(scale),
			std::get<1>(vScreenSpacePos) - (2.0f * std::get<1>(source_size) * std::get<1>(vInvScreenSize)) * std::get<1>(scale)
		};

		DecalInstance di; di.decal = decal;	di.tint[0] = tint;

		di.pos[0] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpacePos) };
		di.pos[1] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpaceDim) };
		di.pos[2] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpaceDim) };
		di.pos[3] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpacePos) };

		cla::float2 uvtl = cla::reduce<std::multiplies<>>(source_pos, decal->vUVScale);
		cla::float2 uvbr = cla::reduce<std::plus<>>(uvtl, cla::reduce<std::multiplies<>>(source_size, decal->vUVScale));
		di.uv[0] = { std::get<0>(uvtl), std::get<1>(uvtl) }; di.uv[1] = { std::get<0>(uvtl), std::get<1>(uvbr) };
		di.uv[2] = { std::get<0>(uvbr), std::get<1>(uvbr) }; di.uv[3] = { std::get<0>(uvbr), std::get<1>(uvtl) };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawPartialDecal(const cla::float2& pos, const cla::float2& size, core::Decal* decal, const cla::float2& source_pos, const cla::float2& source_size, const core::Pixel& tint)
	{
		cla::float2 vScreenSpacePos =
		{
			(std::get<0>(pos) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f,
			((std::get<1>(pos) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f
		};

		cla::float2 vScreenSpaceDim =
		{
			std::get<0>(vScreenSpacePos) + (2.0f * std::get<0>(size) * std::get<0>(vInvScreenSize)),
			std::get<1>(vScreenSpacePos) - (2.0f * std::get<1>(size) * std::get<1>(vInvScreenSize))
		};

		DecalInstance di; di.decal = decal;	di.tint[0] = tint;

		di.pos[0] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpacePos) };
		di.pos[1] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpaceDim) };
		di.pos[2] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpaceDim) };
		di.pos[3] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpacePos) };

		cla::float2 uvtl = cla::reduce<std::multiplies<>>(source_pos, decal->vUVScale);
		cla::float2 uvbr = cla::reduce<std::plus<>>(uvtl, cla::reduce<std::multiplies<>>(source_size, decal->vUVScale));
		di.uv[0] = { std::get<0>(uvtl), std::get<1>(uvtl) }; di.uv[1] = { std::get<0>(uvtl), std::get<1>(uvbr) };
		di.uv[2] = { std::get<0>(uvbr), std::get<1>(uvbr) }; di.uv[3] = { std::get<0>(uvbr), std::get<1>(uvtl) };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawDecal(const cla::float2& pos, core::Decal* decal, const cla::float2& scale, const core::Pixel& tint)
	{
		cla::float2 vScreenSpacePos =
		{
			(std::get<0>(pos) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f,
			((std::get<1>(pos) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f
		};

		cla::float2 vScreenSpaceDim =
		{
			std::get<0>(vScreenSpacePos) + (2.0f * (float(decal->sprite->width) * std::get<0>(vInvScreenSize))) * std::get<0>(scale),
			std::get<1>(vScreenSpacePos) - (2.0f * (float(decal->sprite->height) * std::get<1>(vInvScreenSize))) * std::get<1>(scale)
		};

		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		di.pos[0] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpacePos) };
		di.pos[1] = { std::get<0>(vScreenSpacePos), std::get<1>(vScreenSpaceDim) };
		di.pos[2] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpaceDim) };
		di.pos[3] = { std::get<0>(vScreenSpaceDim), std::get<1>(vScreenSpacePos) };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawRotatedDecal(const cla::float2& pos, core::Decal* decal, const float fAngle, const cla::float2& center, const cla::float2& scale, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;

		di.pos[0] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(0.0f, 0.0f), center), scale);
		di.pos[1] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(0.0f, float(decal->sprite->height)), center), scale);
		di.pos[2] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(float(decal->sprite->width), float(decal->sprite->height)), center), scale);
		di.pos[3] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(float(decal->sprite->width), 0.0f), center), scale);

		float c = std::cosf(fAngle), s = std::sinf(fAngle);

		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = cla::reduce<std::plus<>>(pos, std::make_tuple(std::get<0>(di.pos[i]) * c - std::get<1>(di.pos[i]) * s, std::get<0>(di.pos[i]) * s + std::get<1>(di.pos[i]) * c));
			di.pos[i] = cla::reduce<std::minus<>>(cla::apply<std::multiplies<>, float>(cla::reduce<std::multiplies<>>(di.pos[i], vInvScreenSize), 2.0f), std::make_tuple(1.0f, 1.0f));
			std::get<1>(di.pos[i]) *= -1.0f;
		}

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawExplicitDecal(core::Decal* decal, const cla::float2* pos, const cla::float2* uv, const core::Pixel* col)
	{
		DecalInstance di;
		di.decal = decal;

		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = { (std::get<0>(pos[i]) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f, ((std::get<1>(pos[i]) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = col[i];
		}

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void FillRectDecal(const cla::float2& pos, const cla::float2& size, const core::Pixel col)
	{
		std::array<cla::float2, 4> points = { { { pos }, { std::get<0>(pos), std::get<1>(pos) + std::get<1>(size) }, { cla::reduce<std::plus<>>(pos, size) }, { std::get<0>(pos) + std::get<0>(size), std::get<1>(pos) } } };
		std::array<cla::float2, 4> uvs = { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
		std::array<core::Pixel, 4> cols = { { col, col, col, col } };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data());
	}

	void GradientFillRectDecal(const cla::float2& pos, const cla::float2& size, const core::Pixel&& colTL, const core::Pixel&& colBL, const core::Pixel&& colBR, const core::Pixel&& colTR)
	{
		std::array<cla::float2, 4> points = { { { pos }, { std::get<0>(pos), std::get<1>(pos) + std::get<1>(size) }, { cla::reduce<std::plus<>>(pos, size) }, { std::get<0>(pos) + std::get<0>(size), std::get<1>(pos) } } };
		std::array<cla::float2, 4> uvs = { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
		std::array<core::Pixel, 4> cols = { { colTL, colBL, colBR, colTR } };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data());
	}

	void DrawPartialRotatedDecal(const cla::float2& pos, core::Decal* decal, const float fAngle, const cla::float2& center, const cla::float2& source_pos, const cla::float2& source_size, const cla::float2& scale, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		di.pos[0] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(0.0f, 0.0f), center), scale);
		di.pos[1] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(0.0f, std::get<1>(source_size)), center), scale);
		di.pos[2] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(std::get<0>(source_size), std::get<1>(source_size)), center), scale);
		di.pos[3] = cla::reduce<std::multiplies<>>(cla::reduce<std::minus<>>(std::make_tuple(std::get<0>(source_size), 0.0f), center), scale);
		float c = std::cosf(fAngle), s = std::sinf(fAngle);

		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = cla::reduce<std::plus<>>(pos, std::make_tuple(std::get<0>(di.pos[i]) * c - std::get<1>(di.pos[i]) * s, std::get<0>(di.pos[i]) * s + std::get<1>(di.pos[i]) * c));
			di.pos[i] = cla::reduce<std::minus<>>(cla::apply<std::multiplies<>, float>(cla::reduce<std::multiplies<>>(di.pos[i], vInvScreenSize), 2.0f), std::make_tuple(1.0f, 1.0f));
			std::get<1>(di.pos[i]) *= -1.0f;
		}

		cla::float2 uvtl = cla::reduce<std::multiplies<>>(source_pos, decal->vUVScale);
		cla::float2 uvbr = cla::reduce<std::plus<>>(uvtl, cla::reduce<std::multiplies<>>(source_size, decal->vUVScale));
		di.uv[0] = { std::get<0>(uvtl), std::get<1>(uvtl) }; di.uv[1] = { std::get<0>(uvtl), std::get<1>(uvbr) };
		di.uv[2] = { std::get<0>(uvbr), std::get<1>(uvbr) }; di.uv[3] = { std::get<0>(uvbr), std::get<1>(uvtl) };

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawWarpedDecal(core::Decal* decal, const std::array<cla::float2, 4>& pos, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		cla::float2 center;
		float rd = ((std::get<0>(pos[2]) - std::get<0>(pos[0])) * (std::get<1>(pos[3]) - std::get<1>(pos[1])) - (std::get<0>(pos[3]) - std::get<0>(pos[1])) * (std::get<1>(pos[2]) - std::get<1>(pos[0])));

		if (rd != 0)
		{
			rd = 1.0f / rd;
			float rn = ((std::get<0>(pos[3]) - std::get<0>(pos[1])) * (std::get<1>(pos[0]) - std::get<1>(pos[1])) - (std::get<1>(pos[3]) - std::get<1>(pos[1])) * (std::get<0>(pos[0]) - std::get<0>(pos[1]))) * rd;
			float sn = ((std::get<0>(pos[2]) - std::get<0>(pos[0])) * (std::get<1>(pos[0]) - std::get<1>(pos[1])) - (std::get<1>(pos[2]) - std::get<1>(pos[0])) * (std::get<0>(pos[0]) - std::get<0>(pos[1]))) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = cla::reduce<std::multiplies<>>(cla::apply<std::plus<>, float>(pos[0], rn), cla::reduce<std::minus<>>(pos[2], pos[0]));
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = cla::length(cla::reduce<std::minus<>>(pos[i], center));

			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				//di.uv[i] *= q; di.w[i] *= q;
				di.uv[i] = cla::apply<std::multiplies<>, float, float, float>(di.uv[i], q);
				di.w[i] = di.w[i] * q;

				di.pos[i] = { (std::get<0>(pos[i]) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f, ((std::get<1>(pos[i]) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f };
			}

			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void DrawPartialWarpedDecal(core::Decal* decal, const std::array<cla::float2, 4>& pos, const cla::float2& source_pos, const cla::float2& source_size, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		cla::float2 center;

		float rd = ((std::get<0>(pos[2]) - std::get<0>(pos[0])) * (std::get<1>(pos[3]) - std::get<1>(pos[1])) - (std::get<0>(pos[3]) - std::get<0>(pos[1])) * (std::get<1>(pos[2]) - std::get<1>(pos[0])));

		if (rd != 0)
		{
			cla::float2 uvtl = cla::reduce<std::multiplies<>>(source_pos, decal->vUVScale);
			cla::float2 uvbr = cla::reduce<std::plus<>>(uvtl, cla::reduce<std::multiplies<>>(source_size, decal->vUVScale));
			di.uv[0] = { std::get<0>(uvtl), std::get<1>(uvtl) }; di.uv[1] = { std::get<0>(uvtl), std::get<1>(uvbr) };
			di.uv[2] = { std::get<0>(uvbr), std::get<1>(uvbr) }; di.uv[3] = { std::get<0>(uvbr), std::get<1>(uvtl) };

			rd = 1.0f / rd;
			float rn = ((std::get<0>(pos[3]) - std::get<0>(pos[1])) * (std::get<1>(pos[0]) - std::get<1>(pos[1])) - (std::get<1>(pos[3]) - std::get<1>(pos[1])) * (std::get<0>(pos[0]) - std::get<0>(pos[1]))) * rd;
			float sn = ((std::get<0>(pos[2]) - std::get<0>(pos[0])) * (std::get<1>(pos[0]) - std::get<1>(pos[1])) - (std::get<1>(pos[2]) - std::get<1>(pos[0])) * (std::get<0>(pos[0]) - std::get<0>(pos[1]))) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = cla::reduce<std::plus<>>(pos[0], cla::apply<std::multiplies<>, float>(cla::reduce<std::minus<>>(pos[2], pos[0]), rn));
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = cla::length(cla::reduce<std::minus<>>(pos[i], center));

			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				//di.uv[i] *= q; di.w[i] *= q;
				di.uv[i] = cla::apply<std::multiplies<>, float>(di.uv[i], q);
				di.w[i] = di.w[i] * q;

				di.pos[i] = { (std::get<0>(pos[i]) * std::get<0>(vInvScreenSize)) * 2.0f - 1.0f, ((std::get<1>(pos[i]) * std::get<1>(vInvScreenSize)) * 2.0f - 1.0f) * -1.0f };
			}

			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void DrawStringDecal(const cla::float2& pos, const std::string& sText, const Pixel col, const cla::float2& scale)
	{
		cla::float2 spos = { 0.0f, 0.0f };

		for (auto& c : sText)
		{
			if (c == '\n')
			{
				std::get<0>(spos) = 0; std::get<1>(spos) += 8.0f * std::get<1>(scale);
			}

			else
			{
				std::int32_t ox = (c - 32) % 16;
				std::int32_t oy = (c - 32) / 16;
				DrawPartialDecal(cla::reduce<std::plus<>>(pos, spos), fontDecal, { float(ox) * 8.0f, float(oy) * 8.0f }, { 8.0f, 8.0f }, scale, col);
				std::get<0>(spos) += 8.0f * std::get<0>(scale);
			}
		}
	}


	cla::int2 GetTextSize(const std::string& s)
	{
		cla::int2 size = { 0,1 };
		cla::int2 pos = { 0,1 };

		for (auto c : s)
		{
			if (c == '\n') { std::get<1>(pos)++; std::get<0>(pos) = 0; }
			else std::get<0>(pos)++;
			std::get<0>(size) = std::max(std::get<0>(size), std::get<0>(pos));
			std::get<1>(size) = std::max(std::get<1>(size), std::get<1>(pos));
		}

		return cla::apply<std::multiplies<>, int>(size, 8);
	}

	void SetPixelMode(Pixel::Mode m)
	{
		nPixelMode = m;
	}

	Pixel::Mode GetPixelMode()
	{
		return nPixelMode;
	}

	void SetPixelMode(std::function<core::Pixel(const int x, const int y, const core::Pixel&, const core::Pixel&)> pixelMode)
	{
		funcPixelMode = pixelMode;
		nPixelMode = Pixel::Mode::CUSTOM;
	}

	void SetPixelBlend(float fBlend)
	{
		fBlendFactor = fBlend;
		if (fBlendFactor < 0.0f) fBlendFactor = 0.0f;
		if (fBlendFactor > 1.0f) fBlendFactor = 1.0f;
	}

	void core_UpdateViewport()
	{
		std::int32_t ww = std::get<0>(vScreenSize) * std::get<0>(vPixelSize);
		std::int32_t wh = std::get<1>(vScreenSize) * std::get<1>(vPixelSize);
		float wasp = static_cast<float>(ww) / static_cast<float>(wh);

		if (bPixelCohesion)
		{
			//vScreenPixelSize = (vWindowSize / vScreenSize); vViewSize = (vWindowSize / vScreenSize) * vScreenSize;
			vScreenPixelSize = cla::reduce<std::divides<>>(vWindowSize, vScreenSize);
			vViewSize = vWindowSize;
		}

		else
		{
			std::get<0>(vViewSize) = static_cast<std::int32_t>(std::get<0>(vWindowSize));
			std::get<1>(vViewSize) = static_cast<std::int32_t>((static_cast<float>(std::get<0>(vViewSize)) / wasp));

			if (std::get<1>(vViewSize) > std::get<1>(vWindowSize))
			{
				std::get<1>(vViewSize) = std::get<1>(vWindowSize);
				std::get<0>(vViewSize) = static_cast<std::int32_t>((static_cast<float>(std::get<1>(vViewSize)) * wasp));
			}
		}

		vViewPos = cla::apply<std::divides<>, int>(cla::reduce<std::minus<>>(vWindowSize, vViewSize), 2);
	}

	void core_UpdateWindowSize(std::int32_t x, std::int32_t y)
	{
		vWindowSize = { x, y };
		core_UpdateViewport();
	}

	void core_UpdateMouseWheel(std::int32_t delta)
	{
		nMouseWheelDeltaCache += delta;
	}

	void core_UpdateMouse(std::int32_t x, std::int32_t y)
	{
		bHasMouseFocus = true;
		vMouseWindowPos = { x, y };

		x -= std::get<0>(vViewPos);
		y -= std::get<1>(vViewPos);
		std::get<0>(vMousePosCache) = (std::int32_t)(((float)x / (float)(std::get<0>(vWindowSize) - (std::get<0>(vViewPos) * 2)) * (float)std::get<0>(vScreenSize)));
		std::get<1>(vMousePosCache) = (std::int32_t)(((float)y / (float)(std::get<1>(vWindowSize) - (std::get<1>(vViewPos) * 2)) * (float)std::get<1>(vScreenSize)));
		if (std::get<0>(vMousePosCache) >= (std::int32_t)std::get<0>(vScreenSize)) std::get<0>(vMousePosCache) = std::get<0>(vScreenSize) - 1;
		if (std::get<1>(vMousePosCache) >= (std::int32_t)std::get<1>(vScreenSize)) std::get<1>(vMousePosCache) = std::get<1>(vScreenSize) - 1;
		if (std::get<0>(vMousePosCache) < 0) std::get<0>(vMousePosCache) = 0;
		if (std::get<1>(vMousePosCache) < 0) std::get<1>(vMousePosCache) = 0;
	}

	void core_UpdateMouseState(std::int32_t button, bool state)
	{
		pMouseNewState[button] = state;
	}

	void core_UpdateKeyState(std::int32_t key, bool state)
	{
		pKeyNewState[key] = state;
	}

	void core_UpdateMouseFocus(bool state)
	{
		bHasMouseFocus = state;
	}

	void core_UpdateKeyFocus(bool state)
	{
		bHasInputFocus = state;
	}

	void core_Terminate()
	{
		bAtomActive = false;
	}

	void core_SetMousePos(std::int32_t x, std::int32_t y) noexcept
	{
		std::get<0>(core_nextMousePos) = x;
		std::get<1>(core_nextMousePos) = y;
	}

	void EngineThread()
	{
		core_PrepareEngine();

		if (!OnUserCreate()) bAtomActive = false;

		while (bAtomActive)
		{
			core_CoreUpdate();

			if (!OnUserDestroy())
			{
				bAtomActive = true;
			}
		}

		ThreadCleanUp();
	}

	void core_PrepareEngine()
	{
		if (CreateGraphics(bFullScreen, bEnableVSYNC, vViewPos, vViewSize) == core::FAIL) return;

		core_ConstructFontSheet();

		CreateLayer();
		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		SetDrawTarget(nullptr);

		m_tp1 = std::chrono::system_clock::now();
		m_tp2 = std::chrono::system_clock::now();
	}

	void core_CoreUpdate()
	{
		m_tp2 = std::chrono::system_clock::now();
		auto elapsedTime = m_tp2 - m_tp1;
		m_tp1 = m_tp2;

		float fElapsedTime = static_cast<float>(elapsedTime.count());
		fLastElapsed = fElapsedTime;

		HandleSystemEvent();

		auto ScanHardware = [&](HWButton* pKeys, bool* pStateOld, bool* pStateNew, std::uint32_t nKeyCount)
		{
			for (std::uint32_t i = 0; i < nKeyCount; i++)
			{
				pKeys[i].bPressed = false;
				pKeys[i].bReleased = false;

				if (pStateNew[i] != pStateOld[i])
				{
					if (pStateNew[i])
					{
						pKeys[i].bPressed = !pKeys[i].bHeld;
						pKeys[i].bHeld = true;
					}

					else
					{
						pKeys[i].bReleased = true;
						pKeys[i].bHeld = false;
					}
				}

				pStateOld[i] = pStateNew[i];
			}
		};

		ScanHardware(pKeyboardState, pKeyOldState, pKeyNewState, 256);
		ScanHardware(pMouseState, pMouseOldState, pMouseNewState, nMouseButtons);

		vMousePos = vMousePosCache;
		nMouseWheelDelta = nMouseWheelDeltaCache;
		nMouseWheelDeltaCache = 0;

		if (!OnUserUpdate(fElapsedTime)) bAtomActive = false;

		UpdateViewport(vViewPos, vViewSize);
		ClearBuffer(core::BLACK, true);

		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		PrepareDrawing();

		for (auto layer = vLayers.rbegin(); layer != vLayers.rend(); ++layer)
		{
			if (layer->bShow)
			{
				if (layer->funcHook == nullptr)
				{
					ApplyTexture(layer->nResID);

					if (layer->bUpdate)
					{
						UpdateTexture(layer->nResID, layer->pDrawTarget);
						layer->bUpdate = false;
					}

					DrawLayerQuad(layer->vOffset, layer->vScale, layer->tint);

					for (auto& decal : layer->vecDecalInstance) DrawDecalQuad(decal);
					layer->vecDecalInstance.clear();
				}

				else
				{
					layer->funcHook();
				}
			}
		}

		DisplayFrame();

		fFrameTimer += fElapsedTime;
		nFrameCount++;

		if (fFrameTimer >= 1.0f)
		{
			nLastFPS = nFrameCount;
			fFrameTimer -= 1.0f;
			nFrameCount = 0;
		}
	}


	void core_ConstructFontSheet()
	{
		std::string data;
		data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
		data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
		data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
		data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
		data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
		data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
		data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
		data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
		data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
		data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
		data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
		data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
		data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
		data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
		data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
		data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

		fontSprite = new core::Sprite(128, 48);
		int px = 0, py = 0;

		for (std::size_t b = 0; b < 1024; b += 4)
		{
			std::uint32_t sym1 = (std::uint32_t)data[b + 0] - 48;
			std::uint32_t sym2 = (std::uint32_t)data[b + 1] - 48;
			std::uint32_t sym3 = (std::uint32_t)data[b + 2] - 48;
			std::uint32_t sym4 = (std::uint32_t)data[b + 3] - 48;
			std::uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++)
			{
				int k = r & (1 << i) ? 255 : 0;
				fontSprite->SetPixel(px, py, core::Pixel(k, k, k, k));
				if (++py == 48) { px++; py = 0; }
			}
		}

		fontDecal = new core::Decal(fontSprite);
	}
};