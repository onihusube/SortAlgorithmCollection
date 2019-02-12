#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "sort.hpp"

namespace SortCollection
{

	auto create_shuffled_vector(int size = 100) -> std::vector<int> {

		std::vector<int> array(size);

		auto begin = std::begin(array);
		auto end = std::end(array);

		std::iota(begin, end, 1);

		std::random_device seed_gen{};
		std::mt19937_64 rand{ seed_gen() };
		std::shuffle(begin, end, rand);

		return array;
	}

	template<size_t size = 100>
	auto expected_array() -> const int(&)[size] {
		static int array[size]{};
		static bool init = false;

		if (init == false) {
			std::iota(array, array + size, 1);
			init = true;
		}

		return array;
	}

	template<size_t size, typename Container>
	void check_array(const Container& container) {
		using std::begin;
		using std::end;

		auto begin_it = begin(container);
		auto end_it = end(container);

		auto& expected = expected_array<size>();
		Assert::IsTrue(std::equal(begin_it, end_it, expected, expected + size));
	}

	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(array_shuffle_test) 
		{
			auto array = create_shuffled_vector(100);

			auto& expedted = expected_array<100>();

			Assert::IsFalse(std::equal(std::begin(array), std::end(array), expedted, expedted + 100));
		}
		
		TEST_METHOD(bubble_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			sort_collection::sort<sort_collection::bubble_sort>(array);

			check_array<1000>(array);
		}

		TEST_METHOD(shaker_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			constexpr sort_collection::shaker_sort shaker_sort{};
			
			shaker_sort(std::begin(array), std::end(array), std::less<int>{});

			//sort_collection::sort<sort_collection::shaker_sort>(array);
			
			check_array<1000>(array);
		}

		TEST_METHOD(comb_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			sort_collection::sort<sort_collection::comb_sort>(array);

			check_array<1000>(array);
		}

		TEST_METHOD(gnome_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			sort_collection::sort<sort_collection::gnome_sort>(array);
			
			check_array<1000>(array);
		}

		TEST_METHOD(selection_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			sort_collection::sort<sort_collection::selection_sort>(array);

			check_array<1000>(array);
		}

		TEST_METHOD(insertion_sort_test)
		{
			auto array = create_shuffled_vector(1000);

			sort_collection::sort<sort_collection::selection_sort>(array);

			check_array<1000>(array);
		}
	};
}