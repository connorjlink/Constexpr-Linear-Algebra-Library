module;
#include <tuple>
#include <array>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>
#include "engine.hpp"
export module core;

export namespace cla
{
	//define the matrix alias to a c++ array of arrays
	template<typename T, unsigned int N>
	using matrix = std::array<std::array<T, N>, N>;

	//define matrix type aliases
	using int2x2 = matrix<int, 2>;
	using int3x3 = matrix<int, 3>;
	using int4x4 = matrix<int, 4>;

	using float2x2 = matrix<float, 2>;
	using float3x3 = matrix<float, 3>;
	using float4x4 = matrix<float, 4>;

	using double2x2 = matrix<double, 2>;
	using double3x3 = matrix<double, 3>;
	using double4x4 = matrix<double, 4>;


	template<typename T = float>
	struct v3d_generic
	{
		T x = (T)0.0f, y = (T)0.0f, z = (T)0.0f, w = (T)1.0f;

		constexpr v3d_generic<T>() : x((T)0.0f), y((T)0.0f), z((T)0.0f), w((T)1.0f) {}
		constexpr v3d_generic<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w((T)1.0f) {}
		constexpr v3d_generic<T>(const v3d_generic& v) : x(v.x), y(v.y), z(v.z), w((T)1.0f) {}
		v3d_generic& operator=(const v3d_generic& v) = default;
		bool operator == (const v3d_generic& rhs) const noexcept { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
		bool operator != (const v3d_generic& rhs) const noexcept { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
		const std::string str() const noexcept { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ", " + std::to_string(this->z) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const v3d_generic& rhs) { os << rhs.str(); return os; }
	};

	using vi3d = v3d_generic<int32_t>;
	using vu3d = v3d_generic<uint32_t>;
	using vf3d = v3d_generic<float>;
	using vd3d = v3d_generic<double>;
	
	//define primitive type aliases
	template<typename T = float>
	struct tri
	{
		cla::v3d_generic<T> p1, p2, p3;

		olc::Pixel col;

		tri<T>() = default;

		tri<T>(cla::v3d_generic<T> t1, cla::v3d_generic<T> t2, cla::v3d_generic<T> t3, olc::Pixel col) noexcept
			: p1(t1), p2(t2), p3(t3), col(col) {}
	};

	std::vector<cla::tri<float>> load(const std::string& filename)
	{
		std::ifstream f(filename);
		if (!f.is_open()) return {};

		std::vector<cla::tri<float>> tris;
		std::vector<cla::vf3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				cla::vf3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.emplace_back(cla::tri<float>(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1], olc::WHITE));
			}
		}

		return tris;
	}
}