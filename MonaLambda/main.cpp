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

#include <iostream>
#include <utility>
#include <type_traits>
#include "Monadic/Monad.h"

using namespace Monadic;

int main(void)
{
	{
		using namespace Monadic;

		int vala = 0;
		auto mRes1 = Do
		(
			vala <<= LAZY(ListM::Return(1337)),
			LAZY(ListM::Return(1)),
			LAZY(ListM::Return(2)),
			LAZY(ListM::Return(3)),
			LAZY(ListM::Return(4))
			//valb <<= Maybe::Return(3),
		);

		//bool leq = List::Make(0, 1,'c',3) == List::Make(0, 1,'c',3);

		auto res = mRes1;
		(void)res;

		//Debug::checkMonadLaws<ListM>();

		std::cout << "ListMonad passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		auto mRes1 = Do
		(
			LAZY(ListT<Maybe>::ReturnM(Do(
				LAZY(Maybe::Return(3))
				//LAZY(Maybe::FailWith("error2"))
			))),
			vala <<= LAZY(ListT<Maybe>::Return(1337))
			//valb <<= LAZY(Maybe::Return(3)),
		);

		//bool leq = List::Make(0, 1, 'c', 3) == List::Make(0, 1, 'c', 3);

		auto res = mRes1;
		(void)res;

		//Debug::checkMonadLaws<ListT<Maybe>>();
		//Debug::checkMonadLaws<ListT<State>>(3);
		//Debug::checkMonadLaws<ListT<Cont>>([](auto a) { return a; });

		std::cout << "ListMonadTransformer passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		int valb = 0;
		int valc = 0;

		auto add = [](auto a, auto b){ return  a + b; };
		auto addM = LiftMonad<decltype(add)(int, int)>::liftM<Maybe>(add);

		auto mRes1 = Do
		(
			vala <<= LAZY(Maybe::Return(1337)),
			valb <<= LAZY(Maybe::Return(3)),
			//LAZY(Maybe::FailWith<int>("Error")),
			valc <<= LAZY(addM(vala, valb)),
			LAZY(Maybe::Return(valc + valb))
		);

		auto res = mRes1;
		(void)res;
		
		//Debug::checkMonadLaws<Maybe>();
		
		std::cout << "MaybeMonad passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		int valb = 0;
		int valc = 0;

		auto add = [](auto a, auto b) { return  a + b; };
		auto addM = LiftMonad<decltype(add)(int, int)>::liftM<MaybeT<Cont>>(add);

		auto mRes1 = Do
		(
			LAZY(MaybeT<Cont>::ReturnM(Do(
				LAZY(Cont::Return(3))
			))),
			LAZY(MaybeT<Cont>::FailWith("error1")),
			LAZY(MaybeT<Cont>::Return(2)),
			vala <<= LAZY(MaybeT<Cont>::Return(1337)),
			valb <<= LAZY(MaybeT<Cont>::Return(3)),
			LAZY(MaybeT<Cont>::FailWith<int>("Error")),
			valc <<= LAZY(addM(vala, valb)),
			LAZY(MaybeT<Cont>::Return(valc + valb))
		);

		auto res = mRes1([](auto a) {return a;});
		(void)res;
		
		//Debug::checkMonadLaws<MaybeT<Maybe>>();
		//Debug::checkMonadLaws<MaybeT<State>>(3);
		//Debug::checkMonadLaws<MaybeT<Cont>>();

		std::cout << "MaybeMonadTransformer passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		char valb = 0;
		float valc = 0;

		auto mRes1 = Do
		(
			vala <<= LAZY(Do
			(
				valc <<= LAZY(Cont::Return(1337.0f)),
				LAZY(Cont::Return((int)valc + 1))
			)),
			valb <<= LAZY(Cont::Return('c')),
			LAZY(Cont::Return(std::make_pair(vala, valb)))
		);

		auto res = mRes1([](auto a) { return a; });
		(void)res;

		static int global = 5;
		int v = 0;
		auto cc = Cont::CallCC([&](auto k)
		{
			return Do
			(
				v <<= LAZY(Cont::Return(global)),
				LAZY(k((v == 5) ? v : 1337))
				//If(LAZY(v == 5), LAZY(Do(LAZY(k(v)))), LAZY(Do(LAZY(Cont::Return(1337)))))
			);
		});

		auto r = cc ([](auto a) 
		{ 
			return (2 * a); 
		});
		//auto r2 = r([](auto a) { return a; });

		Debug::checkMonadLaws<Cont>([](auto a) { return a; });

		std::cout << "ContinuationMonad passed" << std::endl;
	} //*/

	{
		using namespace Monadic;

		int vala = 0;
		char valb = 0;

		auto mRes1 = Do
		(
			LAZY(ContT<State>::ReturnM(Do(
				LAZY(State::Return(3))
			))),
			vala <<= LAZY(ContT<State>::Return(1337)),
			valb <<= LAZY(ContT<State>::Return('c')),
			LAZY(ContT<State>::Return(std::make_pair(vala, valb)))
		);

		auto res = mRes1([](auto a) { return a; })(2);
		(void)res;
		
		//Debug::checkMonadLaws<ContT<Maybe>>();
		//Debug::checkMonadLaws<ContT<State>>([](auto a) { return a; });
		//Debug::checkMonadLaws<ContT<Cont>>([](auto a) { return a; });

		std::cout << "ContinuationMonadTransformer passed" <<std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		char valb = 0;
		int vals = 0;
		
		auto mRes1 = Do
		(
			vala <<= LAZY(State::Return(1337)),
			LAZY(State::Modify([](auto s){ return s * 2; })),
			vals <<= LAZY(State::Get()),
			LAZY(State::Put(vals + vala)),
			valb <<= LAZY(State::Return('b')),
			LAZY(State::Return(std::make_pair(vala, valb)))
		);

		const int state = 3;
		auto res = mRes1(state);
		(void)res;

		Debug::checkMonadLaws<State>(state);

		std::cout << "StateMonad passed" << std::endl;
	}

	{
		using namespace Monadic;
		
		int vala = 0;
		char valb = 0;
		int vals = 0;
		
		auto mRes1 = Do
		(		
			LAZY(StateT<Maybe>::ReturnM(Do
			(
				vala <<= LAZY(Maybe::Return(1337))
				//Maybe::FailWith("Error2")
			))),
			LAZY(StateT<Maybe>::Return(1337)),
			vals <<= LAZY(StateT<Maybe>::Get()),
			LAZY(StateT<Maybe>::Put(vala)),
			LAZY(StateT<Maybe>::ReturnM(Do
			(
				valb <<= LAZY(Maybe::Return('c'))
				//Maybe::FailWith("Error1")
			))),
			LAZY(StateT<Maybe>::Modify([](auto s){ return s * 2; })),
			vals <<= LAZY(StateT<Maybe>::Get()),
			LAZY(StateT<Maybe>::Put(valb)),
			LAZY(StateT<Maybe>::Return(std::make_pair(vals, valb)))
		);

		const int state = 3;
		auto res = mRes1(state);
		(void)res;
		
		//Debug::checkMonadLaws<StateT<Maybe>>(state);
		//Debug::checkMonadLaws<StateT<State>>(state, state);
		//Debug::checkMonadLaws<StateT<Cont>>(state);

		std::cout << "StateMonadTransformer passed" << std::endl;
	}//*/

	std::cin.get();
	return 0;
}