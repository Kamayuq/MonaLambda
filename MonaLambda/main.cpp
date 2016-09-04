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
#include "Monadic/Monad.h"

int main(void)
{
	{
		using namespace Monadic;

		int vala = 0;
		auto mRes1 = DO
		(
			vala <<= ListM::Return(1337),
			ListM::Return(1),
			ListM::Return(2),
			ListM::Return(3),
			ListM::Return(4)
			//valb <<= Maybe::Return(3),
		);

		//bool leq = List::Make(0, 1,'c',3) == List::Make(0, 1,'c',3);

		auto res = mRes1.Eval();
		(void)res;

		Debug::checkMonadLaws<ListM>();

		std::cout << "ListMonad passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		auto mRes1 = DO
		(
			ListT<Maybe>::ReturnM(DO(
				Maybe::Return(3)
				//Maybe::FailWith("error2")
			)),
			vala <<= ListT<Maybe>::Return(1337)
			//valb <<= Maybe::Return(3),
		);

		//bool leq = List::Make(0, 1, 'c', 3) == List::Make(0, 1, 'c', 3);

		auto res = mRes1.Eval();
		(void)res;

		Debug::checkMonadLaws<ListT<Maybe>>();
		Debug::checkMonadLaws<ListT<State>>(3);
		Debug::checkMonadLaws<ListT<Cont>>();

		std::cout << "ListMonadTransformer passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		int valb = 0;
		int valc = 0;

		auto add = [](auto a, auto b){ return  a + b; };
		auto addM = LiftMonad<decltype(add)(int, int)>::liftM<Maybe>(add);

		auto mRes1 = DO
		(
			vala <<= Maybe::Return(1337),
			valb <<= Maybe::Return(3),
			//Maybe::FailWith<int>("Error"),
			valc <<= addM(vala, valb),
			Maybe::Return(valc + valb)
		);

		auto res = mRes1.Eval();
		(void)res;
		
		Debug::checkMonadLaws<Maybe>();
		
		std::cout << "MaybeMonad passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		//int vala = 0;
		//int valb = 0;
		//int valc = 0;

		//auto add = [](auto a, auto b) { return  a + b; };
		//auto addM = LiftMonad<decltype(add)(int, int)>::liftM<Maybe>(add);

		auto mRes1 = DO
		(
			MaybeT<Cont>::ReturnM(DO(
				Cont::Return(3)
				//Maybe::FailWith("error2")
			))
			//MaybeT<Cont>::FailWith("error1")
			//MaybeT<Cont>::Return(2)
			//vala <<= Maybe::Return(1337),
			//valb <<= Maybe::Return(3)
			//Maybe::FailWith<int>("Error"),
			//valc <<= addM(vala, valb),
			//Maybe::Return(valc + valb)
		);

		auto res = mRes1.Eval();
		(void)res;
		
		Debug::checkMonadLaws<MaybeT<Maybe>>();
		Debug::checkMonadLaws<MaybeT<State>>(3);
		Debug::checkMonadLaws<MaybeT<Cont>>();

		std::cout << "MaybeMonadTransformer passed" << std::endl;
	}//*/

	{
		using namespace Monadic;

		int vala = 0;
		char valb = 0;

		auto mRes1 = DO
		(
			vala <<= Cont::Return(1337),
			valb <<= Cont::Return('c'),
			Cont::Return(std::make_pair(vala, valb))
		);

		auto res = mRes1.Eval();
		(void)res;

		Debug::checkMonadLaws<Cont>();

		std::cout << "ContinuationMonad passed" << std::endl;
	} //*/

	{
		using namespace Monadic;

		//int vala = 0;
		//char valb = 0;

		auto mRes1 = DO
		(
			ContT<State>::ReturnM(DO(
				State::Return(3)
				//Maybe::FailWith("error2")
			))
			//vala <<= Cont::Return(1337),
			//valb <<= Cont::Return('c'),
			//Cont::Return(std::make_pair(vala, valb))
		);

		auto res = mRes1.Eval(2);
		(void)res;
		
		Debug::checkMonadLaws<ContT<Maybe>>();
		Debug::checkMonadLaws<ContT<State>>(3);
		Debug::checkMonadLaws<ContT<Cont>>();

		std::cout << "ContinuationMonadTransformer passed" <<std::endl;
	}

	{
		using namespace Monadic;

		int vala = 0;
		char valb = 0;
		int vals = 0;
		
		auto mRes1 = DO
		(
			vala <<= State::Return(1337),
			State::Modify([](auto s){ return s * 2; }),
			vals <<= State::Get<int>(),
			State::Put(vals + vala),
			valb <<= State::Return('b'),
			State::Return(std::make_pair(vala, valb))
		);

		const int state = 3;
		auto res = mRes1.Eval(state);
		(void)res;

		Debug::checkMonadLaws<State>(state);

		std::cout << "StateMonad passed" << std::endl;
	}

	{
		using namespace Monadic;
		
		int vala = 0;
		char valb = 0;
		int vals = 0;
		
		auto mRes1 = DO
		(		
			StateT<Maybe>::ReturnM(DO
			(
				vala <<= Maybe::Return(1337)
				//Maybe::FailWith("Error2")
			)),
			StateT<Maybe>::Return(1337),
			vals <<= StateT<Maybe>::Get<int>(),
			StateT<Maybe>::Put(vala),
			StateT<Maybe>::ReturnM(DO
			(
				valb <<= Maybe::Return('c')
				//Maybe::FailWith("Error1")
			)),
			StateT<Maybe>::Modify([](auto s){ return s * 2; }),
			vals <<= StateT<Maybe>::Get<int>(),
			StateT<Maybe>::Put(valb),
			StateT<Maybe>::Return(std::make_pair(vals, valb))
		);

		const int state = 3;
		auto res = mRes1.Eval(state);
		(void)res;
		
		Debug::checkMonadLaws<StateT<Maybe>>(state);
		Debug::checkMonadLaws<StateT<State>>(state, state);
		Debug::checkMonadLaws<StateT<Cont>>(state);

		std::cout << "StateMonadTransformer passed" << std::endl;
	}//*/

	std::cin.get();
	return 0;
}