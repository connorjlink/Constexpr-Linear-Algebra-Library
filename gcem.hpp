module;
#include <cstddef>
#include <limits>
#include <type_traits>
export module gcem;

constexpr auto GCEM_VERSION_MAJOR = 1;
constexpr auto GCEM_VERSION_MINOR = 13;
constexpr auto GCEM_VERSION_PATCH = 1;

namespace gcem
{
	using uint_t = unsigned int;
	using ullint_t = unsigned long long int;

	using llint_t = long long int;

	template<class T>
	using GCLIM = std::numeric_limits<T>;

	template<typename T>
	using return_t = typename std::conditional<std::is_integral<T>::value, double, T>::type;

	template<typename ...T>
	using common_t = typename std::common_type<T...>::type;

	template<typename ...T>
	using common_return_t = return_t<common_t<T...>>;
}

constexpr auto GCEM_LOG_2 = 0.6931471805599453094172321214581765680755L;
constexpr auto GCEM_LOG_10 = 2.3025850929940456840179914546843642076011L;
constexpr auto GCEM_PI = 3.1415926535897932384626433832795028841972L;
constexpr auto GCEM_LOG_PI = 1.1447298858494001741434273513530587116473L;
constexpr auto GCEM_LOG_2PI = 1.8378770664093454835606594728112352797228L;
constexpr auto GCEM_LOG_SQRT_2PI = 0.9189385332046727417803297364056176398614L;
constexpr auto GCEM_SQRT_2 = 1.4142135623730950488016887242096980785697L;
constexpr auto GCEM_HALF_PI = 1.5707963267948966192313216916397514420986L;
constexpr auto GCEM_SQRT_PI = 1.7724538509055160272981674833411451827975L;
constexpr auto GCEM_SQRT_HALF_PI = 1.2533141373155002512078826424055226265035L;
constexpr auto GCEM_E = 2.7182818284590452353602874713526624977572L;

constexpr auto GCEM_ERF_MAX_ITER = 60;
constexpr auto GCEM_ERF_INV_MAX_ITER = 55;
constexpr auto GCEM_EXP_MAX_ITER_SMALL = 25;
constexpr auto GCEM_LOG_MAX_ITER_SMALL = 25;
constexpr auto GCEM_LOG_MAX_ITER_BIG = 255;
constexpr auto GCEM_INCML_BETA_TOL = 1E-15;
constexpr auto GCEM_INCML_BETA_MAX_ITER = 205;
constexpr auto GCEM_INCML_BETA_INV_MAX_ITER = 35;
constexpr auto GCEM_INCML_GAMMA_MAX_ITER = 55;
constexpr auto GCEM_INCML_GAMMA_INV_MAX_ITER = 35;
constexpr auto GCEM_SQRT_MAX_ITER = 100;
constexpr auto GCEM_TAN_MAX_ITER = 35;
constexpr auto GCEM_TANH_MAX_ITER = 35;

#ifndef GCEM_SIGNBIT
#define GCEM_SIGNBIT(x) _signbit(x)
#endif

#ifndef GCEM_COPYSIGN
#define GCEM_COPYSIGN(x,y) _copysign(x,y)
#endif


