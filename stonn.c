#include "stonn.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MIN2(A, B) ((A) < (B) ? (A) : (B))
#define clzll(X) __builtin_clzll(X)
#define ctz(X) __builtin_ctz(X)
#define umulll_overflow(IN, V, OUT) __builtin_umulll_overflow((IN), (V), (OUT))
static uint64_t upow10[] = { 1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL, 100000000ULL };
static const float fpow10[] = { 1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f, 1e6f, 1e7f, 1e8f, 1e9f, 1e10f };
static const double dpow10[] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22 };
static const uint64_t mpow5_64[] = { 0xa5ced43b7e3e9188, 0xcf42894a5dce35ea, 0x818995ce7aa0e1b2,
    0xa1ebfb4219491a1f, 0xca66fa129f9b60a6, 0xfd00b897478238d0, 0x9e20735e8cb16382,
    0xc5a890362fddbc62, 0xf712b443bbd52b7b, 0x9a6bb0aa55653b2d, 0xc1069cd4eabe89f8,
    0xf148440a256e2c76, 0x96cd2a865764dbca, 0xbc807527ed3e12bc, 0xeba09271e88d976b,
    0x93445b8731587ea3, 0xb8157268fdae9e4c, 0xe61acf033d1a45df, 0x8fd0c16206306bab,
    0xb3c4f1ba87bc8696, 0xe0b62e2929aba83c, 0x8c71dcd9ba0b4925, 0xaf8e5410288e1b6f,
    0xdb71e91432b1a24a, 0x892731ac9faf056e, 0xab70fe17c79ac6ca, 0xd64d3d9db981787d,
    0x85f0468293f0eb4e, 0xa76c582338ed2621, 0xd1476e2c07286faa, 0x82cca4db847945ca,
    0xa37fce126597973c, 0xcc5fc196fefd7d0c, 0xff77b1fcbebcdc4f, 0x9faacf3df73609b1,
    0xc795830d75038c1d, 0xf97ae3d0d2446f25, 0x9becce62836ac577, 0xc2e801fb244576d5,
    0xf3a20279ed56d48a, 0x9845418c345644d6, 0xbe5691ef416bd60c, 0xedec366b11c6cb8f,
    0x94b3a202eb1c3f39, 0xb9e08a83a5e34f07, 0xe858ad248f5c22c9, 0x91376c36d99995be,
    0xb58547448ffffb2d, 0xe2e69915b3fff9f9, 0x8dd01fad907ffc3b, 0xb1442798f49ffb4a,
    0xdd95317f31c7fa1d, 0x8a7d3eef7f1cfc52, 0xad1c8eab5ee43b66, 0xd863b256369d4a40,
    0x873e4f75e2224e68, 0xa90de3535aaae202, 0xd3515c2831559a83, 0x8412d9991ed58091,
    0xa5178fff668ae0b6, 0xce5d73ff402d98e3, 0x80fa687f881c7f8e, 0xa139029f6a239f72,
    0xc987434744ac874e, 0xfbe9141915d7a922, 0x9d71ac8fada6c9b5, 0xc4ce17b399107c22,
    0xf6019da07f549b2b, 0x99c102844f94e0fb, 0xc0314325637a1939, 0xf03d93eebc589f88,
    0x96267c7535b763b5, 0xbbb01b9283253ca2, 0xea9c227723ee8bcb, 0x92a1958a7675175f,
    0xb749faed14125d36, 0xe51c79a85916f484, 0x8f31cc0937ae58d2, 0xb2fe3f0b8599ef07,
    0xdfbdcece67006ac9, 0x8bd6a141006042bd, 0xaecc49914078536d, 0xda7f5bf590966848,
    0x888f99797a5e012d, 0xaab37fd7d8f58178, 0xd5605fcdcf32e1d6, 0x855c3be0a17fcd26,
    0xa6b34ad8c9dfc06f, 0xd0601d8efc57b08b, 0x823c12795db6ce57, 0xa2cb1717b52481ed,
    0xcb7ddcdda26da268, 0xfe5d54150b090b02, 0x9efa548d26e5a6e1, 0xc6b8e9b0709f109a,
    0xf867241c8cc6d4c0, 0x9b407691d7fc44f8, 0xc21094364dfb5636, 0xf294b943e17a2bc4,
    0x979cf3ca6cec5b5a, 0xbd8430bd08277231, 0xece53cec4a314ebd, 0x940f4613ae5ed136,
    0xb913179899f68584, 0xe757dd7ec07426e5, 0x9096ea6f3848984f, 0xb4bca50b065abe63,
    0xe1ebce4dc7f16dfb, 0x8d3360f09cf6e4bd, 0xb080392cc4349dec, 0xdca04777f541c567,
    0x89e42caaf9491b60, 0xac5d37d5b79b6239, 0xd77485cb25823ac7, 0x86a8d39ef77164bc,
    0xa8530886b54dbdeb, 0xd267caa862a12d66, 0x8380dea93da4bc60, 0xa46116538d0deb78,
    0xcd795be870516656, 0x806bd9714632dff6, 0xa086cfcd97bf97f3, 0xc8a883c0fdaf7df0,
    0xfad2a4b13d1b5d6c, 0x9cc3a6eec6311a63, 0xc3f490aa77bd60fc, 0xf4f1b4d515acb93b,
    0x991711052d8bf3c5, 0xbf5cd54678eef0b6, 0xef340a98172aace4, 0x9580869f0e7aac0e,
    0xbae0a846d2195712, 0xe998d258869facd7, 0x91ff83775423cc06, 0xb67f6455292cbf08,
    0xe41f3d6a7377eeca, 0x8e938662882af53e, 0xb23867fb2a35b28d, 0xdec681f9f4c31f31,
    0x8b3c113c38f9f37e, 0xae0b158b4738705e, 0xd98ddaee19068c76, 0x87f8a8d4cfa417c9,
    0xa9f6d30a038d1dbc, 0xd47487cc8470652b, 0x84c8d4dfd2c63f3b, 0xa5fb0a17c777cf09,
    0xcf79cc9db955c2cc, 0x81ac1fe293d599bf, 0xa21727db38cb002f, 0xca9cf1d206fdc03b,
    0xfd442e4688bd304a, 0x9e4a9cec15763e2e, 0xc5dd44271ad3cdba, 0xf7549530e188c128,
    0x9a94dd3e8cf578b9, 0xc13a148e3032d6e7, 0xf18899b1bc3f8ca1, 0x96f5600f15a7b7e5,
    0xbcb2b812db11a5de, 0xebdf661791d60f56, 0x936b9fcebb25c995, 0xb84687c269ef3bfb,
    0xe65829b3046b0afa, 0x8ff71a0fe2c2e6dc, 0xb3f4e093db73a093, 0xe0f218b8d25088b8,
    0x8c974f7383725573, 0xafbd2350644eeacf, 0xdbac6c247d62a583, 0x894bc396ce5da772,
    0xab9eb47c81f5114f, 0xd686619ba27255a2, 0x8613fd0145877585, 0xa798fc4196e952e7,
    0xd17f3b51fca3a7a0, 0x82ef85133de648c4, 0xa3ab66580d5fdaf5, 0xcc963fee10b7d1b3,
    0xffbbcfe994e5c61f, 0x9fd561f1fd0f9bd3, 0xc7caba6e7c5382c8, 0xf9bd690a1b68637b,
    0x9c1661a651213e2d, 0xc31bfa0fe5698db8, 0xf3e2f893dec3f126, 0x986ddb5c6b3a76b7,
    0xbe89523386091465, 0xee2ba6c0678b597f, 0x94db483840b717ef, 0xba121a4650e4ddeb,
    0xe896a0d7e51e1566, 0x915e2486ef32cd60, 0xb5b5ada8aaff80b8, 0xe3231912d5bf60e6,
    0x8df5efabc5979c8f, 0xb1736b96b6fd83b3, 0xddd0467c64bce4a0, 0x8aa22c0dbef60ee4,
    0xad4ab7112eb3929d, 0xd89d64d57a607744, 0x87625f056c7c4a8b, 0xa93af6c6c79b5d2d,
    0xd389b47879823479, 0x843610cb4bf160cb, 0xa54394fe1eedb8fe, 0xce947a3da6a9273e,
    0x811ccc668829b887, 0xa163ff802a3426a8, 0xc9bcff6034c13052, 0xfc2c3f3841f17c67,
    0x9d9ba7832936edc0, 0xc5029163f384a931, 0xf64335bcf065d37d, 0x99ea0196163fa42e,
    0xc06481fb9bcf8d39, 0xf07da27a82c37088, 0x964e858c91ba2655, 0xbbe226efb628afea,
    0xeadab0aba3b2dbe5, 0x92c8ae6b464fc96f, 0xb77ada0617e3bbcb, 0xe55990879ddcaabd,
    0x8f57fa54c2a9eab6, 0xb32df8e9f3546564, 0xdff9772470297ebd, 0x8bfbea76c619ef36,
    0xaefae51477a06b03, 0xdab99e59958885c4, 0x88b402f7fd75539b, 0xaae103b5fcd2a881,
    0xd59944a37c0752a2, 0x857fcae62d8493a5, 0xa6dfbd9fb8e5b88e, 0xd097ad07a71f26b2,
    0x825ecc24c873782f, 0xa2f67f2dfa90563b, 0xcbb41ef979346bca, 0xfea126b7d78186bc,
    0x9f24b832e6b0f436, 0xc6ede63fa05d3143, 0xf8a95fcf88747d94, 0x9b69dbe1b548ce7c,
    0xc24452da229b021b, 0xf2d56790ab41c2a2, 0x97c560ba6b0919a5, 0xbdb6b8e905cb600f,
    0xed246723473e3813, 0x9436c0760c86e30b, 0xb94470938fa89bce, 0xe7958cb87392c2c2,
    0x90bd77f3483bb9b9, 0xb4ecd5f01a4aa828, 0xe2280b6c20dd5232, 0x8d590723948a535f,
    0xb0af48ec79ace837, 0xdcdb1b2798182244, 0x8a08f0f8bf0f156b, 0xac8b2d36eed2dac5,
    0xd7adf884aa879177, 0x86ccbb52ea94baea, 0xa87fea27a539e9a5, 0xd29fe4b18e88640e,
    0x83a3eeeef9153e89, 0xa48ceaaab75a8e2b, 0xcdb02555653131b6, 0x808e17555f3ebf11,
    0xa0b19d2ab70e6ed6, 0xc8de047564d20a8b, 0xfb158592be068d2e, 0x9ced737bb6c4183d,
    0xc428d05aa4751e4c, 0xf53304714d9265df, 0x993fe2c6d07b7fab, 0xbf8fdb78849a5f96,
    0xef73d256a5c0f77c, 0x95a8637627989aad, 0xbb127c53b17ec159, 0xe9d71b689dde71af,
    0x9226712162ab070d, 0xb6b00d69bb55c8d1, 0xe45c10c42a2b3b05, 0x8eb98a7a9a5b04e3,
    0xb267ed1940f1c61c, 0xdf01e85f912e37a3, 0x8b61313bbabce2c6, 0xae397d8aa96c1b77,
    0xd9c7dced53c72255, 0x881cea14545c7575, 0xaa242499697392d2, 0xd4ad2dbfc3d07787,
    0x84ec3c97da624ab4, 0xa6274bbdd0fadd61, 0xcfb11ead453994ba, 0x81ceb32c4b43fcf4,
    0xa2425ff75e14fc31, 0xcad2f7f5359a3b3e, 0xfd87b5f28300ca0d, 0x9e74d1b791e07e48,
    0xc612062576589dda, 0xf79687aed3eec551, 0x9abe14cd44753b52, 0xc16d9a0095928a27,
    0xf1c90080baf72cb1, 0x971da05074da7bee, 0xbce5086492111aea, 0xec1e4a7db69561a5,
    0x9392ee8e921d5d07, 0xb877aa3236a4b449, 0xe69594bec44de15b, 0x901d7cf73ab0acd9,
    0xb424dc35095cd80f, 0xe12e13424bb40e13, 0x8cbccc096f5088cb, 0xafebff0bcb24aafe,
    0xdbe6fecebdedd5be, 0x89705f4136b4a597, 0xabcc77118461cefc, 0xd6bf94d5e57a42bc,
    0x8637bd05af6c69b5, 0xa7c5ac471b478423, 0xd1b71758e219652b, 0x83126e978d4fdf3b,
    0xa3d70a3d70a3d70a, 0xcccccccccccccccc, 0x8000000000000000, 0xa000000000000000,
    0xc800000000000000, 0xfa00000000000000, 0x9c40000000000000, 0xc350000000000000,
    0xf424000000000000, 0x9896800000000000, 0xbebc200000000000, 0xee6b280000000000,
    0x9502f90000000000, 0xba43b74000000000, 0xe8d4a51000000000, 0x9184e72a00000000,
    0xb5e620f480000000, 0xe35fa931a0000000, 0x8e1bc9bf04000000, 0xb1a2bc2ec5000000,
    0xde0b6b3a76400000, 0x8ac7230489e80000, 0xad78ebc5ac620000, 0xd8d726b7177a8000,
    0x878678326eac9000, 0xa968163f0a57b400, 0xd3c21bcecceda100, 0x84595161401484a0,
    0xa56fa5b99019a5c8, 0xcecb8f27f4200f3a, 0x813f3978f8940984, 0xa18f07d736b90be5,
    0xc9f2c9cd04674ede, 0xfc6f7c4045812296, 0x9dc5ada82b70b59d, 0xc5371912364ce305,
    0xf684df56c3e01bc6, 0x9a130b963a6c115c, 0xc097ce7bc90715b3, 0xf0bdc21abb48db20,
    0x96769950b50d88f4, 0xbc143fa4e250eb31, 0xeb194f8e1ae525fd, 0x92efd1b8d0cf37be,
    0xb7abc627050305ad, 0xe596b7b0c643c719, 0x8f7e32ce7bea5c6f, 0xb35dbf821ae4f38b,
    0xe0352f62a19e306e, 0x8c213d9da502de45, 0xaf298d050e4395d6, 0xdaf3f04651d47b4c,
    0x88d8762bf324cd0f, 0xab0e93b6efee0053, 0xd5d238a4abe98068, 0x85a36366eb71f041,
    0xa70c3c40a64e6c51, 0xd0cf4b50cfe20765, 0x82818f1281ed449f, 0xa321f2d7226895c7,
    0xcbea6f8ceb02bb39, 0xfee50b7025c36a08, 0x9f4f2726179a2245, 0xc722f0ef9d80aad6,
    0xf8ebad2b84e0d58b, 0x9b934c3b330c8577, 0xc2781f49ffcfa6d5, 0xf316271c7fc3908a,
    0x97edd871cfda3a56, 0xbde94e8e43d0c8ec, 0xed63a231d4c4fb27, 0x945e455f24fb1cf8,
    0xb975d6b6ee39e436, 0xe7d34c64a9c85d44, 0x90e40fbeea1d3a4a, 0xb51d13aea4a488dd,
    0xe264589a4dcdab14, 0x8d7eb76070a08aec, 0xb0de65388cc8ada8, 0xdd15fe86affad912,
    0x8a2dbf142dfcc7ab, 0xacb92ed9397bf996, 0xd7e77a8f87daf7fb, 0x86f0ac99b4e8dafd,
    0xa8acd7c0222311bc, 0xd2d80db02aabd62b, 0x83c7088e1aab65db, 0xa4b8cab1a1563f52,
    0xcde6fd5e09abcf26, 0x80b05e5ac60b6178, 0xa0dc75f1778e39d6, 0xc913936dd571c84c,
    0xfb5878494ace3a5f, 0x9d174b2dcec0e47b, 0xc45d1df942711d9a, 0xf5746577930d6500,
    0x9968bf6abbe85f20, 0xbfc2ef456ae276e8, 0xefb3ab16c59b14a2, 0x95d04aee3b80ece5,
    0xbb445da9ca61281f, 0xea1575143cf97226, 0x924d692ca61be758, 0xb6e0c377cfa2e12e,
    0xe498f455c38b997a, 0x8edf98b59a373fec, 0xb2977ee300c50fe7, 0xdf3d5e9bc0f653e1,
    0x8b865b215899f46c, 0xae67f1e9aec07187, 0xda01ee641a708de9, 0x884134fe908658b2,
    0xaa51823e34a7eede, 0xd4e5e2cdc1d1ea96, 0x850fadc09923329e, 0xa6539930bf6bff45,
    0xcfe87f7cef46ff16, 0x81f14fae158c5f6e, 0xa26da3999aef7749, 0xcb090c8001ab551c,
    0xfdcb4fa002162a63, 0x9e9f11c4014dda7e, 0xc646d63501a1511d, 0xf7d88bc24209a565,
    0x9ae757596946075f, 0xc1a12d2fc3978937, 0xf209787bb47d6b84, 0x9745eb4d50ce6332,
    0xbd176620a501fbff, 0xec5d3fa8ce427aff, 0x93ba47c980e98cdf, 0xb8a8d9bbe123f017,
    0xe6d3102ad96cec1d, 0x9043ea1ac7e41392, 0xb454e4a179dd1877, 0xe16a1dc9d8545e94,
    0x8ce2529e2734bb1d, 0xb01ae745b101e9e4, 0xdc21a1171d42645d, 0x899504ae72497eba,
    0xabfa45da0edbde69, 0xd6f8d7509292d603, 0x865b86925b9bc5c2, 0xa7f26836f282b732,
    0xd1ef0244af2364ff, 0x8335616aed761f1f, 0xa402b9c5a8d3a6e7, 0xcd036837130890a1,
    0x802221226be55a64, 0xa02aa96b06deb0fd, 0xc83553c5c8965d3d, 0xfa42a8b73abbf48c,
    0x9c69a97284b578d7, 0xc38413cf25e2d70d, 0xf46518c2ef5b8cd1, 0x98bf2f79d5993802,
    0xbeeefb584aff8603, 0xeeaaba2e5dbf6784, 0x952ab45cfa97a0b2, 0xba756174393d88df,
    0xe912b9d1478ceb17, 0x91abb422ccb812ee, 0xb616a12b7fe617aa, 0xe39c49765fdf9d94,
    0x8e41ade9fbebc27d, 0xb1d219647ae6b31c, 0xde469fbd99a05fe3, 0x8aec23d680043bee,
    0xada72ccc20054ae9, 0xd910f7ff28069da4, 0x87aa9aff79042286, 0xa99541bf57452b28,
    0xd3fa922f2d1675f2, 0x847c9b5d7c2e09b7, 0xa59bc234db398c25, 0xcf02b2c21207ef2e,
    0x8161afb94b44f57d, 0xa1ba1ba79e1632dc, 0xca28a291859bbf93, 0xfcb2cb35e702af78,
    0x9defbf01b061adab, 0xc56baec21c7a1916, 0xf6c69a72a3989f5b, 0x9a3c2087a63f6399,
    0xc0cb28a98fcf3c7f, 0xf0fdf2d3f3c30b9f, 0x969eb7c47859e743, 0xbc4665b596706114,
    0xeb57ff22fc0c7959, 0x9316ff75dd87cbd8, 0xb7dcbf5354e9bece, 0xe5d3ef282a242e81,
    0x8fa475791a569d10, 0xb38d92d760ec4455, 0xe070f78d3927556a, 0x8c469ab843b89562,
    0xaf58416654a6babb, 0xdb2e51bfe9d0696a, 0x88fcf317f22241e2, 0xab3c2fddeeaad25a,
    0xd60b3bd56a5586f1, 0x85c7056562757456, 0xa738c6bebb12d16c, 0xd106f86e69d785c7,
    0x82a45b450226b39c, 0xa34d721642b06084, 0xcc20ce9bd35c78a5, 0xff290242c83396ce,
    0x9f79a169bd203e41, 0xc75809c42c684dd1, 0xf92e0c3537826145, 0x9bbcc7a142b17ccb,
    0xc2abf989935ddbfe, 0xf356f7ebf83552fe, 0x98165af37b2153de, 0xbe1bf1b059e9a8d6,
    0xeda2ee1c7064130c, 0x9485d4d1c63e8be7, 0xb9a74a0637ce2ee1, 0xe8111c87c5c1ba99,
    0x910ab1d4db9914a0, 0xb54d5e4a127f59c8, 0xe2a0b5dc971f303a, 0x8da471a9de737e24,
    0xb10d8e1456105dad, 0xdd50f1996b947518, 0x8a5296ffe33cc92f, 0xace73cbfdc0bfb7b,
    0xd8210befd30efa5a, 0x8714a775e3e95c78, 0xa8d9d1535ce3b396, 0xd31045a8341ca07c,
    0x83ea2b892091e44d, 0xa4e4b66b68b65d60, 0xce1de40642e3f4b9, 0x80d2ae83e9ce78f3,
    0xa1075a24e4421730, 0xc94930ae1d529cfc, 0xfb9b7cd9a4a7443c, 0x9d412e0806e88aa5,
    0xc491798a08a2ad4e, 0xf5b5d7ec8acb58a2, 0x9991a6f3d6bf1765, 0xbff610b0cc6edd3f,
    0xeff394dcff8a948e, 0x95f83d0a1fb69cd9, 0xbb764c4ca7a4440f, 0xea53df5fd18d5513,
    0x92746b9be2f8552c, 0xb7118682dbb66a77, 0xe4d5e82392a40515, 0x8f05b1163ba6832d,
    0xb2c71d5bca9023f8, 0xdf78e4b2bd342cf6, 0x8bab8eefb6409c1a, 0xae9672aba3d0c320,
    0xda3c0f568cc4f3e8, 0x8865899617fb1871, 0xaa7eebfb9df9de8d, 0xd51ea6fa85785631,
    0x8533285c936b35de, 0xa67ff273b8460356, 0xd01fef10a657842c, 0x8213f56a67f6b29b,
    0xa298f2c501f45f42, 0xcb3f2f7642717713, 0xfe0efb53d30dd4d7, 0x9ec95d1463e8a506,
    0xc67bb4597ce2ce48, 0xf81aa16fdc1b81da, 0x9b10a4e5e9913128, 0xc1d4ce1f63f57d72,
    0xf24a01a73cf2dccf, 0x976e41088617ca01, 0xbd49d14aa79dbc82, 0xec9c459d51852ba2,
    0x93e1ab8252f33b45, 0xb8da1662e7b00a17, 0xe7109bfba19c0c9d, 0x906a617d450187e2,
    0xb484f9dc9641e9da, 0xe1a63853bbd26451, 0x8d07e33455637eb2, 0xb049dc016abc5e5f,
    0xdc5c5301c56b75f7, 0x89b9b3e11b6329ba, 0xac2820d9623bf429, 0xd732290fbacaf133,
    0x867f59a9d4bed6c0, 0xa81f301449ee8c70, 0xd226fc195c6a2f8c, 0x83585d8fd9c25db7,
    0xa42e74f3d032f525, 0xcd3a1230c43fb26f, 0x80444b5e7aa7cf85, 0xa0555e361951c366,
    0xc86ab5c39fa63440, 0xfa856334878fc150, 0x9c935e00d4b9d8d2, 0xc3b8358109e84f07,
    0xf4a642e14c6262c8, 0x98e7e9cccfbd7dbd, 0xbf21e44003acdd2c, 0xeeea5d5004981478,
    0x95527a5202df0ccb, 0xbaa718e68396cffd, 0xe950df20247c83fd, 0x91d28b7416cdd27e,
    0xb6472e511c81471d, 0xe3d8f9e563a198e5, 0x8e679c2f5e44ff8f };

