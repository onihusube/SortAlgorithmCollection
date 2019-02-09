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
			constexpr auto swap(Iterator lhs, Iterator rhs, Compare&& comp) -> bool {
				if (comp(*lhs, *rhs)) {
					std::swap(*lhs, *rhs);
					return true;
				}
				return false;
			}
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

				struct partitioning{};
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


		/**
		* @brief �o�u���\�[�g
		*/
		struct bubble_sort {
			static constexpr bool stable = true;

			using method = category::method::swap;


			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
				if (begin == end) return;

				//����ւ����s���Ȃ������񐔁i��납�猩�ĕ���ł���j���L�^
				int count{};

				//��납��A������count�����ւ����Ă��Ȃ���΁i=����ł���j�A���̕��������炷�i���ɍs���Ȃ�����j
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
		* @brief �V�F�C�J�[�\�[�g
		*/
		struct shaker_sort {
			static constexpr bool stable = true;

			using method = category::method::swap;
			
			template<typename BidirectionalIterator, typename Compare>
			static constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
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
						if (detail::swap(current + 1, current, comp) == true)last_swap = current;
					}

					if (far_left == last_swap) return;

					//�E�[���߂�
					far_right = last_swap;

					//������E
					for (auto current = far_right; current != far_left; --current) {
						if (detail::swap(current, current - 1, comp) == true)last_swap = current;
					}

					//���[���߂�
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

		sort<SortAlgorithm>(container, comp_v<iterator>);
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
	constexpr void sort(BidirectionalIterator begin, BidirectionalIterator end, Compare&& comp = comp_v<BidirectionalIterator>) {
		SortAlgorithm::sort(begin, end, std::forward<Compare>(comp));
	}

}