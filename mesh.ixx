module;
#include <vector>
#include <array>
export module mesh;

import core;

export namespace cla
{
	//define primitive type aliases
	template<typename T = float>
	struct tri 
	{
		cla::v3d_generic<T> p1, p2, p3;

		tri<T>() = default;

		tri<T>(cla::v3d_generic<T> t1, cla::v3d_generic<T> t2, cla::v3d_generic<T> t3)
			: p1(t1), p2(t2), p3(t3) {} 
	};

	template<typename T = float>
	struct quad 
	{
		cla::v3d_generic<T> p1, p2, p3, p4;
		
		quad<T>() = default;

		quad<T>(cla::v3d_generic<T> t1, cla::v3d_generic<T> t2, cla::v3d_generic<T> t3, cla::v3d_generic<T> t4)
			: p1(t1), p2(t2), p3(t3), p4(t4) {}
	};

	//define mesh type aliases
	template<typename T = float>
	using mesh3 = std::vector<cla::tri<T>>;

	template<typename T = float>
	using mesh4 = std::vector<cla::quad<T>>;
}