#pragma once

#include <functional>
#include <type_traits>

/**
 * Shorthand for return type of invokation
 **/
template<typename Function, typename ...Args>
using invoke_return_t = std::invoke_result_t<std::decay_t<Function>,
					     std::decay_t<Args>...>;


/**
 * The identity function.
 *
 * It is used as the only value of a function for the type (a -> b) in concept
 *  Functor.
 *
 * This means that the concept will type check only if the transformation
 *  F<A> -> F<A> is valid. However, fmapping into invalid F specialisations will
 *  still fail.
 *
 * This approach lets the compiler deduce the fmap() call without having to
 *  provide starting and ending types, at the cost of deferring some checks to
 *  fmap() calls.
**/
static constexpr auto id = [](auto x){return x;};


/** Conditionally show ::type if condition is true **/
template<bool Condition, typename T>
struct exist_if {};

template<typename T>
struct exist_if<true, T> { using type = T; };


/** Workaround to always fail a static_assert **/
template<typename...> struct always_false { static constexpr auto value = false; };


/** fmap()
 * Non-overload base returns a volatile type. This way, checking if an overload exists
 *  is checking if the return isn't volatile.
 *
 * Guarding whether or not a type can be fmap'd are the concept #Functor and
 *  #is_functor. However, you might still call fmap(). This is why the body has
 *  a static_assert that will always fail with a diagnostic message.
 *
 * Since the non-overloaded function will get instantiated only when it is
 *  called, the assert won't get evaluated unless it is called.
**/
// Suppress volatile return warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
template<template<typename...> typename F, typename A, typename Function>
volatile F<invoke_return_t<Function, A>>
fmap(Function&& f, F<A>)
{
	static_assert(always_false<F<A>, A, Function>::value, "No implementation for this overload of fmap() exists!");
}
#pragma GCC diagnostic pop

/**
 * Concept that checks an overloaded fmap() call exists and will compile for a
 * starting functor (i.e. vector<int>)
 **/
template<typename F>
concept Functor = requires(F f) {
	typename exist_if<!std::is_volatile_v<decltype(fmap(id, f))>, void>::type;
	{ fmap(id, f) };
};


/**
 * Performs same check as concept Functor, except it sets a value instead of
 * failing a type check
 **/
template<typename F>
struct is_functor {
	static constexpr bool value = !std::is_volatile_v<decltype(fmap(id, std::declval<F>()))>;
};

template<typename F>
constexpr auto is_functor_v = is_functor<F>::value;
