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

#include "ListMonad.h"
#include "MaybeMonad.h"
#include "StateMonad.h"
#include "ContinuationMonad.h"

namespace Monadic
{

//#define MONAD_NDEBUG

struct Debug
{

	template<class MonadType, typename... Args>
	static void checkMonadLaw1(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadType::Return(2 * i); };
		{
			auto verfy1 = MonadType::Bind(MonadType::Return(123), apply1);
			auto verfy2 = apply1(123);
			(void)verfy1;
			(void)verfy2;
/*			auto v1 = verfy1.Eval(args...);
			auto v2 = verfy2.Eval(args...);
			(void)(v1);
			(void)(v2);

			static int b = 0;
			b++;//*/

			assert(verfy1(args...) == verfy2(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy1(args...)), decltype(verfy2(args...))>::value
				, "Monad Law1 not satisfied");
#endif
		}
		{
			auto verfy1 = MonadType::Do
			(
				LAZY(apply1(123))
			);
			
			int a1;
			auto verfy2 = MonadType::Do
			(
				a1 <<= LAZY(MonadType::Return(123)),
				LAZY(apply1(a1))
			);

			(void)verfy1;
			(void)verfy2;
/*			auto v1 = verfy1.Eval(args...);
			auto v2 = verfy2.Eval(args...);
			(void)(v1);
			(void)(v2);

			static int b = 0;
			b++;//*/
			
			assert(verfy1(args...) == verfy2(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy1(args...)), decltype(verfy2(args...))>::value
				, "Monad Law1 not satisfied");
#endif
		}
	}

	template<class MonadType, typename... Args>
	static void checkMonadLaw2(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadType::Return(i); };
		{
			auto verfy3 = MonadType::Bind(MonadType::Return(123), apply1);
			auto verfy4 = MonadType::Return(123);

			(void)verfy3;
			(void)verfy4;

			assert(verfy3(args...) == verfy4(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy3(args...)), decltype(verfy4(args...))>::value
				, "Monad Law2 not satisfied");
#endif
		}
		{
			auto mVal = MonadType::Return(123);

			auto verfy4 = MonadType::Do
			(
				LAZY(mVal)
			);

			int v1;
			auto verfy3 = MonadType::Do
			(
				v1 <<= LAZY(mVal),
				LAZY(apply1(v1))
			);

			(void)verfy3;
			(void)verfy4;

			assert(verfy3(args...) == verfy4(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy3(args...)), decltype(verfy4(args...))>::value
				, "Monad Law2 not satisfied");
#endif
		}
	}

	template<class MonadType, typename... Args>
	static void checkMonadLaw3(const Args&... args)
	{
		auto apply1 = [](int i) { return MonadType::Return(2 * i); };
		auto apply2 = [](int i) { return MonadType::Return(-i); };
		{
			auto verfy5 = MonadType::Bind(MonadType::Bind(MonadType::Return(123), apply1), apply2);
			auto verfy6 = MonadType::Bind(MonadType::Return(123), [=](auto x) { return MonadType::Bind(apply1(x), apply2); });
			(void)verfy5;
			(void)verfy6;
			assert(verfy5(args...) == verfy6(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy5(args...)), decltype(verfy6(args...))>::value
				, "Monad Law3 not satisfied");
#endif
		}
		{
			auto mVal = MonadType::Return(123);

			int v1, v2;
			auto verfy5 = MonadType::Do
			(
				v1 <<= LAZY(mVal),
				v2 <<= LAZY(apply1(v1)),
				LAZY(apply2(v2))
			);

			int v3, v4;
			auto verfy6 = MonadType::Do
			(
				v4 <<= LAZY(MonadType::Do
				(
					v3 <<= LAZY(mVal),
					LAZY(apply1(v3))
				)),
				LAZY(apply2(v4))
			);

			(void)verfy5;
			(void)verfy6;

			assert(verfy5(args...) == verfy6(args...));
#ifndef MONAD_NDEBUG
			static_assert(std::is_same<decltype(verfy5(args...)), decltype(verfy6(args...))>::value
				, "Monad Law3 not satisfied");
#endif
		}
	}

	template<class MonadType, typename... Args>
	static void checkMonadLaws(const Args&... args)
	{
		checkMonadLaw1<MonadType>(args...);
		checkMonadLaw2<MonadType>(args...);
		checkMonadLaw3<MonadType>(args...);
	}

}; //namespace Debug

}; //namespace Monadic