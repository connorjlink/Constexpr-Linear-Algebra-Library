module;
#include <array>
#include <tuple>
#include <string>
#include <fstream>
export module stdex;

import core;

#pragma warning(suppress: 4455)
export constexpr std::size_t operator "" z(unsigned long long n) { return n; }

export namespace std
{
	constexpr auto pi_t = 3.14159265f;
	constexpr auto half_pi_t = pi_t / 2.0f;
	constexpr auto two_pi_t = pi_t * 2.0f;

	template<class T>
	constexpr T my_abs(const T& arg) noexcept
	{
		if (std::is_constant_evaluated()) return (arg < T{}) ? -arg : arg;
		else return static_cast<T>(std::fabs(arg));
	}

	template<class T>
	constexpr T my_sqrt(const T& arg) noexcept
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

		else return static_cast<T>(std::sqrtf(arg));
	}

	template<typename T, typename... Ts>
	constexpr auto make_array(Ts&&... t) noexcept -> std::array<T, sizeof...(Ts)>
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
		return std::as_tuple<Formats...>(arr, std::make_index_sequence<N>{});
	}

	template<typename tuple_t>
	constexpr auto as_array(tuple_t&& tuple) noexcept
	{
		constexpr auto get_array = [](auto&& ... x) { return std::array{ std::forward<decltype(x)>(x) ... }; };
		return std::apply(get_array, std::forward<tuple_t>(tuple));
	}

	template<typename T, std::size_t... S>
	constexpr auto make_tuple_helper(const T& other, std::index_sequence<S...>) noexcept
	{
		return std::make_tuple(std::get<S>(other)...);
	}

	template<typename... Ts>
	constexpr auto flatten(const std::tuple<Ts...>& other) noexcept
	{
		return std::make_tuple_helper(other, std::make_index_sequence < sizeof...(Ts) - std::size_t{ 1 } > {});
	}

	template<>
	constexpr auto flatten(const std::tuple<>& other)
	{
		return other;
	}

	template<typename T, typename... Ts>
	constexpr auto as_custom(const std::tuple<Ts...>& vec) noexcept requires (sizeof...(Ts) == 3 || sizeof...(Ts) == 4)
	{
		typename cla::vf3d outVec(0.0f, 0.0f, 0.0f);

		if constexpr (sizeof...(Ts) == 3)
		{
			outVec.x = std::get<0>(vec);
			outVec.y = std::get<1>(vec);
			outVec.z = std::get<2>(vec);
		}

		else
		{
			outVec.x = std::get<0>(vec);
			outVec.y = std::get<1>(vec);
			outVec.z = std::get<2>(vec);
			outVec.w = std::get<3>(vec);
		}

		return outVec;
	}

	template<typename T, std::size_t S>
	constexpr auto vec_array(const cla::v3d_generic<T>& vec) noexcept requires (S == 4)
	{
		std::array<T, S> outArray;

		outArray[0] = vec.x;
		outArray[1] = vec.y;
		outArray[2] = vec.z;
		outArray[3] = vec.w;
		
		return outArray;
	}

	template<typename T, std::size_t S>
	constexpr auto as_vector(const std::array<T, S>& arr) noexcept requires (S == 4) || (S == 3)
	{
		cla::vf3d outVec;

		if constexpr (S == 3)
		{
			outVec.x = arr[0];
			outVec.y = arr[1];
			outVec.z = arr[2];
		}

		else
		{
			outVec.x = arr[0];
			outVec.y = arr[1];
			outVec.z = arr[2];
			outVec.w = arr[3];
		}

		return outVec;
	}

	template <typename T, typename ...Ts>
	concept are_same = std::conjunction_v<std::is_same<T, Ts>...>;

	auto count_lines(std::string&& filepath) noexcept
	{
		std::size_t numLines = static_cast<std::size_t>(0);
		std::ifstream inFile(std::move(filepath));

		std::string lineStr;

		while (std::getline(inFile, lineStr))
		{
			++numLines;
		}

		return numLines;
	}

	template <typename T = float> 
	T signum(T val) noexcept
	{
		return T((T(0.0f) < val) - (val < T(0.0f)));
	}
}