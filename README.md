# functional_concepts
A short header-only C++20 concepts library to add functional programming
interfaces and typeclasses, such as `Functor`, `Contrafunctor`, `Bifunctor`,
`Profunctor`, `Monoid`... as well as providing typeclass instances for STL
types.

Users can implement typeclass functions for their types by writing template
specialisations for them. The library will then automatically detect if a type
is part of a typeclass.

Aside from providing a concept for each typeclass (i.e. `Functor`), the library
also provides a traditional boolean check struct, too (i.e.
`is_fmappable<std::vector<int>>::value`);

## Extra utilities
Apart from introducing functional typeclasses, the library has several useful
structs, typedefs and functions for 'extracting' types from 'wrappers'.

This means getting the types _inside_ a templated type like
`std::vector<int,allocator_t>`.

See `include/extract.hpp` for more details.

## Implementation details

### Function template overload detection
Detecting if a type has an overloaded typeclass function call -- in other words,
checking if you can call `fmap()` on your type -- works by checking the return
type of the would-be function call with your type.

The base declaration of each typeclass function returns a volatile-qualified
type. The assumption is that user code _won't_ return volatile types (in fact,
such return types are deprecated in C++20!).

This is the only way, that I know, of detecting function overloads _without_
using classes with an overloaded `operator()`, which would make the syntax a bit
more cumbersome.

### Motivation for syntax
Speaking of syntax, the motivation is to make it as unintrusive as possible,
requiring as little explicit type argument passing as possible.

Here's an example of making our custom array type a `Functor`:
```c++
template<typename T, size_t N>
struct array {
    static constexpr auto size = N;
	T items[N];
	auto operator[](size_t i) { return items[i]; }
};

#include <functor.hpp>

template<typename A, typename Function, size_t N>
auto
fmap(Function&& f, array<A, N> arr)
{
	array<invoke_result_t<Function, A>, N> mapped_arr;
	for(auto i = 0; i < arr.size; i++) mapped_arr[i] = f(arr[i]);

	return mapped_arr;
}

// array is now a Functor!
```


And here's how you write a function that works on `Functor`s:
```c++
template<typename T>
concept Addable = requires(T a, T b) { a + b; };

// Function that works on all functors whose wrapped type is Addable
template<Functor T> requires Addable<unwrap_first_t<T>>
auto add_one(T functor)
{ return fmap([](auto i){return i+2;}, functor); }

int main() {
	std::array arr = {1,2,3,4,5,6,7,8,9};
	std::array arr_plus_one = add_one(arr);
}
```

Perhaps the ugliest bits of syntax here are `unwrap_first_t<T>`, and standard
C++ lambda notation. And, if not for `unwrap_first_t<T>` syntax, the template
argument list would get needlessly long and complicated.
