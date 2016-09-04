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

#define EXPAND(...) __VA_ARGS__

#define VA_NARGS_IMPL( _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
					  _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
					  _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
					  _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
					  _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
					  _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
					  _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
					  _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
					  _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
					  _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
			   N, ...) N

#define VA_NARGS(...) EXPAND(VA_NARGS_IMPL(__VA_ARGS__,				 \
									 100,99,98,97,96,95,94,93,92,91, \
									  90,89,88,87,86,85,84,83,82,81, \
									  80,79,78,77,76,75,74,73,72,71, \
									  70,69,68,67,66,65,64,63,62,61, \
									  60,59,58,57,56,55,54,53,52,51, \
									  50,49,48,47,46,45,44,43,42,41, \
									  40,39,38,37,36,35,34,33,32,31, \
									  30,29,28,27,26,25,24,23,22,21, \
									  20,19,18,17,16,15,14,13,12,11, \
									  10, 9, 8, 7, 6, 5, 4, 3, 2, 1 ))

#define DO_IMPL2(count, ...) EXPAND(DO ## count (__VA_ARGS__))
#define DO_IMPL(count, ...) EXPAND(DO_IMPL2(count, __VA_ARGS__)) 

#define  DO1(e)      (e).Bind([&](auto){ return e; })
#define  DO2(e, ...) (e).Bind([&](auto){ return EXPAND( DO1(__VA_ARGS__)); })
#define  DO3(e, ...) (e).Bind([&](auto){ return EXPAND( DO2(__VA_ARGS__)); })
#define  DO4(e, ...) (e).Bind([&](auto){ return EXPAND( DO3(__VA_ARGS__)); })
#define  DO5(e, ...) (e).Bind([&](auto){ return EXPAND( DO4(__VA_ARGS__)); })
#define  DO6(e, ...) (e).Bind([&](auto){ return EXPAND( DO5(__VA_ARGS__)); })
#define  DO7(e, ...) (e).Bind([&](auto){ return EXPAND( DO6(__VA_ARGS__)); })
#define  DO8(e, ...) (e).Bind([&](auto){ return EXPAND( DO7(__VA_ARGS__)); })
#define  DO9(e, ...) (e).Bind([&](auto){ return EXPAND( DO8(__VA_ARGS__)); })

#define DO10(e, ...) (e).Bind([&](auto){ return EXPAND( DO9(__VA_ARGS__)); })
#define DO11(e, ...) (e).Bind([&](auto){ return EXPAND(DO10(__VA_ARGS__)); })
#define DO12(e, ...) (e).Bind([&](auto){ return EXPAND(DO11(__VA_ARGS__)); })
#define DO13(e, ...) (e).Bind([&](auto){ return EXPAND(DO12(__VA_ARGS__)); })
#define DO14(e, ...) (e).Bind([&](auto){ return EXPAND(DO13(__VA_ARGS__)); })
#define DO15(e, ...) (e).Bind([&](auto){ return EXPAND(DO14(__VA_ARGS__)); })
#define DO16(e, ...) (e).Bind([&](auto){ return EXPAND(DO15(__VA_ARGS__)); })
#define DO17(e, ...) (e).Bind([&](auto){ return EXPAND(DO16(__VA_ARGS__)); })
#define DO18(e, ...) (e).Bind([&](auto){ return EXPAND(DO17(__VA_ARGS__)); })
#define DO19(e, ...) (e).Bind([&](auto){ return EXPAND(DO18(__VA_ARGS__)); })

#define DO20(e, ...) (e).Bind([&](auto){ return EXPAND(DO19(__VA_ARGS__)); })
#define DO21(e, ...) (e).Bind([&](auto){ return EXPAND(DO20(__VA_ARGS__)); })
#define DO22(e, ...) (e).Bind([&](auto){ return EXPAND(DO21(__VA_ARGS__)); })
#define DO23(e, ...) (e).Bind([&](auto){ return EXPAND(DO22(__VA_ARGS__)); })
#define DO24(e, ...) (e).Bind([&](auto){ return EXPAND(DO23(__VA_ARGS__)); })
#define DO25(e, ...) (e).Bind([&](auto){ return EXPAND(DO24(__VA_ARGS__)); })
#define DO26(e, ...) (e).Bind([&](auto){ return EXPAND(DO25(__VA_ARGS__)); })
#define DO27(e, ...) (e).Bind([&](auto){ return EXPAND(DO26(__VA_ARGS__)); })
#define DO28(e, ...) (e).Bind([&](auto){ return EXPAND(DO27(__VA_ARGS__)); })
#define DO29(e, ...) (e).Bind([&](auto){ return EXPAND(DO28(__VA_ARGS__)); })

