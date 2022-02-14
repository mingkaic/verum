#include "muta/random_generator.hpp"

#ifdef PKG_MUTA_RANDOM_GENERATOR_HPP

namespace muta
{

size_t RandomGenerator::string_length_ = 16;

const std::string RandomGenerator::char_set_ =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

}

#endif