#ifndef STONN_BYTE_ORDER_BIG_ENDIAN
struct uint64pair {
    uint64_t lo;
    uint64_t hi;
};
static uint64_t numparse8cB10LE(const char* text, size_t length)
{
    uint64_t chunk = 0, d0, d1;
    memcpy((char*)&chunk + (8 - length), text, length);

    d0 = (chunk & 0x0F000F000F000F00) >> 8;
    d1 = (chunk & 0x000F000F000F000F) * 10;
    chunk = d0 + d1;
    d0 = (chunk & 0x00FF000000FF0000) >> 16;
    d1 = (chunk & 0x000000FF000000FF) * 100;
    chunk = d0 + d1;
    d0 = (chunk & 0x0000FFFF00000000) >> 32;
    d1 = (chunk & 0x000000000000FFFF) * 10000;
    chunk = d0 + d1;
    return chunk;
}
#endif

#ifndef STONN_BYTE_ORDER_LITTLE_ENDIAN
struct uint64pair {
    uint64_t hi;
    uint64_t lo;
};
static uint64_t numparse8cB10BE(const char* text, size_t length)
{
    uint64_t chunk = 0, d0, d1;
    memcpy((char*)&chunk, text, length);

    d0 = ((chunk & 0x0F000F000F000F00) >> 8) * 10;
    d1 = chunk & 0x000F000F000F000F;
    chunk = d0 + d1;
    d0 = ((chunk & 0x00FF000000FF0000) >> 16) * 100;
    d1 = chunk & 0x000000FF000000FF;
    chunk = d0 + d1;
    d0 = ((chunk & 0x0000FFFF00000000) >> 32) * 10000;
    d1 = chunk & 0x000000000000FFFF;
    chunk = d0 + d1;
    return chunk;
}
#endif

