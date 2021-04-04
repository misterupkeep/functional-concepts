// A Bifunctor is a container type which encapsulates two distinct types, and is
//  a covariant functor in both. What this means is that it has two types --
//  both of which you can change with the same mechanism as fmap() changes types
//  for Functors.
//
// Implementation wise, one should note the somewhat different template template
//  argument definition for the Bifunctor template arguments in the first(),
//  second() and bimap() functions.
//
// Instead of being something like:
//        template<typename...> typename Bifunctor
// It is instead:
//        template<typename, typename, typename...> typename Bifunctor
//
// This is to ensure that the Bifunctor type has at least two type parameters in
//  its template argument list. It could have been a 0-or-more list too, and it
//  would have been just as type-safe. However, templates' bad habit of
//  deferring things makes error messages hard to read, so this forces a more
//  eager type-check.
//
// The minimal definition for a Bifunctor is (first & second) | bimap. This
// means that all three functions are defined in terms of mutual recursion
// between the conjunctive groups:
//
//     ( these use bimap in their definition )
//        first f b = bimap f id b
//        second f b = bimap id f b
//
//     ( this uses first and second in its definition )
//        bimap f g b = first f (second g b)
//
// This is implemented with a constexpr if inside the bodies of the functions
//  that checks if there exists an overload for the necessary function
//
// If there indeed exists an overload, use that in the default implementation.
//  If there isn't, return a declval of the appropriate volatile-qualified type

#pragma once

#include <functional/common.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
template<template<typename, typename, typename...> typename Bifunctor, typename LeftA, typename RightA, typename LeftFunction>
auto first(LeftFunction&& left, Bifunctor<LeftA, RightA> bifunctor)
{
	// Entering this function body means there is no overload for second()
	// However, the minimal definitions are (first, second || bimap), so
	// let's check if an overload for bimap() exists
	if constexpr(std::is_volatile_v<decltype(bimap(left, id, bifunctor))>) {
		// bimap() returns volatile -> there is no overload
		// So, return a volatile type, too
		static_assert(always_false<Bifunctor<LeftA, RightA>, LeftFunction>::value,
			      "Used type is not a Bifunctor! Minimal implementation requires: (first & second) | bimap");
		return std::declval<volatile Bifunctor<invoke_return_t<LeftFunction, LeftA>, RightA>>();
	} else {
		return bimap(left, id, bifunctor);
	}
}

template<template<typename, typename, typename...> typename Bifunctor, typename LeftA, typename RightA, typename RightFunction>
auto second(RightFunction&& right, Bifunctor<LeftA, RightA> bifunctor)
{
	// Entering this function body means there is no overload for second()
	// However, the minimal definitions are (first, second || bimap), so
	// let's check if an overload for bimap() exists
	if constexpr(std::is_volatile_v<decltype(bimap(id, right, bifunctor))>) {
		// bimap() returns volatile -> there is no overload
		// So, return a volatile type, too
		static_assert(always_false<Bifunctor<LeftA, RightA>, RightFunction>::value,
			      "Used type is not a Bifunctor! Minimal implementation requires: (first & second) | bimap");
		return std::declval<volatile Bifunctor<LeftA, invoke_return_t<RightFunction, RightA>>>();
	} else {
		return bimap(id, right, bifunctor);
	}
}

template<template<typename, typename, typename...> typename Bifunctor, typename LeftA, typename RightA, typename LeftFunction, typename RightFunction>
auto bimap(LeftFunction&& left, RightFunction&& right, Bifunctor<LeftA, RightA> bifunctor)
{
	// Entering this function body means there is no overload for bimap()
	// However, let's check if overloads for first() and second() exist.
	if constexpr(std::is_volatile_v<decltype(first(left, bifunctor))>
		|| std::is_volatile_v<decltype(second(right, bifunctor))>) {
		// One of first(), second() returns volatile -> we're missing an overload
		// So, return a volatile return, too
		static_assert(always_false<Bifunctor<LeftA, RightA>, LeftFunction, RightFunction>::value,
			      "Used type is not a Bifunctor! Minimal implementation requires: (first & second) | bimap");
		return std::declval<volatile Bifunctor<invoke_return_t<LeftFunction, LeftA>, invoke_return_t<RightFunction, RightA>>>();
	} else {
		// We have overloads for both first() and second()
		return first(left, second(right, bifunctor));
	}
}
#pragma GCC diagnostic pop

template<typename B>
concept Bifunctor = requires(B bifunctor) {
	typename std::enable_if<
	(!std::is_volatile_v<decltype(first(id, bifunctor))> && !std::is_volatile_v<decltype(second(id, bifunctor))>)
	|| !std::is_volatile_v<decltype(bimap(id, id, bifunctor))>>::type;

	{ first(id, bifunctor) };
	{ second(id, bifunctor) };
	{ bimap(id, id, bifunctor) };
};

template<typename Bifunctor>
struct is_bifunctor {
	static constexpr auto value =
		(!std::is_volatile_v<decltype(first(id, std::declval<Bifunctor>()))>
		 && !std::is_volatile_v<decltype(second(id, std::declval<Bifunctor>()))>)
		|| !std::is_volatile_v<decltype(bimap(id, id, std::declval<Bifunctor>()))>;
};

template<typename F>
constexpr auto is_bifunctor_v = is_bifunctor<F>::value;
