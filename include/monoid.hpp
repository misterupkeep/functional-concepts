// A Monoid is any Semigroup that has a neutral element for its operation.
//  Examples are: int under +, int under *, strings under concatenation

#pragma once

#include <semigroup.hpp>

namespace __monoid_impl {
// Suppress volatile return warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wreturn-type"

// The goal of these functions is to
//  a) Provide the correct `volatile T` type to mempty in unevaluated contexts
//  b) Throw a helpful diagnostic in evaluated contexts
//
// To delay static_asserts from failing, we make the condition dependant on a
//  template type argument.
template<typename T> constexpr volatile T fail_static_assert_s()
{
	static_assert(always_false<T>::value,
		      "Used type is not a Semigroup! Minimal implementation requires: sappend | operator+");
}

template<typename T> constexpr volatile T fail_static_assert_m()
{
	static_assert(always_false<T>::value,
		      "Used type is not a Monoid! Minimal implementation requires: mempty");
}
#pragma GCC diagnostic pop
}

// We need mempty to fail when selected for a non-Semigroup type.
//  In unevaluated contexts, it should be clear from its return type.
//  In evaluated contexts, we throw a diagnostic with a failed assertion.
//
// We also need it to fail in the same way when used with types that don't have
// a Monoid implementation.
//
// So, the default overload is the bottom `enable_if_t` one. It checks if the
//  type is a Semigroup.
//
// If the type is not a Semigroup, fail the Semigroup assertion.
// If the type is a Semigroup, that means there isn't a more specific overload
//  for mempty, meaning there is no Monoid implementation -- fail the Monoid
//  assertion.
//
// TODO: Move mempty into something like struct mempty<T>{static const T value;}
template<typename RealType, typename = void> constexpr decltype(__monoid_impl::fail_static_assert_s<RealType>()) mempty;

template<typename T> constexpr decltype(__monoid_impl::fail_static_assert_m<T>()) mempty<T, std::enable_if_t<is_semigroup_v<T>>>;


namespace __monoid_impl {
// Instantiating mempty with a type that is not a Semigroup will cause a bunch
//  of evaluations to get the proper return type of fail_static_assert_s(), also
//  failing the assertion.
//
// The workaround is to short circuit the check, as it works fine with a
//  Semigroup type.
template<bool, typename> struct check {};
template<typename M> struct check<true, M> { static constexpr auto value = !std::is_volatile_v<decltype(mempty<M>)>; };
template<typename M> struct check<false, M> { static constexpr auto value = false; };
}

template<typename M>
concept Monoid = requires(M m) {
	typename std::enable_if_t<__monoid_impl::check<is_semigroup_v<M>, M>::value>;
};

template<typename M> struct is_monoid
{ static constexpr auto value = __monoid_impl::check<is_semigroup_v<M>, M>::value; };

template<typename M> constexpr auto is_monoid_v = is_monoid<M>::value;