namespace gcem
{
	constexpr auto gauss_legendre_50_points[50] =
	{
		-0.03109833832718887611232898966595L,
		 0.03109833832718887611232898966595L,
		-0.09317470156008614085445037763960L,
		 0.09317470156008614085445037763960L,
		-0.15489058999814590207162862094111L,
		 0.15489058999814590207162862094111L,
		-0.21600723687604175684728453261710L,
		 0.21600723687604175684728453261710L,
		-0.27628819377953199032764527852113L,
		 0.27628819377953199032764527852113L,
		-0.33550024541943735683698825729107L,
		 0.33550024541943735683698825729107L,
		-0.39341431189756512739422925382382L,
		 0.39341431189756512739422925382382L,
		-0.44980633497403878914713146777838L,
		 0.44980633497403878914713146777838L,
		-0.50445814490746420165145913184914L,
		 0.50445814490746420165145913184914L,
		-0.55715830451465005431552290962580L,
		 0.55715830451465005431552290962580L,
		-0.60770292718495023918038179639183L,
		 0.60770292718495023918038179639183L,
		-0.65589646568543936078162486400368L,
		 0.65589646568543936078162486400368L,
		-0.70155246870682225108954625788366L,
		 0.70155246870682225108954625788366L,
		-0.74449430222606853826053625268219L,
		 0.74449430222606853826053625268219L,
		-0.78455583290039926390530519634099L,
		 0.78455583290039926390530519634099L,
		-0.82158207085933594835625411087394L,
		 0.82158207085933594835625411087394L,
		-0.85542976942994608461136264393476L,
		 0.85542976942994608461136264393476L,
		-0.88596797952361304863754098246675L,
		 0.88596797952361304863754098246675L,
		-0.91307855665579189308973564277166L,
		 0.91307855665579189308973564277166L,
		-0.93665661894487793378087494727250L,
		 0.93665661894487793378087494727250L,
		-0.95661095524280794299774564415662L,
		 0.95661095524280794299774564415662L,
		-0.97286438510669207371334410460625L,
		 0.97286438510669207371334410460625L,
		-0.98535408404800588230900962563249L,
		 0.98535408404800588230900962563249L,
		-0.99403196943209071258510820042069L,
		 0.99403196943209071258510820042069L,
		-0.99886640442007105018545944497422L,
		 0.99886640442007105018545944497422L
	};

	constexpr auto gauss_legendre_50_weights[50] =
	{
		0.06217661665534726232103310736061L,
		0.06217661665534726232103310736061L,
		0.06193606742068324338408750978083L,
		0.06193606742068324338408750978083L,
		0.06145589959031666375640678608392L,
		0.06145589959031666375640678608392L,
		0.06073797084177021603175001538481L,
		0.06073797084177021603175001538481L,
		0.05978505870426545750957640531259L,
		0.05978505870426545750957640531259L,
		0.05860084981322244583512243663085L,
		0.05860084981322244583512243663085L,
		0.05718992564772838372302931506599L,
		0.05718992564772838372302931506599L,
		0.05555774480621251762356742561227L,
		0.05555774480621251762356742561227L,
		0.05371062188899624652345879725566L,
		0.05371062188899624652345879725566L,
		0.05165570306958113848990529584010L,
		0.05165570306958113848990529584010L,
		0.04940093844946631492124358075143L,
		0.04940093844946631492124358075143L,
		0.04695505130394843296563301363499L,
		0.04695505130394843296563301363499L,
		0.04432750433880327549202228683039L,
		0.04432750433880327549202228683039L,
		0.04152846309014769742241197896407L,
		0.04152846309014769742241197896407L,
		0.03856875661258767524477015023639L,
		0.03856875661258767524477015023639L,
		0.03545983561514615416073461100098L,
		0.03545983561514615416073461100098L,
		0.03221372822357801664816582732300L,
		0.03221372822357801664816582732300L,
		0.02884299358053519802990637311323L,
		0.02884299358053519802990637311323L,
		0.02536067357001239044019487838544L,
		0.02536067357001239044019487838544L,
		0.02178024317012479298159206906269L,
		0.02178024317012479298159206906269L,
		0.01811556071348939035125994342235L,
		0.01811556071348939035125994342235L,
		0.01438082276148557441937890892732L,
		0.01438082276148557441937890892732L,
		0.01059054838365096926356968149924L,
		0.01059054838365096926356968149924L,
		0.00675979919574540150277887817799L,
		0.00675979919574540150277887817799L,
		0.00290862255315514095840072434286L,
		0.00290862255315514095840072434286L
	};


	template<typename T>
	constexpr bool is_neginf(const T& x) noexcept
	{
		return x == -GCLIM<T>::infinity();
	}

	template<typename T1, typename T2>
	constexpr bool any_neginf(const T1& x, const T2& y) noexcept
	{
		return(is_neginf(x) || is_neginf(y));
	}

