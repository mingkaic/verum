#include "verum/muta/igenerator.h"

#ifndef VERUM_MUTA_RANDOM_H
#define VERUM_MUTA_RANDOM_H

namespace verum
{

namespace muta
{

// source: https://ideone.com/3A3cv
template <typename IT>
IT fisher_yates_shuffle (IT begin, IT end, size_t m, iGenerator& generator)
{
	size_t left = std::distance(begin, end);
	assert(m < left);
	auto gen = generator.unif_intgen(0, left - 1);
	while (m--)
	{
		IT r = begin;
		size_t adv = gen() % left;
		std::advance(r, adv);
		std::swap(*begin, *r);
		++begin;
		--left;
	}
	return begin;
}

}

}

#endif // VERUM_MUTA_RANDOM_H
