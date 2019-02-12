#pragma once

#include <type_traits>
#include <iterator>
#include <utility>

namespace sort_collection {

	/**
	* @brief �e��\�[�g�A���S���Y�����`
	*/
	inline namespace algorithm {

		/**
		* @brief ��������
		*/
		namespace detail {

			/**
			* @brief �K�v�Ȃ�X���b�v����
			* @detail comp(*lhs, *rhs) == true�Ȃ�swap(*lhs, *rhs)
			* @param lhs ����
			* @param rhs �E��
			* @param comp ��r�Ɏg���t�@���N�^
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
			* @brief �\�[�g�̃J�e�S���Ɋւ��^�O���̒�`
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
			* @brief �f�t�H���g�̔�r�t�@���N�^�^
			* @tparam Iterator �C�e���[�^
			*/
			template<typename Iterator>
			using default_compare = std::less<typename std::iterator_traits<Iterator>::value_type>;

			/**
			* @brief �f�t�H���g�̔�r�t�@���N�^
			* @tparam Iterator �C�e���[�^
			*/
			template<typename Iterator>
			inline constexpr auto comp_v = default_compare<Iterator>{};
		}


		/**
		* @brief �o�u���\�[�g
		*/
		struct bubble_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;


			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				if (begin == end) return;

				//����ւ����s���Ȃ������񐔁i��납�猩�ĕ���ł���j���L�^
				int count{};

				//��납��A������count�����ւ����Ă��Ȃ���΁i=����ł���j�A���̕��������炷�i���ɍs���Ȃ�����j
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
		* @brief �V�F�C�J�[�\�[�g
		*/
		struct shaker_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;
			
			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				if (begin == end) return;
				
				//���[�ƉE�[
				auto far_left = begin;
				auto far_right = end - 1;
				//�Ō�ɓ���ւ����N�����ꏊ
				auto last_swap = begin;

				while (far_left != far_right)
				{
					//�E���獶
					for (auto current = far_left; current != far_right; ++current) {
						if (detail::compare_and_swap(current + 1, current, comp) == true)last_swap = current;
					}

					if (far_left == last_swap) return;

					//�E�[���߂�
					far_right = last_swap;

					//������E
					for (auto current = far_right; current != far_left; --current) {
						if (detail::compare_and_swap(current, current - 1, comp) == true)last_swap = current;
					}

					//���[���߂�
					far_left = last_swap;
				}
			}

			template<typename BidirectionalIterator, typename Compare>
			constexpr void operator()(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) const {
				sort(begin, end, std::forward<Compare>(comp));
			}
		};

		/**
		* @brief �R���\�[�g�i�R���\�[�g11�j
		*/
		struct comb_sort {
			static constexpr bool stable = false;

			using method = detail::category::method::swap;

			template<typename ForwardIterator, typename Compare>
			static constexpr void sort(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) {
				//�C�e���[�^�ԋ����̌^
				using diff_t = typename std::iterator_traits<ForwardIterator>::difference_type;

				//h = h / 1.3 �����߂�
				constexpr auto calc_h = [](const diff_t N) -> diff_t { 
					if (N == diff_t(1)) return N;
					else {
						auto h = static_cast<diff_t>(std::trunc(N / 1.3));
						//Comb sort 11
						if (h == diff_t(9) || h == diff_t(10))return diff_t(11);
						return h;
					}
				};

				//�v�f��
				auto N = static_cast<diff_t>(std::distance(begin, end));

				if (N < diff_t(2)) return;
				//����ւ����N�������ǂ���
				bool is_swapped = false;

				//h=1�̂Ƃ��A�������s���Ȃ��Ȃ�܂ŌJ��Ԃ�
				for (auto h = calc_h(N); diff_t(1) < h || is_swapped; h = calc_h(h)) {
					is_swapped = false;
					for (auto i = diff_t(0); (i + h) < N; ++i) {
						//i�̈ʒu
						auto&& current = std::next(begin, i);
						//i+h�̈ʒu
						auto&& next = std::next(current, h);

						//next < current �̎�����ւ�
						//���ł�true���A���is_swapped�͈ȍ~true�ƂȂ�i�����������̂��߁A��j
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
		* @brief �m�[���\�[�g
		*/
		struct gnome_sort {
			static constexpr bool stable = true;

			using method = detail::category::method::swap;

			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
				
				//�I���ʒu
				auto far_right = end - 1;
				//���ڈʒu
				auto current = begin;

				while (current != far_right)
				{
					//current < current + 1�@���̗v�f�Ɣ�r
					if (detail::compare_and_swap(current + 1, current, comp) == true) {
						//����ւ����N���Ă�����
						if (current != begin) {
							//��߂��Ĕ�r
							--current;
						}
						else {
							//���[�̎��͐i��
							++current;
						}
					}
					else {
						//����ւ����N���Ȃ�������
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
		* @brief �I���\�[�g
		*/
		struct selection_sort {
			static constexpr bool stable = false;

			using method = detail::category::method::select;

			template<typename ForwardIterator, typename Compare>
			static constexpr void sort(ForwardIterator begin, ForwardIterator end, Compare&& comp = detail::comp_v<ForwardIterator>) {
				//�͈͂̉E�[
				auto far_right = end - 1;

				if (begin == far_right) return;

				for (auto current = begin; current != far_right; ++current) {
					using std::swap;

					//��ԏ������v�f��T��
					auto min_it = std::min_element(current, end, comp);
					//���ꂪcurrent���łȂ���Γ���ւ�
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

				//�擪�Ǝ����Ƃ肠������r������ւ�
				detail::compare_and_swap(begin + 1, begin, comp);

				//3�Ԗڂ̗v�f����
				for (auto current = begin + 2; curent != far_right; ++current) {
					//����̗v�f
					auto prev = current - 1;

					//�����ł���Ύ���
					if (comp(*prev, *current) == true) continue;

					//next�̗v�f���R�s�[
					auto tmp = std::move(*current);

					//�}���ʒu��T��
					do {
						*(prev + 1) = std::move(*prev);
						--prev;
						//prev���擪�łȂ��Atmp < prev�ł���ԃ��[�v
					} while (prev != begin && comp(tmp, *prev));
					//�}��
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
	* @brief �R���e�i�ɑ΂��\�[�g���s��
	* @tparam SortAlgorithm �\�[�g�Ɏg�p����A���S���Y��
	* @param container �\�[�g�������C�ӂ̃R���e�i�i::iterator = BidirectionalIterator�ł��邱�Ɓj
	* @param comp ��r�Ɏg���t�@���N�^
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
	* @brief �R���e�i�ɑ΂��\�[�g���s��
	* @tparam SortAlgorithm �\�[�g�Ɏg�p����A���S���Y��
	* @param container �\�[�g�������C�ӂ̃R���e�i�i::iterator = BidirectionalIterator�ł��邱�Ɓj
	*/
	template<typename SortAlgorithm, typename Container>
	constexpr void sort(Container& container) {
		using std::begin;
		using iterator = std::remove_reference_t<decltype(begin(container))>;

		sort<SortAlgorithm>(container, detail::comp_v<iterator>);
	}

	/**
	* @brief �͈͂ɑ΂��\�[�g���s��
	* @tparam SortAlgorithm �\�[�g�Ɏg�p����A���S���Y��
	* @tparam BidirectionalIterator ���בւ���̂Ɏg��Iterator�iBidirectionalIterator�ł��邱�Ɓj
	* @param begin �͈͂̏���
	* @param end �͈͂̏I���
	* @param comp ��r�Ɏg���t�@���N�^
	*/
	template<typename SortAlgorithm, typename BidirectionalIterator, typename Compare>
	constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = detail::comp_v<BidirectionalIterator>) {
		SortAlgorithm::sort(begin, end, std::forward<Compare>(comp));
	}

}