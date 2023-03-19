///
/// macros.hpp
/// exam
///
/// Purpose:
/// Define test utility macros
///

#ifndef VERUM_EXAM_MACROS_HPP
#define VERUM_EXAM_MACROS_HPP

#include <limits>
#include <type_traits>

#include "gtest/gtest.h"

#include "cisab/fmts/format.h"

#include "cisab/types/iterator.h"
#include "cisab/types/math.h"

#include "verum/exam/mock_log.h"
#include "verum/exam/nosupport_log.h"

namespace verum
{

namespace exam
{

struct TestException final : public std::exception
{
	TestException (const std::string& msg) : msg_(msg) {}

	const char* what () const throw ()
	{
		return msg_.c_str();
	}

	std::string msg_;
};

template <typename T>
inline bool canmatch (const T& l, const T& r)
{
	if (std::isinf(l) && std::isinf(r))
	{
		return false;
	}
	if (std::isnan(l) && std::isnan(r))
	{
		return false;
	}
	return true;
}

template <typename T>
inline T absolute_error (const T& l, const T& r)
{
	return std::abs(l - r);
}

template <typename T, typename std::enable_if<!cisab::types::is_complex<T>::value>::type* = nullptr>
inline T relative_error (const T& l, const T& r)
{
	return std::abs(l - r) / (
		std::max(std::abs(l), std::abs(r)) +
		std::numeric_limits<T>::epsilon()
	);
}

template <typename T>
inline T relative_error (const std::complex<T>& l, const std::complex<T>& r)
{
	std::complex<T> out = std::abs(l - r) / (
		std::max(std::abs(l), std::abs(r)) +
		std::numeric_limits<T>::epsilon()
	);
	return out.real();
}

template <typename T>
using ClosenessF = std::function<bool(const T&,const T&)>;

template <typename T>
inline ClosenessF<T> abs_close (T epsilon)
{
	return [epsilon](const T& l, const T& r)
	{
		if (canmatch(l, r))
		{
			return absolute_error(l, r) <= epsilon;
		}
		return true;
	};
}

template <typename T>
inline ClosenessF<T> rel_close (T thresh)
{
	return [thresh](const T& l, const T& r)
	{
		if (canmatch(l, r))
		{
			return relative_error(l, r) <= thresh;
		}
		return true;
	};
}

// takes the best of absolute and relative error
template <typename T>
inline ClosenessF<T> bestcase_close (T thresh)
{
	return [thresh](const T& l, const T& r)
	{
		if (canmatch(l, r))
		{
			return std::min(relative_error(l, r), absolute_error(l, r)) <= thresh;
		}
		return true;
	};
}

template <typename IT>
inline bool vector_close (IT lbegin, IT lend, IT rbegin,
	const ClosenessF<cisab::types::IterValT<IT>>& close_fnc)
{
	bool close = true;
	for (; lbegin != lend && close; ++lbegin, ++rbegin)
	{
		close = close_fnc(*lbegin, *rbegin);
	}
	return close;
}

#define _ARRCHECK(ARR, ARR2, GBOOL) {\
	GBOOL(std::equal(ARR.begin(), ARR.end(), ARR2.begin())) <<\
		"expect list " << cisab::fmts::readable(ARR) <<\
		", got " << cisab::fmts::readable(ARR2) << " instead"; }
#define _VECCHECK(VEC, VEC2, GBOOL) {\
	GBOOL(VEC.size() == VEC2.size() &&\
		std::equal(VEC.begin(), VEC.end(), VEC2.begin())) <<\
		"expect list " << cisab::fmts::readable(VEC) <<\
		", got " << cisab::fmts::readable(VEC2) << " instead"; }
#define _ARRCLOSE(ARR, ARR2, EPS, GBOOL) {\
	GBOOL(::verum::exam::vector_close(ARR.begin(), ARR.end(), ARR2.begin(), ::verum::exam::abs_close(EPS))) <<\
		"expect list " << cisab::fmts::readable(ARR) <<\
		", got " << cisab::fmts::readable(ARR2) << " instead"; }
