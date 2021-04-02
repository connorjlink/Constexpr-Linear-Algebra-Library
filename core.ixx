module;
#include <tuple>
#include <array>
#include <string>
#include <cmath>
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

	//define vector type aliases
	using int2 = std::tuple<int, int>;
	using int3 = std::tuple<int, int, int>;
	using int4 = std::tuple<int, int, int, int>;

	using float2 = std::tuple<float, float>;
	using float3 = std::tuple<float, float, float>;
	using float4 = std::tuple<float, float, float, float>;

	using double2 = std::tuple<double, double>;
	using double3 = std::tuple<double, double, double>;
	using double4 = std::tuple<double, double, double, double>;


	template<typename T>
	struct v3d_generic
	{
		T x = (T)0, y = (T)0, z = (T)0, w = (T)1;

		constexpr v3d_generic<T>() : x(0), y(0), z(0), w(1) {}
		constexpr v3d_generic<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(1) {}
		constexpr v3d_generic<T>(const v3d_generic& v) : x(v.x), y(v.y), z(v.z), w(1) {}
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

}