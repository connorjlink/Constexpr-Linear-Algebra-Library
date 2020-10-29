#include <iostream>
#include <tuple>
#include <utility>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <array>
#include <cmath>

#ifndef __cpp_lib_concepts
#define __cpp_lib_concepts
#include <concepts>
#endif

#include "gcem.hpp"

namespace std
{
	using int_t = int;
	using float_t = float;
	using double_t = double;
	using long_double_t = long double;
	using signed_t = signed;
	using unsigned_t = unsigned;
	using long_t = long;
	using long_long_t = long long;
	using short_t = short;

	constexpr auto pi_t = 3.14159265f;
	constexpr auto half_pi_t = pi_t / 2.0f;
	constexpr auto two_pi_t = pi_t * 2.0f;

	template<typename V, typename... Ts>
	constexpr auto make_array(Ts&&... t) noexcept -> std::array<V, sizeof...(Ts)>
	{
		return { { std::forward<Ts>(t)... } };
	}

	template<class... Formats, std::size_t N, std::size_t... Is, typename T>
	constexpr std::tuple<Formats...> as_tuple(const std::array<T, N>& arr, std::index_sequence<Is...>) noexcept
	{
		return std::make_tuple(Formats{ arr[Is] }...);
	}

	template<class... Formats, typename T, std::size_t N>
	constexpr std::tuple<Formats...> as_tuple(const std::array<T, N>& arr) noexcept requires (N == sizeof...(Formats))
	{
		return as_tuple<Formats...>(arr, std::make_index_sequence<N>{});
	}

	template<typename tuple_t>
	constexpr auto as_array(tuple_t&& tuple) noexcept
	{
		constexpr auto get_array = [](auto&& ... x) { return std::array{ std::forward<decltype(x)>(x) ... }; };
		return std::apply(get_array, std::forward<tuple_t>(tuple));
	}
}

namespace core
{
	//define the matrix alias to a c++ array
	template<std::size_t N, typename T>
	using matrix = std::array<std::array<T, N>, N>;

	//define matrix type aliases
	using int2x2 = matrix<2, std::int_t>;
	using int3x3 = matrix<3, std::int_t>;
	using int4x4 = matrix<4, std::int_t>;

	using float2x2 = matrix<2, std::float_t>;
	using float3x3 = matrix<3, std::float_t>;
	using float4x4 = matrix<4, std::float_t>;

	using double2x2 = matrix<2, std::double_t>;
	using double3x3 = matrix<3, std::double_t>;
	using double4x4 = matrix<4, std::double_t>;

	//define vector type aliases
	using int2 = std::tuple<std::int_t, std::int_t>;
	using int3 = std::tuple<std::int_t, std::int_t, std::int_t>;
	using int4 = std::tuple<std::int_t, std::int_t, std::int_t, std::int_t>;

	using float2 = std::tuple<std::float_t, std::float_t>;
	using float3 = std::tuple<std::float_t, std::float_t, std::float_t>;
	using float4 = std::tuple<std::float_t, std::float_t, std::float_t, std::float_t>;

	using double2 = std::tuple<std::double_t, std::double_t>;
	using double3 = std::tuple<std::double_t, std::double_t, std::double_t>;
	using double4 = std::tuple<std::double_t, std::double_t, std::double_t, std::double_t>;