#if defined(STONN_BYTE_ORDER_BIG_ENDIAN)
#define numparse8cB10(T, L) numparse8cB10BE((T), (L))
#elif defined(STONN_BYTE_ORDER_LITTLE_ENDIAN)
#define numparse8cB10(T, L) numparse8cB10LE((T), (L))
#else
#define numparse8cB10(T, L) ((((union{unsigned x; unsigned char c;}){1}).c) ? numparse8cB10LE((T), (L)) : numparse8cB10BE((T), (L)))
#endif

static uint32_t numparse8cLB(const char* ptr, size_t len, int log_base)
{
    uint32_t acc = 0;
    for (size_t i = 0; i < len; i++)
        acc = (acc << log_base) + (ptr[i] & 15) + (ptr[i] >= 0x40 ? 9 : 0);

    return acc;
}

#define numparse8c(T, L, B) ((B) == 10 ? numparse8cB10(T, L) : numparse8cLB(T, L, ctz(B)))

int stounn(const char* ptr, size_t len, int base, unsigned long long* out)
{
    unsigned long long tmpv;
    int chunk_size = 0;
    uint32_t chunk_value;
    *out = 0;
    for (size_t i = 0; i < len; i += chunk_size) {
        chunk_size = MIN2(8, len - i);
        if (base == 10) {
            chunk_value = numparse8cB10(ptr + i, chunk_size);
            if (umulll_overflow(*out, upow10[chunk_size], &tmpv))
                return -1;
        } else {
            chunk_value = numparse8cLB(ptr + i, chunk_size, ctz(base));
            if (umulll_overflow(*out, 1ULL << (chunk_size * ctz(base)), &tmpv))
                return -1;
        }
        *out = tmpv + chunk_value;
    }
    return 0;
}

