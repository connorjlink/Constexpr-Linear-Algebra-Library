module;
#include <array>
#include <tuple>
#include <string>
#include <fstream>
export module stdex;

export namespace std
{
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
		return std::as_tuple<Formats...>(arr, std::make_index_sequence<N>{});
	}

	template<typename tuple_t>
	constexpr auto as_array(tuple_t&& tuple) noexcept
	{
		constexpr auto get_array = [](auto&& ... x) { return std::array{ std::forward<decltype(x)>(x) ... }; };
		return std::apply(get_array, std::forward<tuple_t>(tuple));
	}

	template<typename T, std::size_t... S>
	constexpr auto make_tuple_helper(T const& other, std::index_sequence<S...>)
	{
		return std::make_tuple(std::get<S>(other)...);
	}

	template<typename... Ts>
	constexpr auto flatten(std::tuple<Ts...> const& other)
	{
		return std::make_tuple_helper(other, std::make_index_sequence < sizeof...(Ts) - std::size_t{ 1 } > {});
	}

	template<>
	constexpr auto flatten(std::tuple<> const& other)
	{
		return other;
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

	template<typename T, std::size_t S>
	std::ostream& operator<<(std::ostream& stream, const std::array<std::array<T, S>, S>& mat) noexcept
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
		if constexpr (bool(sizeof...(Ts)))
		{
			stream << "[ ";

			[&]<std::size_t... i>(std::index_sequence<i...>)
			{
				((stream << std::get<i>(vec) << ", "), ...);
			}
			(std::make_index_sequence<(sizeof...(Ts) - 1)>{});

			stream << std::get<(sizeof...(Ts) - 1)>(vec) << " ]";

			return stream;
		}

		else
		{
			stream << "[ ]";

			return stream;
		}
	}
}