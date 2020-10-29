# Constexpr-Linear-Algebra-Library
A statically evaluable library for dealing with vectors and matricies, designed around some new C++20 features.

To use simply #include the header file in any other header files, as it is #pragma once'd.

Available Functionality:

-std::array as matrix type
-std::tuple as vector type

-vector dot, cross, scaling, combining, and applying, and mul
-matrix apply, compose, multiply

-perspective matrix function
-size templated identity matrix function
-all rotation matrices
-translation matrices

-overloaded operator<< for vector and matrix types