int stonn(const char* ptr, size_t len, int base, long long* out)
{
    unsigned long long other;
    int has_sign = (*ptr == '+' || *ptr == '-');
    int ret = stounn(ptr + has_sign, len - has_sign, base, &other);
    if (ret < 0)
        return ret;
    if (*ptr == '-') {
        if (other > ((unsigned long long)LLONG_MAX + 1))
            return -1;
        *out = -other;
        return 0;
    }
    else {
        if (other > LLONG_MAX)
            return -1;
        *out = other;
        return 0;
    }
}

static int Luinteger10(const char* token, size_t length)
{
    for (int r = 0; r < length; r++) {
        if (isdigit(token[r]))
            continue;
        else
            return r;
    }
    return length;
}

static int Lsuffix(const char* token, size_t length)
{
    if (length > 1 && (token[0] == 'e' || token[0] == 'E')) {
        if (token[1] == '+' || token[1] == '-')
            return 2 + Luinteger10(token + 2, length - 2);
        else
            return 1 + Luinteger10(token + 1, length - 1);
    }
    return 0;
}

static void parse_float(const char* ptr, size_t len, unsigned long long* w, long long* q)
{
    int r = Luinteger10(ptr, len), s = 0, u = 0;
    if (r < len && ptr[r] == '.')
        s = 1 + Luinteger10(ptr + r + 1, len - r - 1);
    u = Lsuffix(ptr + r + s, len - r - s);

    unsigned long long W = 0, Q = 0;
    if (u > 1) {
        int qov = stonn(ptr + r + s + 1, u - 1, 10, &Q);
        if (qov < 0) {
            *w = 1;
            *q = ptr[r + s + 1] == '-' ? LLONG_MIN : LLONG_MAX;
            return;
        }
    }

    // remove leading zeroes
    const char* sstart = ptr;
    char dot = 1;
    while (*sstart == '0') {
        sstart++;
        r--;
    }
    if (r == 0) {
        dot = 0;
        while (*(++sstart) == '0') {
            s--;
            Q--;
        }
    }

    // extract mantissa to at-most 19 significant digits.
    int digits = 0;
    while (digits < MIN2(r, 19)) {
        int chunk_size = MIN2(8, MIN2(r, 19) - digits);
        W = (W * upow10[chunk_size]) + numparse8cB10(sstart + digits, chunk_size);
        digits += chunk_size;
    }
    while (digits < MIN2(r + s, 19)) {
        int chunk_size = MIN2(8, MIN2(r + s, 19) - digits);
        W = (W * upow10[chunk_size]) + numparse8cB10(sstart + digits + dot, chunk_size);
        digits += chunk_size;
        Q -= chunk_size;
    }
    if (r > 19)
        Q += r - 19;

    *w = W;
    *q = Q;
}

