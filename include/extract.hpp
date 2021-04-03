// A collection of structs, typedefs and functions, used to 'extract' types from
//  templates, i.e.
//
//         wrapper <------------+           +------------> the wrapped types
//                              |     /-----+------\
// 			  std::vector<int, allocator>
//
// The primary use for this is when dealing with functional concepts, such as
//  Functors:
//
//        template<typename T> requires Functor<T> && Addable<unwrap_first_t<T>>
//
// Essentially, it delegates the thinking and boilerplate to do the same
//  extractions *for* the developer.
//
// Unfortunately, C++ doesn't support mixed type/non-type parameter packs,
//  meaning there are some limitations to the way types are unwrapped.
//
// Namely, completely (non)type-parameter wrappers are supported, i.e.
//        vector<int, allocator_t> and my_t<12, 'c'>
// And specialisations have been manually written for
//        <type, non-type...>
//        <type, type, non-type...>
//        <non-type, type...>
//        <non-type, non-type, type...>
//
// If you need anything more than that, you can write your own template
//  specialisation for unwrap_nth ( see lines 91-116 ). Just a copy-paste job
//  with the right template parameters should do the trick!

#pragma once

#include <type_traits>

/**
 * Wrapper struct for template template arguments
 **/
template<template<typename...> typename Wrapper>
struct wrapper {
	template<typename ...Args>
	using type = Wrapper<Args...>;
};

/**
 * Extracts the container/wrapper type and returns it in #wrapper
 **/
template<template<typename...> typename Wrapper, typename ...Args>
constexpr auto get_wrapper(const Wrapper<Args...>&) -> wrapper<Wrapper>;


/**
 * Select N-th (1-based) type from pack
 **/
template<std::size_t N, typename ...Ts>
struct select_nth { using type = typename select_nth<N-1, Ts...>::type;};

// End case
template<typename X, typename... Ts>
struct select_nth<1, X, Ts...> { using type = X; };

// Base case
template<std::size_t N, typename X, typename... Ts>
struct select_nth<N, X, Ts...> { using type = typename select_nth<N-1, Ts...>::type; };

// Length mismatch. Ultimately nicer error message than with the concept trick
// with NonNullTypeArgs below.
template<std::size_t N> struct select_nth<N> {};


namespace __functional_impl {
// This counts the length of the parameter pack
template<typename...Args> struct  args_sz { static constexpr auto length = sizeof...(Args); };
template<auto...Args> struct auto_args_sz { static constexpr auto length = sizeof...(Args); };

// Conditionally shows a typedef, if the boolean condition is true
template<bool Condition, typename T> struct exist_if {};
template<                typename T> struct exist_if<true, T> { using type = T; };

// Since concepts are uneval context, outsourcing the evaluation to a type
//  existence check's template parameter forces the (size > 0) expression to get
//  evaluated.
template<typename...Args> concept NonNullTypeArgs = requires
{ typename exist_if<(args_sz<Args...>::length> 0), void>::type; };

template<auto...Args> concept NonNullAutoArgs = requires
{ typename exist_if<(auto_args_sz<Args...>::length> 0), void>::type; };
} // namespace __functional_impl

/**
 * Selects N-th (1-based) type from passed template, i.e.
 * unwrap_nth<1>(declval<std::vector<int>>()) -> int
 **/
template<std::size_t N, template<typename...> typename Wrapper, typename ...Args>
constexpr auto unwrap_nth(const Wrapper<Args...>&) -> typename select_nth<N, Args...>::type;

// As far as I can tell, C++ does not support catch-all packs for mixed
// type/non-type parameters. So, as some sort of workaround, I've written some
// specialisations for type/non-type parameter mixes:

//  <type, non-type...>,
template<std::size_t N, template<typename, auto...> typename Wrapper, typename T1, auto ...Args>
requires __functional_impl::NonNullAutoArgs<Args...> // deduction helper to prevent overload ambiguities
constexpr auto unwrap_nth(const Wrapper<T1, Args...>&) -> typename select_nth<N, T1, decltype(Args)...>::type;

//  <type, type, non-type...>,
template<std::size_t N, template<typename, typename, auto...> typename Wrapper, typename T1, typename T2, auto ...Args>
requires __functional_impl::NonNullAutoArgs<Args...> // deduction helper to prevent overload ambiguities
constexpr auto unwrap_nth(const Wrapper<T1, T2, Args...>&) -> typename select_nth<N, T1, T2, decltype(Args)...>::type;

//  <non-type, type...>,
template<std::size_t N, template<auto, typename...> typename Wrapper, auto T1, typename ...Args>
requires __functional_impl::NonNullTypeArgs<Args...> // deduction helper to prevent overload ambiguities
constexpr auto unwrap_nth(const Wrapper<T1, Args...>&) -> typename select_nth<N, decltype(T1), Args...>::type;

//  <non-type, non-type, type...>,
template<std::size_t N, template<auto, auto, typename...> typename Wrapper, auto T1, auto T2, typename ...Args>
requires __functional_impl::NonNullTypeArgs<Args...> // deduction helper to prevent overload ambiguities
constexpr auto unwrap_nth(const Wrapper<T1, T2, Args...>&) -> typename select_nth<N, decltype(T1), decltype(T2), Args...>::type;


/**
 * Helper type alias for appropriate unwrap_nth() call return type
 **/
template<std::size_t N, typename T>
using unwrap_nth_t = decltype(unwrap_nth<N>(std::declval<T>()));

template<typename T>
using unwrap_first_t = unwrap_nth_t<1, T>;

template<typename T>
using unwrap_second_t = unwrap_nth_t<2, T>;

template<typename T>
using unwrap_third_t = unwrap_nth_t<3, T>;

// God help you if you hit an error in these templates and cannot understand the
// soup above. My prayers are with you.
