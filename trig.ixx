module;
#include <concepts>
export module trig;


export namespace core::trig
{
	template<typename T = float>
	constexpr auto radians(T&& degrees) noexcept requires std::floating_point<T>
	{
		return (std::move(degrees) / (T)57.295779513082320876798154814105L);
	}

	template<typename T = float>
	constexpr auto radians(const T& degrees) noexcept requires std::floating_point<T>
	{
		return (degrees / (T)57.295779513082320876798154814105L);
	}

	template<typename T = float>
	constexpr auto degrees(T&& radians) noexcept requires std::floating_point<T>
	{
		return (std::move(radians) * (T)57.295779513082320876798154814105L);
	}

	template<typename T = float>
	constexpr auto degrees(const T& radians) noexcept requires std::floating_point<T>
	{
		return (radians * (T)57.295779513082320876798154814105L);
	}
}