#define DO30(e, ...) (e).Bind([&](auto){ return EXPAND(DO29(__VA_ARGS__)); })
#define DO31(e, ...) (e).Bind([&](auto){ return EXPAND(DO30(__VA_ARGS__)); })
#define DO32(e, ...) (e).Bind([&](auto){ return EXPAND(DO31(__VA_ARGS__)); })
#define DO33(e, ...) (e).Bind([&](auto){ return EXPAND(DO32(__VA_ARGS__)); })
#define DO34(e, ...) (e).Bind([&](auto){ return EXPAND(DO33(__VA_ARGS__)); })
#define DO35(e, ...) (e).Bind([&](auto){ return EXPAND(DO34(__VA_ARGS__)); })
#define DO36(e, ...) (e).Bind([&](auto){ return EXPAND(DO35(__VA_ARGS__)); })
#define DO37(e, ...) (e).Bind([&](auto){ return EXPAND(DO36(__VA_ARGS__)); })
#define DO38(e, ...) (e).Bind([&](auto){ return EXPAND(DO37(__VA_ARGS__)); })
#define DO39(e, ...) (e).Bind([&](auto){ return EXPAND(DO38(__VA_ARGS__)); })

#define DO40(e, ...) (e).Bind([&](auto){ return EXPAND(DO39(__VA_ARGS__)); })
#define DO41(e, ...) (e).Bind([&](auto){ return EXPAND(DO40(__VA_ARGS__)); })
#define DO42(e, ...) (e).Bind([&](auto){ return EXPAND(DO41(__VA_ARGS__)); })
#define DO43(e, ...) (e).Bind([&](auto){ return EXPAND(DO42(__VA_ARGS__)); })
#define DO44(e, ...) (e).Bind([&](auto){ return EXPAND(DO43(__VA_ARGS__)); })
#define DO45(e, ...) (e).Bind([&](auto){ return EXPAND(DO44(__VA_ARGS__)); })
#define DO46(e, ...) (e).Bind([&](auto){ return EXPAND(DO45(__VA_ARGS__)); })
#define DO47(e, ...) (e).Bind([&](auto){ return EXPAND(DO46(__VA_ARGS__)); })
#define DO48(e, ...) (e).Bind([&](auto){ return EXPAND(DO47(__VA_ARGS__)); })
#define DO49(e, ...) (e).Bind([&](auto){ return EXPAND(DO48(__VA_ARGS__)); })

#define DO50(e, ...) (e).Bind([&](auto){ return EXPAND(DO49(__VA_ARGS__)); })
#define DO51(e, ...) (e).Bind([&](auto){ return EXPAND(DO50(__VA_ARGS__)); })
#define DO52(e, ...) (e).Bind([&](auto){ return EXPAND(DO51(__VA_ARGS__)); })
#define DO53(e, ...) (e).Bind([&](auto){ return EXPAND(DO52(__VA_ARGS__)); })
#define DO54(e, ...) (e).Bind([&](auto){ return EXPAND(DO53(__VA_ARGS__)); })
#define DO55(e, ...) (e).Bind([&](auto){ return EXPAND(DO54(__VA_ARGS__)); })
#define DO56(e, ...) (e).Bind([&](auto){ return EXPAND(DO55(__VA_ARGS__)); })
#define DO57(e, ...) (e).Bind([&](auto){ return EXPAND(DO56(__VA_ARGS__)); })
#define DO58(e, ...) (e).Bind([&](auto){ return EXPAND(DO57(__VA_ARGS__)); })
#define DO59(e, ...) (e).Bind([&](auto){ return EXPAND(DO58(__VA_ARGS__)); })

#define DO60(e, ...) (e).Bind([&](auto){ return EXPAND(DO59(__VA_ARGS__)); })
#define DO61(e, ...) (e).Bind([&](auto){ return EXPAND(DO60(__VA_ARGS__)); })
#define DO62(e, ...) (e).Bind([&](auto){ return EXPAND(DO61(__VA_ARGS__)); })
#define DO63(e, ...) (e).Bind([&](auto){ return EXPAND(DO62(__VA_ARGS__)); })
#define DO64(e, ...) (e).Bind([&](auto){ return EXPAND(DO63(__VA_ARGS__)); })
#define DO65(e, ...) (e).Bind([&](auto){ return EXPAND(DO64(__VA_ARGS__)); })
#define DO66(e, ...) (e).Bind([&](auto){ return EXPAND(DO65(__VA_ARGS__)); })
#define DO67(e, ...) (e).Bind([&](auto){ return EXPAND(DO66(__VA_ARGS__)); })
#define DO68(e, ...) (e).Bind([&](auto){ return EXPAND(DO67(__VA_ARGS__)); })
#define DO69(e, ...) (e).Bind([&](auto){ return EXPAND(DO68(__VA_ARGS__)); })

