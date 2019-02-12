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
			constexpr auto compare_and_swap(Iterator lhs, Iterator rhs, Compare&& comp) -> bool {
				if (comp(*lhs, *rhs)) {
					using std::swap;
					swap(*lhs, *rhs);
					return true;
				}
				return false;
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

					struct partitioning {};
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
		}


		/**
		* @brief バブルソート
		*/
		struct bubble_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;


			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				if (begin == end) return;

				//入れ替えが行われなかった回数（後ろから見て並んでる個数）を記録
				int count{};

				//後ろから連続してcount回入れ替えられていなければ（=並んでいる）、その分後ろを減らす（見に行かなくする）
				for (auto loopend = end - 1; loopend != begin; std::advance(loopend, -count)) {
					count = 0;
					for (auto current = begin; current != loopend; ++current) {
						if (detail::compare_and_swap(current + 1, current,comp) == false)++count;
						else count = 0;
					}
					if (count == 0) ++count;
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		/**
		* @brief シェイカーソート
		*/
		struct shaker_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;
			
			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
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
						if (detail::compare_and_swap(current + 1, current, comp) == true)last_swap = current;
					}

					if (far_left == last_swap) return;

					//右端狭める
					far_right = last_swap;

					//左から右
					for (auto current = far_right; current != far_left; --current) {
						if (detail::compare_and_swap(current, current - 1, comp) == true)last_swap = current;
					}

					//左端狭める
					far_left = last_swap;
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		/**
		* @brief コムソート（コムソート11）
		*/
		struct comb_sort {
			static constexpr bool stable = false;

			using method = detail::category::method::swap;

			template<typename ForwardIterator, typename Compare>
			static constexpr void sort(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) {
				//イテレータ間距離の型
				using diff_t = typename std::iterator_traits<ForwardIterator>::difference_type;

				//h = h / 1.3 を求める
				constexpr auto calc_h = [](const diff_t N) -> diff_t { 
					if (N == diff_t(1)) return N;
					else {
						auto h = static_cast<diff_t>(std::trunc(N / 1.3));
						//Comb sort 11
						if (h == diff_t(9) || h == diff_t(10))return diff_t(11);
						return h;
					}
				};

				//要素数
				auto N = static_cast<diff_t>(std::distance(begin, end));

				if (N < diff_t(2)) return;
				//入れ替えが起きたかどうか
				bool is_swapped = false;

				//h=1のとき、交換が行われなくなるまで繰り返す
				for (auto h = calc_h(N); diff_t(1) < h || is_swapped; h = calc_h(h)) {
					is_swapped = false;
					for (auto i = diff_t(0); (i + h) < N; ++i) {
						//iの位置
						auto&& current = std::next(begin, i);
						//i+hの位置
						auto&& next = std::next(current, h);

						//next < current の時入れ替え
						//一回でもtrueが帰ればis_swappedは以降trueとなる（条件分岐回避のため、趣味）
						is_swapped |= detail::compare_and_swap(std::move(next), std::move(current), comp);
					}
				}
			}

			template<typename ForwardIterator, typename Compare>
			constexpr void operator()(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};


		/**
		* @brief ノームソート
		*/
		struct gnome_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;

			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				
				//終了位置
				auto far_right = end - 1;
				//注目位置
				auto current = begin;

				while (current != far_right)
				{
					//current < current + 1　次の要素と比較
					if (detail::compare_and_swap(current + 1, current, comp) == true) {
						//入れ替えが起きていたら
						if (current != begin) {
							//一つ戻って比較
							--current;
						}
						else {
							//左端の時は進む
							++current;
						}
					}
					else {
						//入れ替えが起きなかったら
						++current;
					}
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		/**
		* @brief 選択ソート
		*/
		struct selection_sort {
			static constexpr bool stable = false;

			using method = detail::category::method::select;

			template<typename ForwardIterator, typename Compare>
			static constexpr void sort(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) {
				//範囲の右端
				auto far_right = end - 1;

				if (begin == far_right) return;

				for (auto current = begin; current != far_right; ++current) {
					using std::swap;

					//一番小さい要素を探す
					auto min_it = std::min_element(current, end, comp);
					//それがcurrentがでなければ入れ替え
					if (min_it != current) swap(*min_it, *current);
				}
			}

			template<typename ForwardIterator, typename Compare>
			constexpr void operator()(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		struct insertion_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::insert;

			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				auto far_right = end - 1;

				//先頭と次をとりあえず比較し入れ替え
				detail::compare_and_swap(begin + 1, begin, comp);

				//3番目の要素から
				for (auto current = begin + 2; curent != far_right; ++current) {
					//一つ左の要素
					auto prev = current - 1;

					//正順であれば次へ
					if (comp(*prev, *current) == true) continue;

					//nextの要素をコピー
					auto tmp = std::move(*current);

					//挿入位置を探す
					do {
						*(prev + 1) = std::move(*prev);
						--prev;
						//prevが先頭でなく、tmp < prevである間ループ
					} while (prev != begin && comp(tmp, *prev));
					//挿入
					*prev = std::move(tmp);
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) const {
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

		sort<SortAlgorithm>(container, detail::comp_v<iterator>);
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
	constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
		SortAlgorithm::sort(begin, end, std::forward<Compare>(comp));
	}

}