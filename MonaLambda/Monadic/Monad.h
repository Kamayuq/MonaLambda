/*
//
// Copyright(c) 2016, Arne Schober
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// *Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and / or other materials provided with the distribution.
// * Neither the name of the <organization> nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL ARNE SCHOBER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*/


#pragma once
#include <assert.h>

#include "DoNotation.h"
#include "ListMonad.h"
#include "MaybeMonad.h"
#include "StateMonad.h"
#include "ContinuationMonad.h"

namespace Monadic
{

//#define MONAD_NDEBUG

struct Debug
{

	template<class MonadUtil, typename... Args>
	static void checkMonadLaw1(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadUtil::Return(2 * i); };
		{
			auto verfy1 = (MonadUtil::Return(123) >= apply1);
			auto verfy2 = apply1(123);
			(void)verfy1;
			(void)verfy2;
/*			auto v1 = verfy1.Eval(args...);
			auto v2 = verfy2.Eval(args...);
			(void)(v1);
			(void)(v2);

			static int b = 0;
			b++;//*/

			assert(verfy1.Eval(args...) == verfy2.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy1.Eval(args...)), decltype(verfy2.Eval(args...))>::value
				, "Monad Law1 not satisfied");
#endif
		}
		{
			auto verfy1 = DO
			(
				apply1(123)
			);
			
			int a1;
			auto verfy2 = DO
			(
				a1 <<= MonadUtil::Return(123),
				apply1(a1)
			);

			(void)verfy1;
			(void)verfy2;
/*			auto v1 = verfy1.Eval(args...);
			auto v2 = verfy2.Eval(args...);
			(void)(v1);
			(void)(v2);

			static int b = 0;
			b++;//*/
			
			assert(verfy1.Eval(args...) == verfy2.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy1.Eval(args...)), decltype(verfy2.Eval(args...))>::value
				, "Monad Law1 not satisfied");
#endif
		}
	}

	template<class MonadUtil, typename... Args>
	static void checkMonadLaw2(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadUtil::Return(i); };
		{
			auto verfy3 = (MonadUtil::Return(123) >= apply1);
			auto verfy4 = MonadUtil::Return(123);

			(void)verfy3;
			(void)verfy4;

			assert(verfy3.Eval(args...) == verfy4.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy3.Eval(args...)), decltype(verfy4.Eval(args...))>::value
				, "Monad Law2 not satisfied");
#endif
		}
		{
			auto mVal = MonadUtil::Return(123);

			auto verfy4 = DO
			(
				mVal
			);

			int v1;
			auto verfy3 = DO
			(
				v1 <<= mVal,
				apply1(v1)
			);

			(void)verfy3;
			(void)verfy4;

			assert(verfy3.Eval(args...) == verfy4.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy3.Eval(args...)), decltype(verfy4.Eval(args...))>::value
				, "Monad Law2 not satisfied");
#endif
		}
	}

	template<class MonadUtil, typename... Args>
	static void checkMonadLaw3(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadUtil::Return(2 * i); };
		auto apply2 = [](int i) { return MonadUtil::Return(-i); };
		{
			auto verfy5 = MonadUtil::Return(123) >= apply1 >= apply2;
			auto verfy6 = MonadUtil::Return(123) >= [=](auto x) { return apply1(x) >= apply2; };
			(void)verfy5;
			(void)verfy6;
			assert(verfy5.Eval(args...) == verfy6.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy5.Eval(args...)), decltype(verfy6.Eval(args...))>::value
				, "Monad Law3 not satisfied");
#endif
		}
		{
			auto mVal = MonadUtil::Return(123);

			int v1, v2;
			auto verfy5 = DO
			(
				v1 <<= mVal,
				v2 <<= apply1(v1),
				apply2(v2)
			);

			int v3, v4;
			auto verfy6 = DO
			(
				v4 <<= DO
				(
					v3 <<= mVal,
					apply1(v3)
				),
				apply2(v4)
			);

			(void)verfy5;
			(void)verfy6;

			assert(verfy5.Eval(args...) == verfy6.Eval(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy5.Eval(args...)), decltype(verfy6.Eval(args...))>::value
				, "Monad Law3 not satisfied");
#endif
		}
	}

	template<class MonadUtil, typename... Args>
	static void checkMonadLaws(const Args&... args)
	{
		checkMonadLaw1<MonadUtil>(args...);
		checkMonadLaw2<MonadUtil>(args...);
		checkMonadLaw3<MonadUtil>(args...);
	}

}; //namespace Debug

}; //namespace Monadic