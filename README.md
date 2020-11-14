# Constexpr-Linear-Algebra-Library
A statically evaluable library for dealing with vectors and matricies, designed around some new C++20 features including concepts, the requires- clause, ranges, variadic type-deduced fold expressions useable in index sequence pack expansions recursed over operator , ...

To use simply #include the header file in any other header files, as it is #pragma once'd.

Available Functionality
{
    -vector operations as std::array
    -matrix operations as std::tuple

    -size templated common matrix functions
        -projection
        -translation
        -rotation

    -size templated common vector functions
        -dot
        -cross
        -sum
        -composition
        -binaryop

    -overloaded operator<< for vector and matrix types
}

Work in Progress
{
    -constexpr scientific functions
    -constexpr rotation matrices
    -proper debugging support
    -reducetion of heap allocations
}

In the Pipeline
{
    -quaternion operations
    -SIMD .impl's
    -gcc automatic command line completion for compilation
    -precache .vs and .vscode syntax symbol cabinets
}