static struct uint64pair mulu64(uint64_t a, uint64_t b)
{
    struct uint64pair out;
    #if defined(__SIZEOF_INT128__)
    __uint128_t p = ((__uint128_t)a) * b;
    out.lo = (uint64_t)p;
    out.hi = (uint64_t)(p >> 64);
    #elif defined(_MSC_VER) && defined(_WIN64)
    out.lo = a * b;
    out.hi = __umulh(a, b);
    #else
    const uint64_t a0 = (uint32_t)a, a1 = a >> 32, b0 = (uint32_t)b, b1 = b >> 32;
    const uint64_t p11 = a1 * b1, p01 = a0 * b1, p10 = a1 * b0, p00 = a0 * b0;
    const uint64_t mid = p10 + (p00 >> 32) + (uint32_t)p01;
    out.lo = (mid << 32) | (uint32_t)p00;
    out.hi = p11 + (mid >> 32) + (p01 >> 32);
    #endif
    return out;
}

#define STONF_FINISH(OUT) do { *out = (OUT); if (has_sign && *ptr == '-') *out = -*out; return 0;} while (0)

int stonlf(const char* ptr, size_t len, double* out)
{
#if 0
The contents of this function are based on software that is ...

Copyright (c) Daniel Lemire

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
#endif

    unsigned long long w;
    long long q;
    int has_sign = (*ptr == '+' || *ptr == '-');
    parse_float(ptr + has_sign, len - has_sign, &w, &q);

    if (w < (1ULL << 53) && -22 <= q && q <= 22) {
        double dw = w;
        if (q >= 0)
            STONF_FINISH(dw * dpow10[q]);
        else
            STONF_FINISH(dw / dpow10[-q]);
    }
    if (w == 0 || q < -325)
        STONF_FINISH(0);
    if (q > 308)
        STONF_FINISH(INFINITY);

    int l = clzll(w);
    w <<= l;

    struct uint64pair product = mulu64(w, mpow5_64[q + 325]);
    uint64_t upperbit = product.hi >> 63, mantissa = product.hi >> (upperbit + 9);
    l += (int)(1 ^ upperbit);
    mantissa = (mantissa + (mantissa & 1)) >> 1;
    if (mantissa >= (1ULL << 53)) {
        mantissa = (1ULL << 52);
        l--;
    }
    mantissa &= ~(1ULL << 52);

    uint64_t exponent = (((217706 * q) >> 16) + 1024 + 63) - l;
    if (exponent < 1 || exponent > 2046) // out of range
        STONF_FINISH(strtod(ptr + has_sign, NULL));

    mantissa |= exponent << 52;
    memcpy(out, &mantissa, sizeof(double));
    if (has_sign && *ptr == '-') *out = -*out;
    return 0;
}
