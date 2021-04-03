#include <functional/vector.hpp>
#include <functional/array.hpp>
#include <extract.hpp>

template<typename T>
concept Addable = requires(T a, T b) {
	a + b;
};

template<Functor F> requires Addable<unwrap_first_t<F>>
auto
add_two(F functor)
{
	return fmap([](auto i){return i + 2;}, functor);
}


#include <iostream>
int
main()
{
	static_assert(is_fmappable<std::vector<int>>::value, "vector is_fmappable");

	static_assert(
		std::is_same<
		        int,
		        unwrap_first_t<std::array<int, 23>>
		>::value,
		"asd");

	std::vector int_vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector char_vec = fmap([](int i)->char{return'A'+i;}, add_two(int_vec));

	for(auto a: char_vec) std::cout << a << ' ';

	return 0;
}