#define DO70(e, ...) (e).Bind([&](auto){ return EXPAND(DO69(__VA_ARGS__)); })
#define DO71(e, ...) (e).Bind([&](auto){ return EXPAND(DO70(__VA_ARGS__)); })
#define DO72(e, ...) (e).Bind([&](auto){ return EXPAND(DO71(__VA_ARGS__)); })
#define DO73(e, ...) (e).Bind([&](auto){ return EXPAND(DO72(__VA_ARGS__)); })
#define DO74(e, ...) (e).Bind([&](auto){ return EXPAND(DO73(__VA_ARGS__)); })
#define DO75(e, ...) (e).Bind([&](auto){ return EXPAND(DO74(__VA_ARGS__)); })
#define DO76(e, ...) (e).Bind([&](auto){ return EXPAND(DO75(__VA_ARGS__)); })
#define DO77(e, ...) (e).Bind([&](auto){ return EXPAND(DO76(__VA_ARGS__)); })
#define DO78(e, ...) (e).Bind([&](auto){ return EXPAND(DO77(__VA_ARGS__)); })
#define DO79(e, ...) (e).Bind([&](auto){ return EXPAND(DO78(__VA_ARGS__)); })

#define DO80(e, ...) (e).Bind([&](auto){ return EXPAND(DO79(__VA_ARGS__)); })
#define DO81(e, ...) (e).Bind([&](auto){ return EXPAND(DO80(__VA_ARGS__)); })
#define DO82(e, ...) (e).Bind([&](auto){ return EXPAND(DO81(__VA_ARGS__)); })
#define DO83(e, ...) (e).Bind([&](auto){ return EXPAND(DO82(__VA_ARGS__)); })
#define DO84(e, ...) (e).Bind([&](auto){ return EXPAND(DO83(__VA_ARGS__)); })
#define DO85(e, ...) (e).Bind([&](auto){ return EXPAND(DO84(__VA_ARGS__)); })
#define DO86(e, ...) (e).Bind([&](auto){ return EXPAND(DO85(__VA_ARGS__)); })
#define DO87(e, ...) (e).Bind([&](auto){ return EXPAND(DO86(__VA_ARGS__)); })
#define DO88(e, ...) (e).Bind([&](auto){ return EXPAND(DO87(__VA_ARGS__)); })
#define DO89(e, ...) (e).Bind([&](auto){ return EXPAND(DO88(__VA_ARGS__)); })

#define DO90(e, ...) (e).Bind([&](auto){ return EXPAND(DO89(__VA_ARGS__)); })
#define DO91(e, ...) (e).Bind([&](auto){ return EXPAND(DO90(__VA_ARGS__)); })
#define DO92(e, ...) (e).Bind([&](auto){ return EXPAND(DO91(__VA_ARGS__)); })
#define DO93(e, ...) (e).Bind([&](auto){ return EXPAND(DO92(__VA_ARGS__)); })
#define DO94(e, ...) (e).Bind([&](auto){ return EXPAND(DO93(__VA_ARGS__)); })
#define DO95(e, ...) (e).Bind([&](auto){ return EXPAND(DO94(__VA_ARGS__)); })
#define DO96(e, ...) (e).Bind([&](auto){ return EXPAND(DO95(__VA_ARGS__)); })
#define DO97(e, ...) (e).Bind([&](auto){ return EXPAND(DO96(__VA_ARGS__)); })
#define DO98(e, ...) (e).Bind([&](auto){ return EXPAND(DO97(__VA_ARGS__)); })
#define DO99(e, ...) (e).Bind([&](auto){ return EXPAND(DO98(__VA_ARGS__)); })

#define DO100(e, ...) (e).Bind([&](auto){ return EXPAND(DO99(__VA_ARGS__)); })

#define DON(...) DO_IMPL(VA_NARGS(__VA_ARGS__), __VA_ARGS__)
#define DO(...) (DON(__VA_ARGS__))