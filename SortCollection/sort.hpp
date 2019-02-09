#pragma once

#include <type_traits>
#include <iterator>
#include <utility>

namespace sort_collection {

	/**
	* @brief 各種ソートアルゴリズムを定義
	*/
	inline namespace algorithm {

		/**
		* @brief 内部処理
		*/
		namespace detail {

			/**
			* @brief 必要ならスワップする
			* @detail comp(*lhs, *rhs) == trueならswap(*lhs, *rhs)
			* @param lhs 左辺
			* @param rhs 右辺
			* @param comp 比較に使うファンクタ
			* @return comp(*lhs, *rhs)
			*/
			template<typename Iterator, typename Compare>
			constexpr auto swap(Iterator lhs, Iterator rhs, Compare&& comp) -> bool {
				if (comp(*lhs, *rhs)) {
					std::swap(*lhs, *rhs);
					return true;
				}
				return false;
			}
		}

		/**
		* @brief ソートのカテゴリに関わるタグ等の定義
		*/
		namespace category {

			inline namespace method {

				struct swap {};

				struct select {};

				struct insert {};

				struct merge {};

				struct partitioning{};
			}

		}

		/**
		* @brief デフォルトの比較ファンクタ型
		* @tparam Iterator イテレータ
		*/
		template<typename Iterator>
		using default_compare = std::less<typename std::iterator_traits<Iterator>::value_type>;

		/**
		* @brief デフォルトの比較ファンクタ
		* @tparam Iterator イテレータ
		*/
		template<typename Iterator>
		inline constexpr auto comp_v = default_compare<Iterator>{};


		/**
		* @brief バブルソート
		*/
		struct bubble_sort {
			static constexpr bool stable = true;

			using method = category::method::swap;


			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
				if (begin == end) return;

				//入れ替えが行われなかった回数（後ろから見て並んでる個数）を記録
				int count{};

				//後ろから連続してcount回入れ替えられていなければ（=並んでいる）、その分後ろを減らす（見に行かなくする）
				for (auto loopend = end - 1; loopend != begin; std::advance(loopend, -count)) {
					count = 0;
					for (auto current = begin; current != loopend; ++current) {
						if (detail::swap(current + 1, current,comp) == false)++count;
						else count = 0;
					}
					if (count == 0) ++count;
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		/**
		* @brief シェイカーソート
		*/
		struct shaker_sort {
			static constexpr bool stable = true;

			using method = category::method::swap;
			
			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
				if (begin == end) return;
				
				//左端と右端
				auto far_left = begin;
				auto far_right = end - 1;
				//最後に入れ替えが起きた場所
				auto last_swap = begin;

				while (far_left != far_right)
				{
					//右から左
					for (auto current = far_left; current != far_right; ++current) {
						if (detail::swap(current + 1, current, comp) == true)last_swap = current;
					}

					if (far_left == last_swap) return;

					//右端狭める
					far_right = last_swap;

					//左から右
					for (auto current = far_right; current != far_left; --current) {
						if (detail::swap(current, current - 1, comp) == true)last_swap = current;
					}

					//左端狭める
					far_left = last_swap;
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};
	}

	/**
	* @brief コンテナに対しソートを行う
	* @tparam SortAlgorithm ソートに使用するアルゴリズム
	* @param container ソートしたい任意のコンテナ（::iterator = BidirectionalIteratorであること）
	* @param comp 比較に使うファンクタ
	*/
	template<typename SortAlgorithm, typename Container, typename Compare>
	constexpr void sort(Container& container, Compare&& comp) {
		using std::begin;
		using std::end;

		auto begin_it = begin(container);
		auto end_it = end(container);

		SortAlgorithm::sort(begin_it, end_it, std::forward<Compare>(comp));
	}

	/**
	* @brief コンテナに対しソートを行う
	* @tparam SortAlgorithm ソートに使用するアルゴリズム
	* @param container ソートしたい任意のコンテナ（::iterator = BidirectionalIteratorであること）
	*/
	template<typename SortAlgorithm, typename Container>
	constexpr void sort(Container& container) {
		using std::begin;
		using iterator = std::remove_reference_t<decltype(begin(container))>;

		sort<SortAlgorithm>(container, comp_v<iterator>);
	}

	/**
	* @brief 範囲に対しソートを行う
	* @tparam SortAlgorithm ソートに使用するアルゴリズム
	* @tparam BidirectionalIterator 並べ替えるのに使うIterator（BidirectionalIteratorであること）
	* @param begin 範囲の初め
	* @param end 範囲の終わり
	* @param comp 比較に使うファンクタ
	*/
	template<typename SortAlgorithm, typename BidirectionalIterator, typename Compare>
	constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
		SortAlgorithm::sort(begin, end, std::forward<Compare>(comp));
	}

}