	template<typename T1, typename T2>
	constexpr bool all_neginf(const T1& x, const T2& y) noexcept
	{
		return(is_neginf(x) && is_neginf(y));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool any_neginf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_neginf(x) || is_neginf(y) || is_neginf(z));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool all_neginf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_neginf(x) && is_neginf(y) && is_neginf(z));
	}

	template<typename T>
	constexpr bool is_posinf(const T& x) noexcept
	{
		return x == GCLIM<T>::infinity();
	}

	template<typename T1, typename T2>
	constexpr bool any_posinf(const T1& x, const T2& y) noexcept
	{
		return(is_posinf(x) || is_posinf(y));
	}

	template<typename T1, typename T2>
	constexpr bool all_posinf(const T1& x, const T2& y) noexcept
	{
		return(is_posinf(x) && is_posinf(y));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool any_posinf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_posinf(x) || is_posinf(y) || is_posinf(z));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool all_posinf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_posinf(x) && is_posinf(y) && is_posinf(z));
	}

	template<typename T>
	constexpr bool is_inf(const T& x) noexcept
	{
		return(is_neginf(x) || is_posinf(x));
	}

	template<typename T1, typename T2>
	constexpr bool any_inf(const T1& x, const T2& y) noexcept
	{
		return(is_inf(x) || is_inf(y));
	}

	template<typename T1, typename T2>
	constexpr bool all_inf(const T1& x, const T2& y) noexcept
	{
		return(is_inf(x) && is_inf(y));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool any_inf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_inf(x) || is_inf(y) || is_inf(z));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool all_inf(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_inf(x) && is_inf(y) && is_inf(z));
	}



	template<typename T>
	constexpr bool is_nan(const T& x) noexcept
	{
		return x != x;
	}

	template<typename T1, typename T2>
	constexpr bool any_nan(const T1& x, const T2& y) noexcept
	{
		return(is_nan(x) || is_nan(y));
	}

	template<typename T1, typename T2>
	constexpr bool all_nan(const T1& x, const T2& y) noexcept
	{
		return(is_nan(x) && is_nan(y));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool any_nan(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_nan(x) || is_nan(y) || is_nan(z));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool all_nan(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_nan(x) && is_nan(y) && is_nan(z));
	}

	
	template<typename T>
	constexpr bool is_finite(const T& x) noexcept
	{
		return (!is_nan(x)) && (!is_inf(x));
	}

	template<typename T1, typename T2>
	constexpr bool any_finite(const T1& x, const T2& y) noexcept
	{
		return(is_finite(x) || is_finite(y));
	}

	template<typename T1, typename T2>
	constexpr bool all_finite(const T1& x, const T2& y) noexcept
	{
		return(is_finite(x) && is_finite(y));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool any_finite(const T1& x, const T2& y, const T3& z) noexcept
	{
		return(is_finite(x) || is_finite(y) || is_finite(z));
	}

	template<typename T1, typename T2, typename T3>
	constexpr bool all_finite(const T1& x, const T2& y, const T3& z) noexcept
	{
		return (is_finite(x) && is_finite(y) && is_finite(z));
	}

	template<typename T>
	export constexpr T abs(const T& x) noexcept
	{
		return (x == T(0) ? T(0) : x < T(0) ? - x : x);
	}

	template<typename T>
	constexpr int ceil_resid(const T& x, const T& x_whole) noexcept
	{
		return ((x > T(0)) && (x > x_whole));
	}

	template<typename T>
	constexpr T ceil_int(const T& x, const T& x_whole) noexcept
	{
		return (x_whole + static_cast<T>(ceil_resid(x, x_whole)));
	}

	template<typename T>
	constexpr T ceil_check(const T& x) noexcept
	{
		return (is_nan(x) ? GCLIM<T>::quiet_NaN() : !is_finite(x) ? x : GCLIM<T>::epsilon() > abs(x) ? x : ceil_int(x, T(static_cast<llint_t>(x))));
	}

	template<typename T>
	export constexpr return_t<T> ceil(const T& x) noexcept
	{
		return ceil_check(static_cast<return_t<T>>(x));
	}

	template<typename T>
	constexpr int floor_resid(const T& x, const T& x_whole) noexcept
	{
		return ((x < T(0)) && (x < x_whole));
	}

	template<typename T>
	constexpr T floor_int(const T& x, const T& x_whole) noexcept
	{
		return (x_whole - static_cast<T>(floor_resid(x, x_whole)));
	}

	template<typename T>
	constexpr T floor_check(const T& x) noexcept
	{
		return (is_nan(x) ? GCLIM<T>::quiet_NaN() : !is_finite(x) ? x : GCLIM<T>::epsilon() > abs(x) ? x : floor_int(x, T(static_cast<llint_t>(x))));
	}

	template<typename T>
	export constexpr return_t<T> floor(const T& x) noexcept
	{
		return floor_check(static_cast<return_t<T>>(x));
	}

	template<typename T>
	constexpr T trunc_int(const T& x) noexcept
	{
		return (T(static_cast<llint_t>(x)));
	}

	template<typename T>
	constexpr T trunc_check(const T& x) noexcept
	{
		return (is_nan(x) ? GCLIM<T>::quiet_NaN() : !is_finite(x) ? x : GCLIM<T>::epsilon() > abs(x) ? x : trunc_int(x));
	}

	template<typename T>
	export constexpr return_t<T> trunc(const& T x) noexcept
	{
		return trunc_check(static_cast<return_t<T>>(x));
	}

	export constexpr bool is_odd(const llint_t& x) noexcept
	{
		return ((x & 1U) != 0);
	}

	export constexpr bool is_even(const llint_t& x) noexcept
	{
		return !(is_odd(x));
	}

	template<typename T1, typename T2>
	export constexpr common_t<T1, T2> max(const T1& x, const T2& y) noexcept
	{
		return (y < x ? x : y);
	}

	template<typename T1, typename T2>
	export constexpr common_t<T1, T2> min(const T1& x, const T2& y) noexcept
	{
		return (y > x ? x : y);
	}

	template<typename T>
	constexpr T sqrt_recur(const T& x, const T& xn, const int& count) noexcept
	{
		return (abs(xn - x / xn) / (T(1) + xn) < GCLIM<T>::epsilon() ? xn : count < GCEM_SQRT_MAX_ITER ? sqrt_recur(x, T(0.5) * (xn + x / xn), count + 1) : xn);
	}

	template<typename T>
	constexpr T sqrt_check(const T& x, const T& m_val) noexcept
	{
		return(is_nan(x) ? GCLIM<T>::quiet_NaN() : x < T(0) ? GCLIM<T>::quiet_NaN() : is_posinf(x) ? x : GCLIM<T>::epsilon() > abs(x) ? T(0) : GCLIM<T>::epsilon() > abs(T(1) - x) ? x : x > T(4) ? sqrt_check(x / T(4), T(2) * m_val) : m_val * sqrt_recur(x, x / T(2), 0));
	}

	template<typename T>
	export constexpr return_t<T> sqrt(const T& x) noexcept
	{
		return sqrt_check(static_cast<return_t<T>>(x), return_t<T>(1));
	}

	template <typename T>
	constexpr bool signbit(const T& x) noexcept
	{
		return ((x == T(0)) ? (_fpclass(x) == _FPCLASS_NZ) : (x < T(0)));
	}

	template <typename T1, typename T2>
	constexpr T1 copysign(const T1& x, const T2& y) noexcept
	{
		return (signbit(x) != signbit(y) ? -x : x);
	}

	template<typename T>
	constexpr bool neg_zero(const T& x) noexcept
	{
		return ((x == T(0.0)) && (copysign(T(1.0), x) == T(-1.0)));
	}

	template<typename T>
	constexpr int sgn(const T& x) noexcept
	{
		return (x > T(0) ? 1 : x < T(0) ? -1 : 0);
	}

	template<typename T>
	constexpr llint_t find_exponent(const T& x, const llint_t&& exponent) noexcept
	{
		return(x < T(1) ? find_exponent(x * T(10), exponent - llint_t(1)) : x > T(10) ? find_exponent(x / T(10), exponent + llint_t(1)) : exponent);
	}

	template<typename T>
	constexpr T find_fraction(const T& x) noexcept
	{
		return (abs(x - floor_check(x)) >= T(0.5) ? x - floor_check(x) - sgn(x) : x - floor_check(x));
	}

	template<typename T>
	constexpr llint_t find_whole(const T& x) noexcept
	{
		return (abs(x - internal::floor_check(x)) >= T(0.5) ? static_cast<llint_t>(internal::floor_check(x) + sgn(x)) : static_cast<llint_t>(internal::floor_check(x)));
	}

	template<typename T>
	export constexpr T mantissa(const T& x) noexcept
	{
		return (x < T(1) ? mantissa(x * T(10)) : x > T(10) ? mantissa(x / T(10)) : x);
	}

	template<typename T>
	constexpr T round_int(const T& x) noexcept
	{
		return static_cast<T>(find_whole(x));
	}

	template<typename T>
	constexpr T round_check(const T& x) noexcept
	{
		return (is_nan(x) ? GCLIM<T>::quiet_NaN() : !is_finite(x) ? x : GCLIM<T>::epsilon() > abs(x) ? x : sgn(x) * round_int(abs(x)));
	}

	template<typename T>
	export constexpr return_t<T> round(const T& x) noexcept
	{
		return round_check(static_cast<return_t<T>>(x));
	}

	template<typename T>
	constexpr T fmod_check(const T& x, const T& y) noexcept
	{
		return (any_nan(x, y) ? GCLIM<T>::quiet_NaN() : !all_finite(x, y) ? GCLIM<T>::quiet_NaN() : x - trunc(x / y) * y);
	}

	template<typename T1, typename T2, typename TC = common_return_t<T1, T2>>
	constexpr TC fmod_type_check(const T1& x, const T2& y) noexcept
	{
		return fmod_check(static_cast<TC>(x), static_cast<TC>(y));
	}

	template<typename T1, typename T2>
	export constexpr common_return_t<T1, T2> fmod(const T1& x, const T2& y) noexcept
	{
		return fmod_type_check(x, y);
	}

	keywords - 86 156 214
	functions - 211 211 150
	namespaces - 171 222 197
	braces and paranthesis - 116 199 222
	control - 210 143 218
	typenames - 107 222 168
	literals - 181 206 168
	parameters - 154 154 154
	local variables - 126 209 254

	#include "pow_integral.hpp"
	#include "exp.hpp"
	#include "expm1.hpp"
	#include "log.hpp"
	#include "log1p.hpp"
	#include "log2.hpp"
	#include "pow.hpp"

	#include "gcd.hpp"
	#include "lcm.hpp"

	#include "tan.hpp"
	#include "cos.hpp"
	#include "sin.hpp"

	#include "atan.hpp"
	#include "atan2.hpp"
	#include "acos.hpp"
	#include "asin.hpp"

	#include "tanh.hpp"
	#include "cosh.hpp"
	#include "sinh.hpp"

	#include "atanh.hpp"
	#include "acosh.hpp"
	#include "asinh.hpp"

	#include "binomial_coef.hpp"
	#include "lgamma.hpp"
	#include "tgamma.hpp"
	#include "factorial.hpp"
	#include "lbeta.hpp"
	#include "beta.hpp"
	#include "lmgamma.hpp"
	#include "log_binomial_coef.hpp"

	#include "erf.hpp"
	#include "erf_inv.hpp"
	#include "incomplete_beta.hpp"
	#include "incomplete_beta_inv.hpp"
	#include "incomplete_gamma.hpp"
	#include "incomplete_gamma_inv.hpp"
}