	namespace trig
	{
		template<typename T = std::float_t>
		constexpr auto radians(T&& degrees) noexcept requires std::floating_point<T>
		{
			return (std::move(degrees) / (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto radians(const T& degrees) noexcept requires std::floating_point<T>
		{
			return (degrees / (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto degrees(T&& radians) noexcept requires std::floating_point<T>
		{
			return (std::move(radians) * (T)57.295779513082320876798154814105);
		}

		template<typename T = std::float_t>
		constexpr auto degrees(const T& radians) noexcept requires std::floating_point<T>
		{
			return (radians * (T)57.295779513082320876798154814105);
		}
	}

	namespace vec
	{
		template<typename binaryop = std::multiplies<void>, typename T, typename... Ts>
		constexpr auto apply(const std::tuple<Ts...>& vec, T&& operand) noexcept
		{
			std::tuple<Ts...> outVec;

			[&]<std::size_t... i>(std::index_sequence<i...>)
			{
				((std::get<i>(outVec) = binaryop{}(std::get<i>(vec), operand)) , ...);
			}
			(std::make_index_sequence<sizeof...(Ts)>{});

			return outVec;
		}

		template<typename binaryop = std::multiplies<void>, typename... Ts>
		constexpr auto apply(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept
		{
			std::tuple<Ts...> outVec;

			[&] <std::size_t... i>(std::index_sequence<i...>)
			{
				((std::get<i>(outVec) = binaryop{}(std::get<i>(vec1), std::get<i>(vec2))) , ...);
			}
			(std::make_index_sequence<sizeof...(Ts)>{});

			return outVec;
		}

		template<typename... Ts>
		constexpr auto sum(const std::tuple<Ts...>& vec) noexcept
		{
			return std::apply([](auto&&... v) { return (v + ...); }, vec);
		}

		template<typename... Ts>
		constexpr auto lengthSquared(const std::tuple<Ts...>& vec) noexcept
		{
			return sum(vec::apply<std::multiplies<>>(vec, vec));
		}

		template<typename... Ts>
		constexpr auto length(const std::tuple<Ts...>& vec) noexcept
		{
			return gcem::sqrt(lengthSquared(vec));
		}

		template<typename... Ts>
		constexpr auto normalize(const std::tuple<Ts...>& vec) noexcept
		{
			return vec::apply<std::divides<>>(vec, (length(vec)));
		}

		template<bool Normalize = false, typename... Ts>
		constexpr auto dot(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept
		{
			if constexpr (Normalize) return sum(std::tuple<Ts...>{ vec::apply<std::multiplies<>>(normalize(vec1), normalize(vec2)) });
			else return sum(std::tuple<Ts...>{ vec::apply<std::multiplies<>>(vec1, vec2) });
		}

		template<typename... Ts>
		constexpr auto cross(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept requires (sizeof...(Ts) == 3)
		{
			return std::make_tuple
			(
				((std::get<1>(vec1) * std::get<2>(vec2)) - (std::get<2>(vec1) * std::get<1>(vec2))),
				((std::get<2>(vec1) * std::get<0>(vec2)) - (std::get<0>(vec1) * std::get<2>(vec2))),
				((std::get<0>(vec1) * std::get<1>(vec2)) - (std::get<1>(vec1) * std::get<0>(vec2)))
			);
		}

		template<typename T, std::size_t S, typename... Ts>
		constexpr auto mul(const std::tuple<Ts...>& vec, const matrix<S, T>& mat) noexcept requires (S == sizeof...(Ts))
		{
			std::array<T, S> outVec;

			std::array<T, S> inVec = std::as_array(vec);

			for (auto i = 0; i < S; ++i)
			{
				outVec[i] = 0;

				for (auto j = 0; j < S; ++j)
				{
					outVec[i] += mat[i][j] * inVec[j];
				}
			}

			return std::as_tuple<Ts...>(outVec);
		}
	}

	namespace mat
	{
		template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
		constexpr auto apply(const matrix<S, T>& mat, T&& operand)
		{
			matrix<S, T> outMat;

			for (auto i = 0; i < S; ++i)
			{
				for (auto j = 0; j < S; ++j)
				{
					outMat[i][j] = binaryop{}(mat[i][j], operand);
				}
			}

			return outMat;
		}

		template<typename binaryop = std::multiplies<void>, typename T, std::size_t S>
		constexpr auto apply(const matrix<S, T>& mat1, const matrix<S, T>& mat2)
		{
			matrix<S, T> outMat;

			for (auto i = 0; i < S; ++i)
			{
				for (auto j = 0; j < S; ++j)
				{
					outMat[i][j] = binaryop{}(mat1[i][j], mat2[i][j]);
				}
			}

			return outMat;
		}

		template<typename T, std::size_t S>
		constexpr auto compose(const matrix<S, T>& m1, const matrix<S, T>& m2) noexcept
		{
			matrix<S, T> m3 = {};

			for (auto i = 0; i < S; i++)
			{
				for (auto j = 0; j < S; j++)
				{
					T sum = static_cast<T>(0);

					for (auto k = 0; k < S; k++)
					{
						sum += m1[i][k] * m2[k][j];
					}

					m3[i][j] = sum;
				}
			}

			return m3;
		}

		template<std::size_t S, typename T>
		constexpr auto identity() noexcept
		{
			matrix<S, T> mat = {};

			for (auto i = 0; i < S; ++i)
			{
				mat[i][i] = static_cast<T>(1);
			}

			return mat;
		}

		template<typename T>
		constexpr auto rotationX(T&& angle) noexcept
		{
			matrix<4, T> mat = {};

			mat[0][0] = 1.0f;
			mat[1][1] = gcem::cos(angle);
			mat[1][2] = gcem::sin(angle);
			mat[2][1] = -gcem::sin(angle);
			mat[2][2] = gcem::cos(angle);
			mat[3][3] = 1.0f;

			return mat;
		}

		template<typename T>
		constexpr auto rotationY(T&& angle) noexcept
		{
			matrix<4, T> mat = {};

			mat[0][0] = gcem::cos(angle);
			mat[0][2] = gcem::sin(angle);
			mat[2][0] = -gcem::sin(angle);
			mat[1][1] = 1.0f;
			mat[2][2] = gcem::cos(angle);
			mat[3][3] = 1.0f;

			return mat;
		}

		template<typename T>
		constexpr auto rotationZ(T&& angle) noexcept
		{
			matrix<4, T> mat = {};

			mat[0][0] = gcem::cos(angle);
			mat[0][1] = gcem::sin(angle);
			mat[1][0] = -gcem::sin(angle);
			mat[1][1] = gcem::cos(angle);
			mat[2][2] = 1.0f;
			mat[3][3] = 1.0f;

			return mat;
		}

		template<typename T>
		constexpr auto rotationXYZ(T&& x, T&& y, T&& z) noexcept
		{
			return mat::compose(mat::rotationX(std::move(x)), mat::compose(mat::rotationY(std::move(y)), mat::rotationZ(std::move(z))));
		}

		template<typename T>
		constexpr auto rotationXYZ(const T& x, const T& y, const T& z) noexcept
		{
			return mat::compose(mat::rotationX(x), mat::compose(mat::rotationY(y), mat::rotationZ(y)));
		}

		template<typename T>
		constexpr auto translation(T&& x, T&& y, T&& z) noexcept
		{
			matrix<4, T> mat;

			mat[0][0] = 1.0f;
			mat[1][1] = 1.0f;
			mat[2][2] = 1.0f;
			mat[3][3] = 1.0f;
			mat[3][0] = x;
			mat[3][1] = y;
			mat[3][2] = z;

			return mat;
		}

		template<typename T>
		constexpr auto projection(T&& fov, T&& aspectRatio, T&& near, T&& far) noexcept requires std::floating_point<T>
		{
			T fovRad = (T)1 / gcem::tan(fov * (T)0.5 / (T)180 * (T)3.14159265359);

			matrix<4, T> mat;

			mat[0][0] = aspectRatio * fovRad;
			mat[1][1] = fovRad;
			mat[2][2] = far / (far - near);
			mat[3][2] = (-far * near) / (far - near);
			mat[2][3] = 1.0f;
			mat[3][3] = 0.0f;

			return mat;
		}

		template<typename T, typename... Ts>
		constexpr auto pointAt(const std::tuple<Ts...>& pos, const std::tuple<Ts...>& target, const std::tuple<Ts...>& up) noexcept requires (sizeof...(Ts) == 3) or (sizeof...(Ts) == 4)
		{
			auto newForward = vec::normalize(vec::apply<std::minus<>>(target, pos));

			auto newUp = vec::normalize(vec::apply<std::minus<>>(up, vec::apply<std::multiplies<>>(newForward, vec::dot(up, newForward))));

			auto newRight = vec::cross(newUp, newForward);

			return matrix<4, T>
			{
				std::get<0>(newRight),   std::get<1>(newRight),   std::get<2>(newRight),   0.0f,
				std::get<0>(newUp),      std::get<1>(newUp),      std::get<2>(newUp),      0.0f,
				std::get<0>(newForward), std::get<1>(newForward), std::get<2>(newForward), 0.0f,
				std::get<0>(pos),        std::get<1>(pos),        std::get<2>(pos),        1.0f
			};
		}
	}
}

template<typename T, std::size_t S>
std::ostream& operator<<(std::ostream& stream, const core::matrix<S, T>& mat) noexcept
{
	stream << "[ ";

	for (auto i = 0; i < S; ++i)
	{
		for (auto j = 0; j < S; ++j)
		{
			stream << mat[i][j];

			if (j != (S - 1)) stream << ' ';
		}

		stream << " ]" << '\n';

		if (i != (S - 1)) stream << "[ ";
	}

	return stream;
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& stream, const std::tuple<Ts...>& vec) noexcept
{
	stream << "[ ";
	
	[&] <std::size_t... i>(std::index_sequence<i...>)
	{
		(( stream << std::get<i>(vec) << ", " ) , ...);
	} 
	(std::make_index_sequence<(sizeof...(Ts) - 1)>{});

	stream << std::get<(sizeof...(Ts) - 1)>(vec) << " ]";

	return stream;
}


int main()
{
	using namespace core;

	//constexpr int4x4 a = { 2, 3, 1, 4, 1, 4, 5, 8, 5, 1, 4, 5, 4, 2, 7, 5 };
	//constexpr int4 b = { 2, 5, 7, 3 };
	//
	//constexpr int4 c = vec::mul(b, a);
	//
	//std::cout << c << std::endl;

	constexpr float4x4 a = mat::rotationXYZ<float>(std::pi_t, std::pi_t, std::pi_t);

	std::cout << a << std::endl;

	return 0;
}