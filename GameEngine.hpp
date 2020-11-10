#pragma once

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

namespace core
{
	namespace math 
	{
		template<class T>
		constexpr T abs(T arg) noexcept
		{
			if (std::is_constant_evaluated()) return (arg < T{}) ? -arg : arg;
			else return std::abs(arg);
		}

		template<class T>
		constexpr T sqrt(T arg) noexcept
		{
			if (std::is_constant_evaluated())
			{
				T z = T{ 1 };

				for (int i = 0; i < 10; ++i)
				{
					z -= (z * z - arg) / (2 * z);
				}

				return z;
			}

			else return std::sqrt(arg);
		}
	}

	class PixelGameEngine;
	class Sprite;

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


	template <class T>
	struct v2d_generic
	{
		T x = 0, y = 0;

		constexpr v2d_generic() noexcept : x(0), y(0) {}
		constexpr v2d_generic(T _x, T _y) noexcept : x(_x), y(_y) {}
		constexpr v2d_generic(const v2d_generic& v) noexcept = default;
		constexpr v2d_generic& operator=(const v2d_generic& v) noexcept = default;
		constexpr T mag() const noexcept { return T(math::sqrt(x * x + y * y)); }
		constexpr T mag2() const noexcept { return x * x + y * y; }
		constexpr v2d_generic  norm() const noexcept { T r = 1 / mag(); return v2d_generic(x * r, y * r); }
		constexpr v2d_generic  perp() const noexcept { return v2d_generic(-y, x); }
		constexpr T dot(const v2d_generic& rhs) const noexcept { return this->x * rhs.x + this->y * rhs.y; }
		constexpr T cross(const v2d_generic& rhs) const noexcept { return this->x * rhs.y - this->y * rhs.x; }
		constexpr v2d_generic  operator +  (const v2d_generic& rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
		constexpr v2d_generic  operator -  (const v2d_generic& rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
		constexpr v2d_generic  operator *  (const T& rhs)           const { return v2d_generic(this->x * rhs, this->y * rhs); }
		constexpr v2d_generic  operator *  (const v2d_generic& rhs) const { return v2d_generic(this->x * rhs.x, this->y * rhs.y); }
		constexpr v2d_generic  operator /  (const T& rhs)           const { return v2d_generic(this->x / rhs, this->y / rhs); }
		constexpr v2d_generic  operator /  (const v2d_generic& rhs) const { return v2d_generic(this->x / rhs.x, this->y / rhs.y); }
		constexpr v2d_generic& operator += (const v2d_generic& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
		constexpr v2d_generic& operator -= (const v2d_generic& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
		constexpr v2d_generic& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
		constexpr v2d_generic& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
		constexpr v2d_generic  operator +  () const { return { +x, +y }; }
		constexpr v2d_generic  operator -  () const { return { -x, -y }; }

		constexpr auto operator<=>(const v2d_generic& rhs) const noexcept = default;

		constexpr bool operator == (const v2d_generic& rhs) const noexcept { return (this->x == rhs.x && this->y == rhs.y); }
		constexpr bool operator != (const v2d_generic& rhs) const noexcept { return (this->x != rhs.x || this->y != rhs.y); }

		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const v2d_generic& rhs) { os << rhs.str(); return os; }
		constexpr operator v2d_generic<std::int32_t>() const noexcept { return { static_cast<std::int32_t>(this->x), static_cast<std::int32_t>(this->y) }; }
		constexpr operator v2d_generic<float>() const noexcept { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
		constexpr operator v2d_generic<double>() const noexcept { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
	};


	template<class T> constexpr v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs * (float)rhs.x), (T)(lhs * (float)rhs.y));
	}
	template<class T> constexpr v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs * (double)rhs.x), (T)(lhs * (double)rhs.y));
	}
	template<class T> constexpr v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs * (int)rhs.x), (T)(lhs * (int)rhs.y));
	}
	template<class T> constexpr v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs / (float)rhs.x), (T)(lhs / (float)rhs.y));
	}
	template<class T> constexpr v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs / (double)rhs.x), (T)(lhs / (double)rhs.y));
	}
	template<class T> constexpr v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs) noexcept
	{
		return v2d_generic<T>((T)(lhs / (int)rhs.x), (T)(lhs / (int)rhs.y));
	}

	using vi2d = v2d_generic<std::int32_t>;
	using vu2d = v2d_generic<std::uint32_t>;
	using vf2d = v2d_generic<float>;
	using vd2d = v2d_generic<double>;
	using vl2d = v2d_generic<long double>;

	struct HWButton
	{
		bool bPressed = false;
		bool bReleased = false;
		bool bHeld = false;
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
		core::rcode LoadFromFile(const std::string& sImageFile);

	public:
		std::int32_t width = 0;
		std::int32_t height = 0;
		enum Mode { NORMAL, PERIODIC };
		enum Flip { NONE = 0, HORIZ = 1, VERT = 2 };

	public:
		constexpr void SetSampleMode(core::Sprite::Mode mode = core::Sprite::Mode::NORMAL) noexcept { modeSample = mode; }
		constexpr bool  SetPixel(std::int32_t x, std::int32_t y, Pixel p) noexcept;
		constexpr Pixel GetPixel(const core::vi2d& a) const noexcept { return GetPixel(a.x, a.y); }
		constexpr bool  SetPixel(const core::vi2d& a, Pixel p) noexcept { return SetPixel(a.x, a.y, p); }
		constexpr Pixel Sample(float x, float y) const noexcept;
		Pixel SampleBL(float u, float v) const noexcept;
		constexpr const Pixel* GetData() const noexcept { return pColData; }
		constexpr Pixel* GetData() noexcept { return pColData; }
		core::Sprite* Duplicate() const;
		core::Sprite* Duplicate(const core::vi2d& vPos, const core::vi2d& vSize) const;
		constexpr Pixel GetPixel(std::int32_t x, std::int32_t y) const noexcept;
		Pixel* pColData = nullptr;
		core::Sprite::Mode modeSample = core::Sprite::Mode::NORMAL;
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
		core::vf2d vUVScale = { 1.0f, 1.0f };
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
		core::vf2d pos[4] = { { 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
		core::vf2d uv[4] = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		float w[4] = { 1, 1, 1, 1 };
		core::Pixel tint[4] = { core::WHITE, core::WHITE, core::WHITE, core::WHITE };;
	};


	struct LayerDesc
	{
		core::vf2d vOffset = { 0, 0 };
		core::vf2d vScale = { 1, 1 };
		bool bShow = false;
		bool bUpdate = false;
		core::Sprite* pDrawTarget = nullptr;
		std::uint32_t nResID = 0;
		std::vector<DecalInstance> vecDecalInstance;
		core::Pixel tint = core::WHITE;
		std::function<void()> funcHook = nullptr;
	};

	static std::map<std::size_t, std::uint8_t> mapKeys;



	
		
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
	const core::vi2d& GetWindowMouse() noexcept;
	const core::vi2d& GetMousePos() noexcept;

	void SetMousePos(core::vi2d pos) noexcept;

	std::int32_t ScreenWidth() noexcept;
	std::int32_t ScreenHeight() noexcept;
	std::int32_t GetDrawTargetWidth() noexcept;
	std::int32_t GetDrawTargetHeight() noexcept;
	core::Sprite* GetDrawTarget() noexcept;
	void SetScreenSize(int w, int h);
	void SetDrawTarget(Sprite* target) noexcept;
	std::uint32_t GetFPS() noexcept;
	float GetElapsedTime() noexcept;
	const core::vi2d& GetWindowSize() noexcept;
	const core::vi2d& GetPixelSize() noexcept;
	const core::vi2d& GetScreenPixelSize() noexcept;

	void SetDrawTarget(std::uint8_t layer) noexcept;
	void SetLayerOffset(std::uint8_t layer, const core::vf2d& offset) noexcept;
	void SetLayerOffset(std::uint8_t layer, float x, float y) noexcept;
	void SetLayerScale(std::uint8_t layer, const core::vf2d& scale) noexcept;
	void SetLayerScale(std::uint8_t layer, float x, float y) noexcept;
	void SetLayerTint(std::uint8_t layer, const core::Pixel& tint) noexcept;
	void SetLayerCustomRenderFunction(std::uint8_t layer, std::function<void()> f) noexcept;

	std::vector<LayerDesc>& GetLayers()  noexcept;
	std::uint32_t CreateLayer();

	void SetPixelMode(Pixel::Mode m);
	Pixel::Mode GetPixelMode();
	void SetPixelMode(std::function<core::Pixel(const int x, const int y, const core::Pixel& pSource, const core::Pixel& pDest)> pixelMode);
	void SetPixelBlend(float fBlend);

	bool Draw(std::int32_t x, std::int32_t y, Pixel p = core::WHITE);
	bool Draw(const core::vi2d& pos, Pixel p = core::WHITE);
	void DrawLine(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, Pixel p = core::WHITE, std::uint32_t pattern = 0xFFFFFFFF);
	void DrawLine(const core::vi2d& pos1, const core::vi2d& pos2, Pixel p = core::WHITE, std::uint32_t pattern = 0xFFFFFFFF);
	void DrawCircle(std::int32_t x, std::int32_t y, std::int32_t radius, Pixel p = core::WHITE, std::uint8_t mask = 0xFF);
	void DrawCircle(const core::vi2d& pos, std::int32_t radius, Pixel p = core::WHITE, std::uint8_t mask = 0xFF);
	void FillCircle(std::int32_t x, std::int32_t y, std::int32_t radius, Pixel p = core::WHITE);
	void FillCircle(const core::vi2d& pos, std::int32_t radius, Pixel p = core::WHITE);
	void DrawRect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, Pixel p = core::WHITE);
	void DrawRect(const core::vi2d& pos, const core::vi2d& size, Pixel p = core::WHITE);
	void FillRect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, Pixel p = core::WHITE);
	void FillRect(const core::vi2d& pos, const core::vi2d& size, Pixel p = core::WHITE);
	void DrawTriangle(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, Pixel p = core::WHITE);
	void DrawTriangle(const core::vi2d& pos1, const core::vi2d& pos2, const core::vi2d& pos3, Pixel p = core::WHITE);
	void FillTriangle(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, Pixel p = core::WHITE);
	void FillTriangle(const core::vi2d& pos1, const core::vi2d& pos2, const core::vi2d& pos3, Pixel p = core::WHITE);
	void DrawSprite(std::int32_t x, std::int32_t y, Sprite* sprite, std::uint32_t scale = 1, std::uint8_t flip = core::Sprite::NONE);
	void DrawSprite(const core::vi2d& pos, Sprite* sprite, std::uint32_t scale = 1, std::uint8_t flip = core::Sprite::NONE);
	void DrawPartialSprite(std::int32_t x, std::int32_t y, Sprite* sprite, std::int32_t ox, std::int32_t oy, std::int32_t w, std::int32_t h, std::uint32_t scale = 1, std::uint8_t flip = core::Sprite::NONE);
	void DrawPartialSprite(const core::vi2d& pos, Sprite* sprite, const core::vi2d& sourcepos, const core::vi2d& size, std::uint32_t scale = 1, std::uint8_t flip = core::Sprite::NONE);

	void DrawDecal(const core::vf2d& pos, core::Decal* decal, const core::vf2d& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialDecal(const core::vf2d& pos, core::Decal* decal, const core::vf2d& source_pos, const core::vf2d& source_size, const core::vf2d& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialDecal(const core::vf2d& pos, const core::vf2d& size, core::Decal* decal, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint = core::WHITE);
	void DrawExplicitDecal(core::Decal* decal, const core::vf2d* pos, const core::vf2d* uv, const core::Pixel* col);
	void DrawWarpedDecal(core::Decal* decal, const core::vf2d(&pos)[4], const core::Pixel& tint = core::WHITE);
	void DrawWarpedDecal(core::Decal* decal, const core::vf2d* pos, const core::Pixel& tint = core::WHITE);
	void DrawWarpedDecal(core::Decal* decal, const std::array<core::vf2d, 4>& pos, const core::Pixel& tint = core::WHITE);
	void DrawPartialWarpedDecal(core::Decal* decal, const core::vf2d(&pos)[4], const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint = core::WHITE);
	void DrawPartialWarpedDecal(core::Decal* decal, const core::vf2d* pos, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint = core::WHITE);
	void DrawPartialWarpedDecal(core::Decal* decal, const std::array<core::vf2d, 4>& pos, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint = core::WHITE);
	void DrawRotatedDecal(const core::vf2d& pos, core::Decal* decal, const float fAngle, const core::vf2d& center = { 0.0f, 0.0f }, const core::vf2d& scale = { 1.0f,1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawPartialRotatedDecal(const core::vf2d& pos, core::Decal* decal, const float fAngle, const core::vf2d& center, const core::vf2d& source_pos, const core::vf2d& source_size, const core::vf2d& scale = { 1.0f, 1.0f }, const core::Pixel& tint = core::WHITE);
	void DrawStringDecal(const core::vf2d& pos, const std::string& sText, const Pixel col = core::WHITE, const core::vf2d& scale = { 1.0f, 1.0f });
	void FillRectDecal(const core::vf2d& pos, const core::vf2d& size, const core::Pixel col = core::WHITE);
	void GradientFillRectDecal(const core::vf2d& pos, const core::vf2d& size, const core::Pixel colTL, const core::Pixel colBL, const core::Pixel colBR, const core::Pixel colTR);


	void DrawString(std::int32_t x, std::int32_t y, const std::string& sText, Pixel col = core::WHITE, std::uint32_t scale = 1);
	void DrawString(const core::vi2d& pos, const std::string& sText, Pixel col = core::WHITE, std::uint32_t scale = 1);
	core::vi2d GetTextSize(const std::string& s);
	void Clear(Pixel p);
	void ClearBuffer(Pixel p, bool bDepth = true);

	std::string sAppName;
	Sprite* pDrawTarget = nullptr;
	Pixel::Mode nPixelMode = Pixel::NORMAL;
	float fBlendFactor = 1.0f;
	core::vi2d vScreenSize = { 256, 240 };
	core::vf2d vInvScreenSize = { 1.0f / 256.0f, 1.0f / 240.0f };
	core::vi2d vPixelSize = { 4, 4 };
	core::vi2d vScreenPixelSize = { 4, 4 };
	core::vi2d vMousePos = { 0, 0 };
	std::int32_t nMouseWheelDelta = 0;
	core::vi2d vMousePosCache = { 0, 0 };
	core::vi2d vMouseWindowPos = { 0, 0 };
	std::int32_t nMouseWheelDeltaCache = 0;
	core::vi2d vWindowSize = { 0, 0 };
	core::vi2d vViewPos = { 0, 0 };
	core::vi2d vViewSize = { 0,0 };
	bool bFullScreen = false;
	core::vf2d vPixel = { 1.0f, 1.0f };
	bool bHasInputFocus = false;
	bool bHasMouseFocus = false;
	bool bEnableVSYNC = false;
	float fFrameTimer = 1.0f;
	float fLastElapsed = 0.0f;
	int nFrameCount = 0;
	Sprite* fontSprite = nullptr;
	Decal* fontDecal = nullptr;
	Sprite* pDefaultDrawTarget = nullptr;
	std::vector<LayerDesc> vLayers;
	std::uint8_t nTargetLayer = 0;
	std::uint32_t nLastFPS = 0;
	bool bPixelCohesion = false;
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
	vi2d core_nextMousePos;
	bool core_hideCursor = false;

	core::rcode LoadImageResource(core::Sprite* spr, const std::string& sImageFile);


	constexpr bool Sprite::SetPixel(std::int32_t x, std::int32_t y, Pixel p) noexcept
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			pColData[y * width + x] = p;
			return true;
		}

		else return false;
	}

	constexpr Pixel Sprite::GetPixel(std::int32_t x, std::int32_t y) const noexcept
	{
		if (modeSample == Sprite::Mode::NORMAL) 
		{
			if (x >= 0 && x < width && y >= 0 && y < height) return pColData[y * width + x];
			else return Pixel{};
		}

		else return pColData[math::abs(y % height) * width + math::abs(x % width)];
	}

	constexpr Pixel Sprite::Sample(float x, float y) const noexcept
	{
		const std::int32_t sx = std::min(std::int32_t(x * float(width)), width - 1);
		const std::int32_t sy = std::min(std::int32_t(y * float(height)), height - 1);
		return GetPixel(sx, sy);
	}
}


#ifdef PGE_APPLICATION
#undef PGE_APPLICATION


typedef bool(WINAPI wglSwapInterval_t) (int interval);
static wglSwapInterval_t* wglSwapInterval = nullptr;

using glDeviceContext_t = HDC;
using glRenderContext_t = HGLRC;


namespace core
{
	glDeviceContext_t glDeviceContext = 0;
	glRenderContext_t glRenderContext = 0;

	bool bSync = false;

	core::PixelGameEngine* pge = nullptr;

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

	void DrawLayerQuad(const core::vf2d& offset, const core::vf2d& scale, const core::Pixel tint)
	{
		glBegin(GL_QUADS);
		glColor4ub(tint.r, tint.g, tint.b, tint.a);
		glTexCoord2f(0.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(0.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
		glVertex3f(1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
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
			glTexCoord4f(decal.uv[0].x, decal.uv[0].y, 0.0f, decal.w[0]); glVertex2f(decal.pos[0].x, decal.pos[0].y);
			glColor4ub(decal.tint[1].r, decal.tint[1].g, decal.tint[1].b, decal.tint[1].a);
			glTexCoord4f(decal.uv[1].x, decal.uv[1].y, 0.0f, decal.w[1]); glVertex2f(decal.pos[1].x, decal.pos[1].y);
			glColor4ub(decal.tint[2].r, decal.tint[2].g, decal.tint[2].b, decal.tint[2].a);
			glTexCoord4f(decal.uv[2].x, decal.uv[2].y, 0.0f, decal.w[2]); glVertex2f(decal.pos[2].x, decal.pos[2].y);
			glColor4ub(decal.tint[3].r, decal.tint[3].g, decal.tint[3].b, decal.tint[3].a);
			glTexCoord4f(decal.uv[3].x, decal.uv[3].y, 0.0f, decal.w[3]); glVertex2f(decal.pos[3].x, decal.pos[3].y);
			glEnd();
		}

		else
		{
			glBindTexture(GL_TEXTURE_2D, decal.decal->id);
			glBegin(GL_QUADS);
			glColor4ub(decal.tint[0].r, decal.tint[0].g, decal.tint[0].b, decal.tint[0].a);
			glTexCoord4f(decal.uv[0].x, decal.uv[0].y, 0.0f, decal.w[0]); glVertex2f(decal.pos[0].x, decal.pos[0].y);
			glTexCoord4f(decal.uv[1].x, decal.uv[1].y, 0.0f, decal.w[1]); glVertex2f(decal.pos[1].x, decal.pos[1].y);
			glTexCoord4f(decal.uv[2].x, decal.uv[2].y, 0.0f, decal.w[2]); glVertex2f(decal.pos[2].x, decal.pos[2].y);
			glTexCoord4f(decal.uv[3].x, decal.uv[3].y, 0.0f, decal.w[3]); glVertex2f(decal.pos[3].x, decal.pos[3].y);
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

	void UpdateViewport(const core::vi2d& pos, const core::vi2d& size)
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}






#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"
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

	core::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const core::vi2d& vViewPos, const core::vi2d& vViewSize)
	{
		if (CreateDevice({ core_hWnd }, bFullScreen, bEnableVSYNC) == core::rcode::OK)
		{
			UpdateViewport(vViewPos, vViewSize);
			return core::rcode::OK;
		}

		else return core::rcode::FAIL;
	}

	core::rcode CreateWindowPane(const core::vi2d& vWindowPos, core::vi2d& vWindowSize, bool bFullScreen)
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

		core::vi2d vTopLeft = vWindowPos;

		if (bFullScreen)
		{
			dwExStyle = 0;
			dwStyle = WS_VISIBLE | WS_POPUP;
			HMONITOR hmon = MonitorFromWindow(core_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			if (!GetMonitorInfoW(hmon, &mi)) return core::rcode::FAIL;
			vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
			vTopLeft.x = 0;
			vTopLeft.y = 0;
		}

		RECT rWndRect = { 0, 0, vWindowSize.x, vWindowSize.y };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		core_hWnd = CreateWindowExW(dwExStyle, L"core_PIXEL_GAME_ENGINE", L"OpenGL Game Engine", dwStyle,
			vTopLeft.x, vTopLeft.y, width, height, NULL, NULL, GetModuleHandleW(nullptr), NULL);

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
		LoadFromFile(sImageFile);
	}

	Pixel Sprite::SampleBL(float u, float v) const noexcept
	{
		u = u * width - 0.5f;
		v = v * height - 0.5f;
		int x = (int)std::floorf(u);
		int y = (int)std::floorf(v);
		float u_ratio = u - x;
		float v_ratio = v - y;
		float u_opposite = 1 - u_ratio;
		float v_opposite = 1 - v_ratio;

		core::Pixel p1 = GetPixel(std::max(x, 0), std::max(y, 0));
		core::Pixel p2 = GetPixel(std::min(x + 1, (int)width - 1), std::max(y, 0));
		core::Pixel p3 = GetPixel(std::max(x, 0), std::min(y + 1, (int)height - 1));
		core::Pixel p4 = GetPixel(std::min(x + 1, (int)width - 1), std::min(y + 1, (int)height - 1));

		return core::Pixel
		(
			(std::uint8_t)((p1.r * u_opposite + p2.r * u_ratio) * v_opposite + (p3.r * u_opposite + p4.r * u_ratio) * v_ratio),
			(std::uint8_t)((p1.g * u_opposite + p2.g * u_ratio) * v_opposite + (p3.g * u_opposite + p4.g * u_ratio) * v_ratio),
			(std::uint8_t)((p1.b * u_opposite + p2.b * u_ratio) * v_opposite + (p3.b * u_opposite + p4.b * u_ratio) * v_ratio)
		);
	}



	core::rcode Sprite::LoadFromFile(const std::string& sImageFile)
	{
		return LoadImageResource(this, sImageFile);
	}



	core::Sprite* Sprite::Duplicate() const
	{
		core::Sprite* spr = new core::Sprite(width, height);
		std::memcpy(spr->GetData(), GetData(), width * height * sizeof(core::Pixel));
		spr->modeSample = modeSample;
		return spr;
	}
	
	core::Sprite* Sprite::Duplicate(const core::vi2d& vPos, const core::vi2d& vSize) const
	{
		core::Sprite* spr = new core::Sprite(vSize.x, vSize.y);

		for (int y = 0; y < vSize.y; y++)
		{
			for (int x = 0; x < vSize.x; x++)
			{
				spr->SetPixel(x, y, GetPixel(vPos.x + x, vPos.y + y));
			}
		}

		return spr;
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
		pSprite = std::make_unique<core::Sprite>();
		if (pSprite->LoadFromFile(sFile))
		{
			pDecal = std::make_unique<core::Decal>(pSprite.get());
			return core::rcode::OK;
		}

		else
		{
			pSprite.release();
			pSprite = nullptr;
			return core::rcode::NO_FILE;
		}
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
		vWindowSize = vScreenSize * vPixelSize;
		bFullScreen = full_screen;
		bEnableVSYNC = vsync;
		vPixel = 2.0f / vScreenSize;

		if (vPixelSize.x <= 0 || vPixelSize.y <= 0 || vScreenSize.x <= 0 || vScreenSize.y <= 0) return core::FAIL;

		return core::OK;
	}



	void SetScreenSize(int w, int h)
	{
		vScreenSize = { w, h };
		vInvScreenSize = { 1.0f / float(w), 1.0f / float(h) };
		for (auto& layer : vLayers)
		{
			delete layer.pDrawTarget;
			layer.pDrawTarget = new Sprite(vScreenSize.x, vScreenSize.y);
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
		core_UpdateWindowSize(vWindowSize.x, vWindowSize.y);

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
	
	void SetLayerOffset(std::uint8_t layer, const core::vf2d& offset) noexcept
	{
		SetLayerOffset(layer, offset.x, offset.y);
	}
	
	void SetLayerOffset(std::uint8_t layer, float x, float y) noexcept
	{
		if (layer < vLayers.size()) vLayers[layer].vOffset = { x, y };
	}
	
	void SetLayerScale(std::uint8_t layer, const core::vf2d& scale) noexcept
	{
		SetLayerScale(layer, scale.x, scale.y);
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
		ld.pDrawTarget = new core::Sprite(vScreenSize.x, vScreenSize.y);
		ld.nResID = CreateTexture(vScreenSize.x, vScreenSize.y);
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
		return vMousePos.x;
	}
	
	std::int32_t GetMouseY() noexcept
	{
		return vMousePos.y;
	}
	
	const core::vi2d& GetMousePos() noexcept
	{
		return vMousePos;
	}
	
	std::int32_t GetMouseWheel() noexcept
	{
		return nMouseWheelDelta;
	}
	
	std::int32_t ScreenWidth() noexcept
	{
		return vScreenSize.x;
	}
	
	std::int32_t ScreenHeight() noexcept
	{
		return vScreenSize.y;
	}
	
	float GetElapsedTime() noexcept
	{
		return fLastElapsed;
	}
	
	const core::vi2d& GetWindowSize() noexcept
	{
		return vWindowSize;
	}
	
	const core::vi2d& GetPixelSize() noexcept
	{
		return vPixelSize;
	}
	
	const core::vi2d& GetScreenPixelSize() noexcept
	{
		return vScreenPixelSize;
	}
	
	const core::vi2d& GetWindowMouse() noexcept
	{
		return vMouseWindowPos;
	}
	
	void SetMousePos(vi2d pos) noexcept
	{
		core_SetMousePos(pos.x, pos.y);
	}
	
	bool Draw(const core::vi2d& pos, Pixel p)
	{
		return Draw(pos.x, pos.y, p);
	}
	
	bool Draw(std::int32_t x, std::int32_t y, Pixel p)
	{
		if (!pDrawTarget) return false;

		if (nPixelMode == Pixel::NORMAL)
		{
			return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::MASK)
		{
			if (p.a == 255) return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::ALPHA)
		{
			Pixel d = pDrawTarget->GetPixel(x, y);
			float a = static_cast<float>((p.a / 255.0f)) * fBlendFactor;
			float c = 1.0f - a;
			float r = a * (float)p.r + c * (float)d.r;
			float g = a * (float)p.g + c * (float)d.g;
			float b = a * (float)p.b + c * (float)d.b;
			return pDrawTarget->SetPixel(x, y, Pixel((std::uint8_t)r, (std::uint8_t)g, (std::uint8_t)b/*, (std::uint8_t)(p.a * fBlendFactor)*/));
		}

		if (nPixelMode == Pixel::CUSTOM)
		{
			return pDrawTarget->SetPixel(x, y, funcPixelMode(x, y, p, pDrawTarget->GetPixel(x, y)));
		}

		return false;
	}

	void DrawLine(const core::vi2d& pos1, const core::vi2d& pos2, Pixel p, std::uint32_t pattern)
	{
		DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p, pattern);
	}

	void DrawLine(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, Pixel p, std::uint32_t pattern)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		auto rol = [&](void)
		{ 
			pattern = (pattern << 1) | (pattern >> 31); return pattern & 1;
		};

		if (dx == 0)
		{
			if (y2 < y1) std::swap(y1, y2);
			for (y = y1; y <= y2; y++) if (rol()) Draw(x1, y, p);
			return;
		}

		if (dy == 0)
		{
			if (x2 < x1) std::swap(x1, x2);
			for (x = x1; x <= x2; x++) if (rol()) Draw(x, y1, p);
			return;
		}

		dx1 = math::abs(dx); dy1 = math::abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}

			else
			{
				x = x2; y = y2; xe = x1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0) px = px + 2 * dy1;

				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}

				if (rol()) Draw(x, y, p);
			}
		}

		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}

			else
			{
				x = x2; y = y2; ye = y1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0) py = py + 2 * dx1;

				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}

				if (rol()) Draw(x, y, p);
			}
		}
	}

	void DrawCircle(const core::vi2d& pos, std::int32_t radius, Pixel p, std::uint8_t mask)
	{
		DrawCircle(pos.x, pos.y, radius, p, mask);
	}

	void DrawCircle(std::int32_t x, std::int32_t y, std::int32_t radius, Pixel p, std::uint8_t mask)
	{
		if (radius < 0 || x < -radius || y < -radius || x - GetDrawTargetWidth() > radius || y - GetDrawTargetHeight() > radius) return;

		if (radius > 0)
		{
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;

			while (y0 >= x0)
			{
				if (mask & 0x01) Draw(x + x0, y - y0, p);
				if (mask & 0x04) Draw(x + y0, y + x0, p);
				if (mask & 0x10) Draw(x - x0, y + y0, p);
				if (mask & 0x40) Draw(x - y0, y - x0, p);
				if (x0 != 0 && x0 != y0)
				{
					if (mask & 0x02) Draw(x + y0, y - x0, p);
					if (mask & 0x08) Draw(x + x0, y + y0, p);
					if (mask & 0x20) Draw(x - y0, y + x0, p);
					if (mask & 0x80) Draw(x - x0, y - y0, p);
				}

				if (d < 0) d += 4 * x0++ + 6;
				else d += 4 * (x0++ - y0--) + 10;
			}
		}

		else Draw(x, y, p);
	}

	void FillCircle(const core::vi2d& pos, std::int32_t radius, Pixel p)
	{
		FillCircle(pos.x, pos.y, radius, p);
	}

	void FillCircle(std::int32_t x, std::int32_t y, std::int32_t radius, Pixel p)
	{
		if (radius < 0 || x < -radius || y < -radius || x - GetDrawTargetWidth() > radius || y - GetDrawTargetHeight() > radius) return;

		if (radius > 0)
		{
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;

			auto drawline = [&](int sx, int ex, int y)
			{
				for (int x = sx; x <= ex; x++) Draw(x, y, p);
			};

			while (y0 >= x0)
			{
				drawline(x - y0, x + y0, y - x0);
				if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

				if (d < 0) d += 4 * x0++ + 6;

				else
				{
					if (x0 != y0)
					{
						drawline(x - x0, x + x0, y - y0);
						drawline(x - x0, x + x0, y + y0);
					}

					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}

		else Draw(x, y, p);
	}

	void DrawRect(const core::vi2d& pos, const core::vi2d& size, Pixel p)
	{
		DrawRect(pos.x, pos.y, size.x, size.y, p);
	}

	void DrawRect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, Pixel p)
	{
		DrawLine(x, y, x + w, y, p);
		DrawLine(x + w, y, x + w, y + h, p);
		DrawLine(x + w, y + h, x, y + h, p);
		DrawLine(x, y + h, x, y, p);
	}

	void Clear(Pixel p)
	{
		int pixels = GetDrawTargetWidth() * GetDrawTargetHeight();
		Pixel* m = GetDrawTarget()->GetData();
		for (int i = 0; i < pixels; i++) m[i] = p;
	}


	void FillRect(const core::vi2d& pos, const core::vi2d& size, Pixel p)
	{
		FillRect(pos.x, pos.y, size.x, size.y, p);
	}

	void FillRect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, Pixel p)
	{
		std::int32_t x2 = x + w;
		std::int32_t y2 = y + h;

		if (x < 0) x = 0;
		if (x >= static_cast<std::int32_t>(GetDrawTargetWidth())) x = static_cast<std::int32_t>(GetDrawTargetWidth());
		if (y < 0) y = 0;
		if (y >= static_cast<std::int32_t>(GetDrawTargetHeight())) y = static_cast<std::int32_t>(GetDrawTargetHeight());

		if (x2 < 0) x2 = 0;
		if (x2 >= static_cast<std::int32_t>(GetDrawTargetWidth())) x2 = static_cast<std::int32_t>(GetDrawTargetWidth());
		if (y2 < 0) y2 = 0;
		if (y2 >= static_cast<std::int32_t>(GetDrawTargetHeight())) y2 = static_cast<std::int32_t>(GetDrawTargetHeight());

		for (int i = x; i < x2; i++)
		{
			for (int j = y; j < y2; j++)
			{
				Draw(i, j, p);
			}
		}
	}

	void DrawTriangle(const core::vi2d& pos1, const core::vi2d& pos2, const core::vi2d& pos3, Pixel p)
	{
		DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
	}

	void DrawTriangle(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, Pixel p)
	{
		DrawLine(x1, y1, x2, y2, p);
		DrawLine(x2, y2, x3, y3, p);
		DrawLine(x3, y3, x1, y1, p);
	}

	void FillTriangle(const core::vi2d& pos1, const core::vi2d& pos2, const core::vi2d& pos3, Pixel p)
	{
		FillTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
	}

	void FillTriangle(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, Pixel p)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;

		if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
		if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
		if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

		t1x = t2x = x1; y = y1;
		dx1 = (int)(x2 - x1);
		if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1);
		if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
		if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

		e2 = (int)(dx2 >> 1);
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;)
		{
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }

			while (i < dx1)
			{
				i++;
				e1 += dy1;

				while (e1 >= dx1)
				{
					e1 -= dx1;
					if (changed1) t1xp = signx1;
					else goto next1;
				}

				if (changed1) break;
				else t1x += signx1;
			}

		next1:
			while (1)
			{
				e2 += dy2;

				while (e2 >= dx2)
				{
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else goto next2;
				}

				if (changed2) break;
				else t2x += signx2;
			}

		next2:
			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);

			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}

	next:
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1)
		{
			std::swap(dy1, dx1);
			changed1 = true;
		}

		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) 
		{
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }

			while (i < dx1)
			{
				e1 += dy1;
				while (e1 >= dx1) 
				{
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }
					else goto next3;
				}

				if (changed1) break;
				else t1x += signx1;
				if (i < dx1) i++;
			}

		next3:
			while (t2x != x3) 
			{
				e2 += dy2;
				while (e2 >= dx2) 
				{
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else goto next4;
				}

				if (changed2) break;
				else t2x += signx2;
			}

		next4:
			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	void DrawSprite(const core::vi2d& pos, Sprite* sprite, std::uint32_t scale, std::uint8_t flip)
	{
		DrawSprite(pos.x, pos.y, sprite, scale, flip);
	}

	void DrawSprite(std::int32_t x, std::int32_t y, Sprite* sprite, std::uint32_t scale, std::uint8_t flip)
	{
		if (sprite == nullptr) return;

		std::int32_t fxs = 0, fxm = 1, fx = 0;
		std::int32_t fys = 0, fym = 1, fy = 0;

		if (flip & core::Sprite::Flip::HORIZ) { fxs = sprite->width - 1; fxm = -1; }
		if (flip & core::Sprite::Flip::VERT) { fys = sprite->height - 1; fym = -1; }

		if (scale > 1)
		{
			fx = fxs;
			for (std::int32_t i = 0; i < sprite->width; i++, fx += fxm)
			{
				fy = fys;

				for (std::int32_t j = 0; j < sprite->height; j++, fy += fym)
				{
					for (std::uint32_t is = 0; is < scale; is++)
					{
						for (std::uint32_t js = 0; js < scale; js++)
						{
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx, fy));
						}
					}
				}
			}
		}

		else
		{
			fx = fxs;
			for (std::int32_t i = 0; i < sprite->width; i++, fx += fxm)
			{
				fy = fys;

				for (std::int32_t j = 0; j < sprite->height; j++, fy += fym)
				{
					Draw(x + i, y + j, sprite->GetPixel(fx, fy));
				}
			}
		}
	}

	void DrawPartialSprite(const core::vi2d& pos, Sprite* sprite, const core::vi2d& sourcepos, const core::vi2d& size, std::uint32_t scale, std::uint8_t flip)
	{
		DrawPartialSprite(pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale, flip);
	}

	void DrawPartialSprite(std::int32_t x, std::int32_t y, Sprite* sprite, std::int32_t ox, std::int32_t oy, std::int32_t w, std::int32_t h, std::uint32_t scale, std::uint8_t flip)
	{
		if (sprite == nullptr) return;

		std::int32_t fxs = 0, fxm = 1, fx = 0;
		std::int32_t fys = 0, fym = 1, fy = 0;
		if (flip & core::Sprite::Flip::HORIZ) { fxs = w - 1; fxm = -1; }
		if (flip & core::Sprite::Flip::VERT) { fys = h - 1; fym = -1; }

		if (scale > 1)
		{
			fx = fxs;
			for (std::int32_t i = 0; i < w; i++, fx += fxm)
			{
				fy = fys;
				for (std::int32_t j = 0; j < h; j++, fy += fym)
				{
					for (std::uint32_t is = 0; is < scale; is++)
					{
						for (std::uint32_t js = 0; js < scale; js++)
						{
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx + ox, fy + oy));
						}
					}
				}
			}
		}

		else
		{
			fx = fxs;
			for (std::int32_t i = 0; i < w; i++, fx += fxm)
			{
				fy = fys;
				for (std::int32_t j = 0; j < h; j++, fy += fym)
				{
					Draw(x + i, y + j, sprite->GetPixel(fx + ox, fy + oy));
				}
			}
		}
	}

	void DrawPartialDecal(const core::vf2d& pos, core::Decal* decal, const core::vf2d& source_pos, const core::vf2d& source_size, const core::vf2d& scale, const core::Pixel& tint)
	{
		core::vf2d vScreenSpacePos =
		{
			(pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			((pos.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		core::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * source_size.x * vInvScreenSize.x) * scale.x,
			vScreenSpacePos.y - (2.0f * source_size.y * vInvScreenSize.y) * scale.y
		};

		DecalInstance di; di.decal = decal;	di.tint[0] = tint;

		di.pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
		di.pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
		di.pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
		di.pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };

		core::vf2d uvtl = source_pos * decal->vUVScale;
		core::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
		di.uv[0] = { uvtl.x, uvtl.y }; di.uv[1] = { uvtl.x, uvbr.y };
		di.uv[2] = { uvbr.x, uvbr.y }; di.uv[3] = { uvbr.x, uvtl.y };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawPartialDecal(const core::vf2d& pos, const core::vf2d& size, core::Decal* decal, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint)
	{
		core::vf2d vScreenSpacePos =
		{
			(pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			((pos.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		core::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * size.x * vInvScreenSize.x),
			vScreenSpacePos.y - (2.0f * size.y * vInvScreenSize.y)
		};

		DecalInstance di; di.decal = decal;	di.tint[0] = tint;

		di.pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
		di.pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
		di.pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
		di.pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };

		core::vf2d uvtl = source_pos * decal->vUVScale;
		core::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
		di.uv[0] = { uvtl.x, uvtl.y }; di.uv[1] = { uvtl.x, uvbr.y };
		di.uv[2] = { uvbr.x, uvbr.y }; di.uv[3] = { uvbr.x, uvtl.y };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawDecal(const core::vf2d& pos, core::Decal* decal, const core::vf2d& scale, const core::Pixel& tint)
	{
		core::vf2d vScreenSpacePos =
		{
			(pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			((pos.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		core::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(decal->sprite->width) * vInvScreenSize.x)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(decal->sprite->height) * vInvScreenSize.y)) * scale.y
		};

		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		di.pos[0] = { vScreenSpacePos.x, vScreenSpacePos.y };
		di.pos[1] = { vScreenSpacePos.x, vScreenSpaceDim.y };
		di.pos[2] = { vScreenSpaceDim.x, vScreenSpaceDim.y };
		di.pos[3] = { vScreenSpaceDim.x, vScreenSpacePos.y };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawRotatedDecal(const core::vf2d& pos, core::Decal* decal, const float fAngle, const core::vf2d& center, const core::vf2d& scale, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		di.pos[0] = (core::vf2d(0.0f, 0.0f) - center) * scale;
		di.pos[1] = (core::vf2d(0.0f, float(decal->sprite->height)) - center) * scale;
		di.pos[2] = (core::vf2d(float(decal->sprite->width), float(decal->sprite->height)) - center) * scale;
		di.pos[3] = (core::vf2d(float(decal->sprite->width), 0.0f) - center) * scale;
		float c = std::cosf(fAngle), s = std::sinf(fAngle);
		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = pos + core::vf2d(di.pos[i].x * c - di.pos[i].y * s, di.pos[i].x * s + di.pos[i].y * c);
			di.pos[i] = di.pos[i] * vInvScreenSize * 2.0f - core::vf2d(1.0f, 1.0f);
			di.pos[i].y *= -1.0f;
		}

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawExplicitDecal(core::Decal* decal, const core::vf2d* pos, const core::vf2d* uv, const core::Pixel* col)
	{
		DecalInstance di;
		di.decal = decal;

		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = col[i];
		}

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void FillRectDecal(const core::vf2d& pos, const core::vf2d& size, const core::Pixel col)
	{
		std::array<core::vf2d, 4> points = { { {pos}, {pos.x, pos.y + size.y}, {pos + size}, {pos.x + size.x, pos.y} } };
		std::array<core::vf2d, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<core::Pixel, 4> cols = { {col, col, col, col} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data());
	}

	void GradientFillRectDecal(const core::vf2d& pos, const core::vf2d& size, const core::Pixel colTL, const core::Pixel colBL, const core::Pixel colBR, const core::Pixel colTR)
	{
		std::array<core::vf2d, 4> points = { { {pos}, {pos.x, pos.y + size.y}, {pos + size}, {pos.x + size.x, pos.y} } };
		std::array<core::vf2d, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<core::Pixel, 4> cols = { {colTL, colBL, colBR, colTR} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data());
	}

	void DrawPartialRotatedDecal(const core::vf2d& pos, core::Decal* decal, const float fAngle, const core::vf2d& center, const core::vf2d& source_pos, const core::vf2d& source_size, const core::vf2d& scale, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		di.pos[0] = (core::vf2d(0.0f, 0.0f) - center) * scale;
		di.pos[1] = (core::vf2d(0.0f, source_size.y) - center) * scale;
		di.pos[2] = (core::vf2d(source_size.x, source_size.y) - center) * scale;
		di.pos[3] = (core::vf2d(source_size.x, 0.0f) - center) * scale;
		float c = std::cosf(fAngle), s = std::sinf(fAngle);

		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = pos + core::vf2d(di.pos[i].x * c - di.pos[i].y * s, di.pos[i].x * s + di.pos[i].y * c);
			di.pos[i] = di.pos[i] * vInvScreenSize * 2.0f - core::vf2d(1.0f, 1.0f);
			di.pos[i].y *= -1.0f;
		}

		core::vf2d uvtl = source_pos * decal->vUVScale;
		core::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
		di.uv[0] = { uvtl.x, uvtl.y }; di.uv[1] = { uvtl.x, uvbr.y };
		di.uv[2] = { uvbr.x, uvbr.y }; di.uv[3] = { uvbr.x, uvtl.y };

		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void DrawPartialWarpedDecal(core::Decal* decal, const core::vf2d* pos, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		core::vf2d center;

		float rd = ((pos[2].x - pos[0].x) * (pos[3].y - pos[1].y) - (pos[3].x - pos[1].x) * (pos[2].y - pos[0].y));
		if (rd != 0)
		{
			core::vf2d uvtl = source_pos * decal->vUVScale;
			core::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
			di.uv[0] = { uvtl.x, uvtl.y }; di.uv[1] = { uvtl.x, uvbr.y };
			di.uv[2] = { uvbr.x, uvbr.y }; di.uv[3] = { uvbr.x, uvtl.y };

			rd = 1.0f / rd;
			float rn = ((pos[3].x - pos[1].x) * (pos[0].y - pos[1].y) - (pos[3].y - pos[1].y) * (pos[0].x - pos[1].x)) * rd;
			float sn = ((pos[2].x - pos[0].x) * (pos[0].y - pos[1].y) - (pos[2].y - pos[0].y) * (pos[0].x - pos[1].x)) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = pos[0] + rn * (pos[2] - pos[0]);
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = (pos[i] - center).mag();

			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				di.uv[i] *= q; di.w[i] *= q;
				di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			}

			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void DrawWarpedDecal(core::Decal* decal, const core::vf2d* pos, const core::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.tint[0] = tint;
		core::vf2d center;
		float rd = ((pos[2].x - pos[0].x) * (pos[3].y - pos[1].y) - (pos[3].x - pos[1].x) * (pos[2].y - pos[0].y));

		if (rd != 0)
		{
			rd = 1.0f / rd;
			float rn = ((pos[3].x - pos[1].x) * (pos[0].y - pos[1].y) - (pos[3].y - pos[1].y) * (pos[0].x - pos[1].x)) * rd;
			float sn = ((pos[2].x - pos[0].x) * (pos[0].y - pos[1].y) - (pos[2].y - pos[0].y) * (pos[0].x - pos[1].x)) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = pos[0] + rn * (pos[2] - pos[0]);
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = (pos[i] - center).mag();

			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				di.uv[i] *= q; di.w[i] *= q;
				di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			}

			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void DrawWarpedDecal(core::Decal* decal, const std::array<core::vf2d, 4>& pos, const core::Pixel& tint)
	{
		DrawWarpedDecal(decal, pos.data(), tint);
	}

	void DrawWarpedDecal(core::Decal* decal, const core::vf2d(&pos)[4], const core::Pixel& tint)
	{
		DrawWarpedDecal(decal, &pos[0], tint);
	}

	void DrawPartialWarpedDecal(core::Decal* decal, const std::array<core::vf2d, 4>& pos, const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint)
	{
		DrawPartialWarpedDecal(decal, pos.data(), source_pos, source_size, tint);
	}

	void DrawPartialWarpedDecal(core::Decal* decal, const core::vf2d(&pos)[4], const core::vf2d& source_pos, const core::vf2d& source_size, const core::Pixel& tint)
	{
		DrawPartialWarpedDecal(decal, &pos[0], source_pos, source_size, tint);
	}

	void DrawStringDecal(const core::vf2d& pos, const std::string& sText, const Pixel col, const core::vf2d& scale)
	{
		core::vf2d spos = { 0.0f, 0.0f };

		for (auto& c : sText)
		{
			if (c == '\n')
			{
				spos.x = 0; spos.y += 8.0f * scale.y;
			}

			else
			{
				std::int32_t ox = (c - 32) % 16;
				std::int32_t oy = (c - 32) / 16;
				DrawPartialDecal(pos + spos, fontDecal, { float(ox) * 8.0f, float(oy) * 8.0f }, { 8.0f, 8.0f }, scale, col);
				spos.x += 8.0f * scale.x;
			}
		}
	}


	core::vi2d GetTextSize(const std::string& s)
	{
		core::vi2d size = { 0,1 };
		core::vi2d pos = { 0,1 };

		for (auto c : s)
		{
			if (c == '\n') { pos.y++;  pos.x = 0; }
			else pos.x++;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}

		return size * 8;
	}

	void DrawString(const core::vi2d& pos, const std::string& sText, Pixel col, std::uint32_t scale)
	{
		DrawString(pos.x, pos.y, sText, col, scale);
	}

	void DrawString(std::int32_t x, std::int32_t y, const std::string& sText, Pixel col, std::uint32_t scale)
	{
		std::int32_t sx = 0;
		std::int32_t sy = 0;
		Pixel::Mode m = nPixelMode;

		if (col.a != 255) SetPixelMode(Pixel::ALPHA);
		else SetPixelMode(Pixel::MASK);

		for (auto& c : sText)
		{
			if (c == '\n')
			{
				sx = 0; sy += 8 * scale;
			}

			else
			{
				std::int32_t ox = (c - 32) % 16;
				std::int32_t oy = (c - 32) / 16;

				if (scale > 1)
				{
					for (std::uint32_t i = 0; i < 8; i++)
					{
						for (std::uint32_t j = 0; j < 8; j++)
						{
							if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
							{
								for (std::uint32_t is = 0; is < scale; is++)
								{
									for (std::uint32_t js = 0; js < scale; js++)
									{
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
									}
								}
							}
						}
					}
				}

				else
				{
					for (std::uint32_t i = 0; i < 8; i++)
					{
						for (std::uint32_t j = 0; j < 8; j++)
						{
							if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
							{
								Draw(x + sx + i, y + sy + j, col);
							}
						}
					}
				}

				sx += 8 * scale;
			}
		}

		SetPixelMode(m);
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


	//bool OnUserCreate()
	//{
	//	return false;
	//}
	//
	//bool OnUserUpdate(float fElapsedTime)
	//{
	//	static_cast<void>(fElapsedTime); return false;
	//}
	//
	//bool OnUserDestroy()
	//{
	//	return true;
	//}

	void core_UpdateViewport()
	{
		std::int32_t ww = vScreenSize.x * vPixelSize.x;
		std::int32_t wh = vScreenSize.y * vPixelSize.y;
		float wasp = static_cast<float>(ww) / static_cast<float>(wh);

		if (bPixelCohesion)
		{
			vScreenPixelSize = (vWindowSize / vScreenSize);
			vViewSize = (vWindowSize / vScreenSize) * vScreenSize;
		}

		else
		{
			vViewSize.x = static_cast<std::int32_t>(vWindowSize.x);
			vViewSize.y = (std::int32_t)((float)vViewSize.x / wasp);

			if (vViewSize.y > vWindowSize.y)
			{
				vViewSize.y = vWindowSize.y;
				vViewSize.x = (std::int32_t)((float)vViewSize.y * wasp);
			}
		}

		vViewPos = (vWindowSize - vViewSize) / 2;
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

		x -= vViewPos.x;
		y -= vViewPos.y;
		vMousePosCache.x = (std::int32_t)(((float)x / (float)(vWindowSize.x - (vViewPos.x * 2)) * (float)vScreenSize.x));
		vMousePosCache.y = (std::int32_t)(((float)y / (float)(vWindowSize.y - (vViewPos.y * 2)) * (float)vScreenSize.y));
		if (vMousePosCache.x >= (std::int32_t)vScreenSize.x)	vMousePosCache.x = vScreenSize.x - 1;
		if (vMousePosCache.y >= (std::int32_t)vScreenSize.y)	vMousePosCache.y = vScreenSize.y - 1;
		if (vMousePosCache.x < 0) vMousePosCache.x = 0;
		if (vMousePosCache.y < 0) vMousePosCache.y = 0;
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
		core_nextMousePos.x = x;
		core_nextMousePos.y = y;
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

#endif //end core