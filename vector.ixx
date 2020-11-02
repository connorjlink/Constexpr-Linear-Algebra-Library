module;
#include <tuple>
#include <type_traits>
#include <functional>
#include <concepts>
#include "gcem.hpp"
export module vector;

import stdex;

export namespace cla::vec
{
	enum class mod
	{
		add, remove
	};

	template<mod op, typename T = int, typename... Ts>
	constexpr auto modify(const std::tuple<Ts...>& vec, T&& val = 0) noexcept requires std::are_same<T, Ts...>
	{
		if constexpr (op == mod::add)
		{
			return std::tuple_cat(vec, std::make_tuple(val));
		}

		if constexpr (op == mod::remove)
		{
			return std::flatten(vec);
		}
	}

	template<typename binaryop = std::multiplies<void>, typename T, typename... Ts>
	constexpr auto apply(const std::tuple<Ts...>& vec, T&& operand) noexcept
	{
		std::tuple<Ts...> outVec;

		[&]<std::size_t... i>(std::index_sequence<i...>)
		{
			((std::get<i>(outVec) = binaryop{}(std::get<i>(vec), operand)), ...);
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
			((std::get<i>(outVec) = binaryop{}(std::get<i>(vec1), std::get<i>(vec2))), ...);
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
		return cla::vec::sum(cla::vec::apply<std::multiplies<>>(vec, vec));
	}

	template<typename... Ts>
	constexpr auto length(const std::tuple<Ts...>& vec) noexcept
	{
		return gcem::sqrt(lengthSquared(vec));
	}

	template<typename... Ts>
	constexpr auto normalize(const std::tuple<Ts...>& vec) noexcept
	{
		return cla::vec::apply<std::divides<>>(vec, (cla::vec::length(vec)));
	}

	template<bool Normalize = false, typename... Ts>
	constexpr auto dot(const std::tuple<Ts...>& vec1, const std::tuple<Ts...>& vec2) noexcept
	{
		if constexpr (Normalize) return cla::vec::sum(std::tuple<Ts...>{ cla::vec::apply<std::multiplies<>>(cla::vec::normalize(vec1), cla::vec::normalize(vec2)) });
		else return cla::vec::sum(std::tuple<Ts...>{ cla::vec::apply<std::multiplies<>>(vec1, vec2) });
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
	constexpr auto mul(const std::tuple<Ts...>& vec, const std::array<std::array<T, S>, S>& mat) noexcept requires (S == sizeof...(Ts))
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