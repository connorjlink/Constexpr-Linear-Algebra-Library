A statically evaluable library for dealing with vectors and matricies, designed around some new C++20 features including concepts, the requires- clause, ranges, variadic type-deduced fold expressions useable in index sequence pack expansions recursed over inline constexpr operators. Every function is noexcept, const, and reference correct with perfect forwarding of rvalue reference pass-throughs. A mouthful to say, but really this just means that it's fairly quick, fairly easy to use, and most critically of all, can entirely be evaluated at compile time. Created with VS2019 .ixx compiled .ifc modules, which aren't cross compatible. To use on Linux or Mac, change file extensions of all included modules to GCC or Clang's preferred label.

Components (which reside in ::cla::):
matrix<T, S> -  represents an S*S matrix of Ts
v3d_generic<T> - represents a 3D vector, with optional w component built in
    
Functionality:
-matrix composition
-matrix reduction
-matrix multiplication
-matrix scaling
-matrix commons (identity, rotationXYZ, translation, projection, ...)

-vector reduction
-vector multiplication
-vector scaling
-vector addition
-vector application
