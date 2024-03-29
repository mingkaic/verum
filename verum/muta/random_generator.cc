#include "verum/muta/random_generator.h"

#ifdef VERUM_MUTA_RANDOM_GENERATOR_H

namespace verum
{

namespace muta
{

size_t RandomGenerator::string_length_ = 16;

const std::string RandomGenerator::char_set_ =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

}

}

#endif
