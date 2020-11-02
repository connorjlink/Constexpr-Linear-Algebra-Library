module;
#include <vector>
export module mesh;

export namespace core::prim
{
	//define primitive type aliases
	template<typename T>
	struct Triangle { T p1, p2, p3; };

	template<typename T>
	struct Quad { T p1, p2, p3, p4; };
}

export namespace core::mesh
{
	//define mesh type aliases
	template<typename T>
	using mesh3 = std::vector<core::prim::Triangle<T>>;

	template<typename T>
	using mesh4 = std::vector<core::prim::Quad<T>>;
}