#define _VECCLOSE(VEC, VEC2, EPS, GBOOL) {\
	GBOOL(VEC.size() == VEC2.size() &&\
		::verum::exam::vector_close(VEC.begin(), VEC.end(), VEC2.begin(), ::verum::exam::abs_close(EPS))) <<\
		"expect list " << cisab::fmts::readable(VEC) <<\
		", got " << cisab::fmts::readable(VEC2) << " instead"; }
#define _INSET(SET, CONTENT, GBOOL, PREFIX_MSG) {\
	GBOOL(SET.end() != SET.find(CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #SET; }
#define _INARR(ARR, CONTENT, GBOOL, PREFIX_MSG) { auto et = ARR.end();\
	GBOOL(et != std::find(ARR.begin(), et, CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #ARR; }

#define EXPECT_STRHAS(SUPER, SUB) EXPECT_THAT(SUPER.c_str(), ::testing::HasSubstr(SUB))
#define EXPECT_STRHASNOT(SUPER, SUB) EXPECT_THAT(SUPER.c_str(), Not(::testing::HasSubstr(SUB)))

#define ASSERT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_TRUE)
#define EXPECT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_TRUE)
#define ASSERT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_FALSE)
#define EXPECT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_FALSE)

#define ASSERT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_TRUE)
#define EXPECT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_TRUE)
#define ASSERT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_FALSE)
#define EXPECT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_FALSE)

#define ASSERT_ARRCLOSE(ARR, ARR2, EPS) _ARRCLOSE(ARR, ARR2, EPS, ASSERT_TRUE)
#define EXPECT_ARRCLOSE(ARR, ARR2, EPS) _ARRCLOSE(ARR, ARR2, EPS, EXPECT_TRUE)
#define ASSERT_ARRNCLOSE(ARR, ARR2, EPS) _ARRCLOSE(ARR, ARR2, EPS, ASSERT_FALSE)
#define EXPECT_ARRNCLOSE(ARR, ARR2, EPS) _ARRCLOSE(ARR, ARR2, EPS, EXPECT_FALSE)

#define ASSERT_VECCLOSE(VEC, VEC2, EPS) _VECCLOSE(VEC, VEC2, EPS, ASSERT_TRUE)
#define EXPECT_VECCLOSE(VEC, VEC2, EPS) _VECCLOSE(VEC, VEC2, EPS, EXPECT_TRUE)
#define ASSERT_VECNCLOSE(VEC, VEC2, EPS) _VECCLOSE(VEC, VEC2, EPS, ASSERT_FALSE)
#define EXPECT_VECNCLOSE(VEC, VEC2, EPS) _VECCLOSE(VEC, VEC2, EPS, EXPECT_FALSE)

#define ASSERT_HAS(SET, KEY) _INSET(SET, KEY, ASSERT_TRUE, "expecting to")
#define EXPECT_HAS(SET, KEY) _INSET(SET, KEY, EXPECT_TRUE, "expecting to")
#define ASSERT_HASNOT(SET, KEY) _INSET(SET, KEY, ASSERT_FALSE, "cannot")
#define EXPECT_HASNOT(SET, KEY) _INSET(SET, KEY, EXPECT_FALSE, "cannot")

#define ASSERT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_TRUE, "expecting to")
#define EXPECT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_TRUE, "expecting to")
#define ASSERT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_FALSE, "cannot")
#define EXPECT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_FALSE, "cannot")

#define EXPECT_FATAL(EVENT, MSG) try { EVENT; FAIL() <<\
	"did not expect " << #EVENT << " to succeed"; }\
	catch (::verum::exam::TestException& e) { EXPECT_STREQ(MSG, e.what()); }\
	catch (std::exception& e) { FAIL() << "unexpected throw " << e.what(); }

#define EXPECT_REL_CLOSE(L, R, ERR) EXPECT_LT(::verum::exam::relative_error(L, R), ERR)

#define EXPECT_NOT_REL_CLOSE(L, R, ERR) EXPECT_GE(::verum::exam::relative_error(L, R), ERR)

#define EXPECT_CLOSE(L, R) EXPECT_REL_CLOSE(L, R, 0.02)

#define EXPECT_NOT_CLOSE(L, R) EXPECT_NOT_REL_CLOSE(L, R, 0.02)

}

}

#endif // VERUM_EXAM_HPP
