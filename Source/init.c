/** 
 * @file init.c
 * This file includes all data needed to be initialized.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"stdlib.h"


u8 Sq120ToSq64[BRD_SQ_NUM];
u8 Sq64ToSq120[64];

u64 PieceKeys[13][64] = {
	{
	0x593d91e36b8b4567, 0xfa586e28e6334873, 0x712aac762ae8944a, 0xde8dac5d46e87ccd, 
	0xe0db8f7daeb141f2, 0xa8af803e7545e146, 0xe6d893fc12200854, 0x48c866f39f16e9e8, 
	0x99a912ad140e0f76, 0xffee61198ded7263, 0xb5b403cd41a7c4c9, 0xe8cda806a5e45d32, 
	0xfe41f22c3f2dba31, 0x61b63092e2bbd95a, 0x390ed18bb33ab105, 0x33b1c22f2d1d5ae9, 
	0x7cc1c65908edbdab, 0xc4d3b4d8b03e0c6 , 0x96544308f1f32454, 0x5d4afc3a02901d82, 
	0x3e1ede9e9e7ff521, 0x51110d386ceaf087, 0x70a7ea50b006c83e, 0xe205d6fe5577f8e1, 
	0xbba32f80b804823e, 0x1231dcf55c482a97, 0xd6a8bbcb51ead36b, 0x9c2aac2e153ea438, 
	0xd5243e586a2342ec, 0xd66c4d19725a06fb, 0x25ac47aa7a6d8d3c, 0x1c21bfedede91b18, 
	0xb425240d32fff902, 0x9ee1207df49abb43, 0xfae3619d79a1deaa, 0xa90776e8f0c6a529, 
	0x11fce09e4f4ef005, 0x1c9c32baa75ac794, 0x51add430980115be, 0xcadad7ae354fe9f9, 
	0xc8119e4c8d34b6a8, 0xff062bd8e1574095, 0x58862859d79be4f1, 0x12d385fbaf305def, 
	0x8fa47550cad084e9, 0xc807c7e55db70ae5, 0x2faf3fe815014acb, 0x435ca3b2799d0247, 
	0x8f5d2e91968e121f, 0xea05238e5dc79ea8, 0x309f7ee2d1d9c564, 0x614b1f2d11447b73, 
	0x4d1911a588f2b15e, 0x24b1409de8eb2f63, 0x4a19831206a5ee64, 0x38f508849a27709e, 
	0x375adea7fb7e0aa , 0x442f0d894211f2  , 0xcb8b381dcc04a8af, 0x3a6f0771b222e7cd, 
	0x635bea23adf6d648, 0xebfe27ddb9ee015c, 0xb077808cc3f18422, 0x64ed183eff01579b
	},
	{
	0xa8400f70dfb8370b, 0xf553c7bfdfb8011c, 0x352fb814efc75af8, 0xf9d0c10ddf3534a4, 
	0x9fd31565555c55b5, 0xb8e4894c6a3dd3e8, 0x4fdff47053299938, 0x2cd6f51e9d545c4d, 
	0x8ecfb72faa155dbc, 0xce5062fd51088277, 0x3e2c7e82c15e286c, 0xee087f10c5e6d486, 
	0x65ec2c8d3c5991aa, 0x1586c6df39b7aaa2, 0xc034f1b9b79e21b5, 0xf553dbae4c9b0904, 
	0xdee8a84a5675ff36, 0x2d92d6712708c9af, 0xc29d854f4f97e3e4, 0x6b21c68ad915ff32, 
	0x8bd0dac12c6e4afd, 0x2ec445045046b5a9, 0x8cf10dd95ec6afd4, 0x29cce32a57a61a29, 
	0x59bf4054427069a , 0x2be97887704e1dd5, 0x172478220e3e47a8, 0xaaa8dcf9cbee5a5b, 
	0x5274ceebe34c574c, 0xbeffce849849c29b, 0x1536f40369e7f3e5, 0x7d5a4ed7b7df2233, 
	0xecbda6c261e74ea3, 0x1890b3d6d7c7d42d, 0xbaa43723f8b5e776, 0x32cb460e1a0dde32, 
	0x39ea69e2260d8c4a, 0xdfe197106fde8af6, 0xd187c2b614d53685, 0x1daca403bf06ecb2, 
	0xd2a09df63f7c2ff4, 0xae902f10579328b9, 0x9f83d661cd32ab86, 0xb67c078dcb44a05, 
	0xc7c2b433bfcfaed9, 0x14c9a34cae22fbb7, 0x27d06971f44939a3, 0x1fbfaeece8b867d3, 
	0x2a2a4a2f32794ff7, 0x9adc0882123d5f2, 0x296bd8828dcdf8f6, 0x15356cdf24e60401, 
	0x3bcec2a236b2acbc, 0x6a28e7a4a1faa2fa, 0xa10bf11be6400e6, 0x61223ced5015cd1a, 
	0x1b46d9bf475e256a, 0x4fa30da8b27b517e, 0x28dfb5a45d5babb3, 0x3971a09d2b4b8b53, 
	0x40589d18b494b2fb, 0xd2cb9f19631f1690, 0xee24f5776ec9d844, 0x7f2246f4b97c46bc
	},
	{
	0x1e5447679afe3625, 0x865f2df48c058df5, 0x2a0e40a9a6a02c5e, 0x834be96910db9daa, 
	0xb1f9bf42ba966cd0, 0x479f84ec38a5d054, 0xa1e9de6a4a10b4e8, 0x137969b22e534a82, 
	0xcade55d43d00b9d9, 0x279c7932c34bae75, 0xf51b94451de8725a, 0x7b1e5b400ca6b462, 
	0xe6e716f3da97044 , 0x96dc4044a539dfa5, 0xf42776acac106a57, 0x107a5ed684a66051, 
	0x3582b94ad01f9786, 0xcac5e8b9c4a08ec , 0x2b2cbb788e0bb885, 0x4cd292b35c17530c, 
	0x1bc6cbe028677b7c, 0x221a2611316032bb, 0x80eede33471745e4, 0xd38bce05b0eada61, 
	0x29940331215641af, 0x6be7335a75b52783, 0xd3f6ff1d5953172f, 0xa21c039380c4c3af, 
	0xe921dfd64f294393, 0x572d895c334a6f1f, 0x6b61471a66a48d11, 0xc41160778435d38d, 
	0xa79d036a1803d089, 0x4012709ac8aeb063, 0x10bd9172271210c7, 0xb982533edc45e83 , 
	0xb841b0cb556b69ed, 0x78a412a2a817e7ec, 0x6038b46c141d2302, 0x79ddebfc567bd50a, 
	0x6d303284b27fac77, 0x6f4b1c4b46111ba5, 0x7b4d048f9dd6d6f4, 0x9fa45cc1777a4eaa, 
	0xb32d6524f835626c, 0xaa0f9466cc187c90, 0x14c5495d77c9fd68, 0xa8d13d534b697c7a, 
	0xb2dfced4634102b4, 0x9e1db9594d5c4899, 0xf75d8f9565d2a137, 0x5a661d67d50b8808, 
	0x295188e1cc672fc9, 0xe50eb03712fcde5e, 0x2129616d59b76e28, 0x47359fb541ed20d7, 
	0xc31a9e6941531ded, 0x68cf5ca6313c7c99, 0x9a6fde00126e008b, 0x11e024d3410ed56 , 
	0xb40752e8c96fb218, 0x5e8d16ecb92edbe4, 0x44c0aed187843e45, 0xce3f1e00cc73a9c6
	},
	{
	0x7b1ad51550abcec9, 0xdc116580bb3ebe15, 0x7cbaf47779f0d62f, 0x555932c3e22d8102, 
	0x9e9033099c0ca67c, 0x3898a9b4bbab699e, 0x5d5082c097859f72, 0xb9e7f0b2e21af471, 
	0x7daa0fd5e17c843e, 0xe3e3e4b8b38125cf, 0x4c39cc1d4fa327ce, 0xa9d92b27ae17eca7, 
	0x26d1963f1af7f0ea, 0xd7e856c08e0d31ff, 0xa5eea9fed2c12c61, 0xc3c45ecdcc54e2c3, 
	0x4f872e86c8249dbf, 0x8094a8bf2c02fe8c, 0x7152eadeccfb8d32, 0x8b137dc67775797c, 
	0x32e511adbe6da1c7, 0x9f108017b9b743c , 0x976d0076f0ec11b2, 0xde7399765fb29816, 
	0x7d42fa6f92b8401 , 0xedfea1084fc4d600, 0x5210efe7978f7b67, 0xe8126f2d99fb2650, 
	0xe9035c71036b29f , 0x4a4af28501c65e98, 0x2981c368bb1dd403, 0xa74f7d868ce8e1a7, 
	0xdfc5865fb266459b, 0xf9ed26aa586d60e , 0x4c34946b48781401, 0xb416ff6b3746a5f2, 
	0x7fdebd1f2d9df57d, 0x4cdc50474303a216, 0xd4f7a996d992a02e, 0x55dad8e352c77402, 
	0xb369c2e4e2234363, 0xf9de33b85749361f, 0x93115686605138de, 0xe5d4c18d18333c89, 
	0x4a10cb8d8cfc7321, 0x25b41e86916e0907, 0xbc831945f1dce0fd, 0x1e04ea50f66d98c2, 
	0x8ace3a67cb232ee3, 0x3431f4693cf93092, 0x6543961ac365174b, 0x50e8312a9cd484d5, 
	0x4110cd99c4e2ed6c, 0xcd2a6bde2c7c62c2, 0xd3a8a56f33537ced, 0x1c5fa9f2d293bfef, 
	0xd54e1a715a0201c7, 0x5084e6d2612dbddd, 0x36167e4402121a81, 0x9493168d4ce5ca53, 
	0x3645a233436f2beb, 0x44afe49da0cc134c, 0x27215b5471e5621e, 0x3d5f8cb569215dfb
	},
	{
	0x97094b2c2c06dcf3, 0xc0532b23830df306, 0x6d542c07257d4626, 0x9deb0af5c0f2cbd2, 
	0x9ef418361872261f, 0x566fd60058df53b , 0xa71e9355db0dad2a, 0xb7849cfc7b14914e, 
	0xb0774e0b3cebd7c7, 0x2507fd0880e4b973, 0x3b0b6b825f3272db, 0xfc92650564d17722, 
	0x4f510818a32fcf4d, 0x2fca6e018c2a5c5b, 0x7c038111162eb70d, 0x7eca7bae8c600e47, 
	0x2c515243abb7929b, 0x29db9aeab79a5b56, 0x885186ce4cdced4c, 0x8c3e387b06235ea , 
	0x385d1cf58ea697f2, 0xeac5d8869afd9053, 0x3c5dc02d27fadefa, 0xc272ee5a8fa085b0, 
	0x4d8a49e1331c4250, 0x93ba510494802f5d, 0xa9961a5280b2172a, 0xc56347fdbb2125a3, 
	0x2bd1bea3e4996e13, 0xc402d998ef00529a, 0x8b562591c3773132, 0xd8d4edbb0e04e6ce, 
	0xacd25d38d1b76675, 0xf4a2a010b40bf64d, 0x8e30c138f5f56228, 0xd870d8e7afe5d025, 
	0x18c9e47ce76b3580, 0xf65a774e77933f62, 0xa8a72e585b727f19, 0xe02757a54ff4cc6d, 
	0xc1e2f03e55fee0d1, 0x48e563a54cc32f1f, 0xb1c116e019132e7b, 0xcbc71206de1eae31, 
	0x46c1c31b28100a9c, 0x1eb4ab2d8d15faca, 0x526a45ed99908d02, 0x8e33e59ef93753de, 
	0xbbed252be3a4aab6, 0x23e78b6225b57ed4, 0xee6fbcaefcc0624 , 0xb548935ad08ed897, 
	0x81d22a9862fb8680, 0x30e181b0cf0ceedb, 0x84e986fea4639756, 0x4b748464161bc243, 
	0x583ccae538b59eff, 0x94d874d40ce344b5, 0x46aaca2f4dc5d907, 0x99858999dff6ca09, 
	0xd16b8cabb2766a55, 0x39b2f8f708bbeb1d, 0x2b30bc3770cc332f, 0x52b733a4bc5ea902
	},
	{
	0xe4e8ff501f7f42e2, 0xeff6e071902362f8, 0x4c51aff1be30d969, 0x52c3524c1112dbb6, 
	0x39a63ecf805ca941, 0xe9a1a3d46780c122, 0x82dcd8fe9673efc8, 0x73ace8d63351604, 
	0xbfa0e8249a8762f4, 0x1dd03ca5d57e0515, 0x2d8fde16f237aa96, 0xf5a18f9298c45e15, 
	0x94b9fc47d63a1a5c, 0x9d436a22c4296c6d, 0x5d71bec35bc9a827, 0x51321f5480bf783f, 
	0x9c6c1738d8c05b8a, 0xe06a23ad398582c , 0x9b4a53b541d646bc, 0x4609563f8675adba, 
	0xff252b8af0837c02, 0xb875a95a46348ea , 0xf6a467c3b830d6b6, 0x58b91dfae4a1c464, 
	0xc69df20e369956ab, 0x6fdae2dfbdd6690 , 0xb35ddbf29464be1e, 0xdd2a0808b2a5c7a8, 
	0x7e152b66df5c6e4d, 0x15002438b54e53b , 0xb991c04572edd574, 0x4bc364ba35a681db, 
	0x27dbd01563a24d68, 0x5a2cb247bb121183, 0x75795d9d4d08a9e4, 0x336810e519e2bfcc, 
	0x7d9a6ff40921145c, 0x96ed53c8b137975e, 0xc69b8aef755b5ed6, 0xcc45fd8cf013cdb7, 
	0x45bce846f6dee918, 0x60902934fbf45b8e, 0x12614fe89818832f, 0xefea58aa21857a57, 
	0x566ead9cdc64ad75, 0xe3600d82a47fc4ae, 0x27f097b0fa963843, 0x408c6360376863bc, 
	0xfb3a12cb2e8e9f47, 0x3343f9a6a1df319e, 0x6eb36e3297cfc87d, 0xacad9bf980e3dfe6, 
	0xf8b9dd91017ec0c4, 0x97c9abd51d141cc , 0x77aef177d67d2763, 0x89eed3cec61bc834, 
	0xc739eff01c8a8acf, 0xe2ee21ceee2e2d53, 0x7a3571726dac7768, 0xcb250a417a61d75b, 
	0x170cee801fa33267, 0x177ede73505e5e01, 0x983e3ed54239dc03, 0x126dfbb58d7f9d7
	},
	{
	0xec65736ccf4ac8ff, 0x4f735786f29af360, 0x5db89ceedcab38c6, 0x94cfb398169cad8d, 
	0x8ba5ef4cd1a6e915, 0x48d6dafa03254a32, 0x58a8742e1b2b9e9d, 0xc0d7a32e9098cf6e, 
	0xa1f491305f613832, 0xe608e1b9f9d5c18 , 0xb605f29b72d2b579, 0xef536c4b3565f1c8, 
	0x1da908ae0bd6d5b5, 0x68f75f7570bb9983, 0x91cad3ff9b7ca0ed, 0x935d7918baa88b3c, 
	0xa0f34867f01e8a0d, 0xe93fb01ed3ae65de, 0x18f04c37e63bfe95, 0xe8befa43a94614f7, 
	0xa22855006e9dead0, 0x3cdb4de48cc0a782, 0x42c6b8bf49ca1341, 0xfb248580c244b8de, 
	0x88e2d5f7e45ec5ab, 0xe6371b959dc4b111, 0x9e46609c68d8526b, 0xc81d938b653cdc22, 
	0xbb3b92d5e350ac35, 0x8fde9d51a54d9ea9, 0x872d92b9840bad55, 0xcd8de67aced5a137, 
	0xb272f01b78466daf, 0xd3954c8a5d094b84, 0x85c4af5f93bae843, 0x54a707e86e022d01, 
	0x4913311e6f0463f1, 0xfbb19f2f31ed2baf, 0xed59f549c75553ab, 0x8000c49e8e9e1415, 
	0x42069b492764c907, 0xa9716be4847c27fa, 0x2594a2bc5b4a22b5, 0x5419487eb7a58eef, 
	0x1df6bca00a4a062b, 0x54f7d3216eb42955, 0x4c7a05199f8c83ab, 0xe5709af10cf19f38, 
	0x891b44c68a91e1dc, 0xd06a4ed1fea94a2b, 0x641cb355127ded1d, 0xa65ac75234adb582, 
	0xd3ffd8ad71f94adc, 0x2fd2a024942ac06c, 0x34f7a33a5d86cf11, 0xac51b7e5326c3c76, 
	0x7c17f9bab09fb9ab, 0x290c956c0f9991c , 0x47d9ba44c8b6b6eb, 0x72e5f5a2b27bf4d , 
	0xd06d55e88d9a50ed, 0xeac430a9f41a191e, 0x63c0d617ad7cb919, 0xedd6364e32089e8c
	},
	{
	0xdc999dd6dec714fc, 0x75cfbc11c77304ac, 0x8e1f98e75a0e9a9f, 0x2e9c6575059e3ac4, 
	0x52f7c0eafc0f10cc, 0xa88ba09184b67062, 0xef58c9082e326d9a, 0xe965d5971fe7d17d, 
	0x80243223de52c405, 0x99288169c4bd67e6, 0x88c0be78acd4c11 , 0xec458e3e3acc76b1, 
	0x994cdb8dc36f111a, 0x1c1bf8efc4419ef9, 0x9a2381564a35bf85, 0x5c7eb9875fdd549a, 
	0x7397f05403e1a864, 0xc38bd91348c0a800, 0xf2b53b15e32808ef, 0x9513ef08c511160d, 
	0x9a7a951182a68fc8, 0xb7e0d06a796f4530, 0x599858f75bd772bb, 0x3bb92873da7ed215, 
	0x20d40836365efc5a, 0x90c2b283b82ea874, 0xd2b386b58b1595b1, 0x7713dab5bcae14fe, 
	0x28a7df2d41123f79, 0x4b306a3445bece3c, 0xc83b2187cbc5fd3 , 0x93b54b044d20939 , 
	0xf7270e0de572cf1b, 0xa4667718b8ad4c18, 0xff95f545c41fbaf4, 0x9bad497f83d92c7d, 
	0x613814587c901113, 0x7f8f1ed75c76d652, 0xe0183e93c3754db7, 0xc2f7a5b20fde3b47, 
	0xc155d59b866f06e9, 0xe3beda3831f4fcad, 0xd67841c5cd3c9346, 0x32cee7d19bb4dbcd, 
	0xd4dec54be19f753b, 0x96cb5b8a69321f16, 0x95136414124a2df8, 0x34ecf3d60c94a46 , 
	0xe50976191d701bff, 0xecf893bcab66ac12, 0xb03017314fbe108e, 0x892a95880d1cd939, 
	0x6fc8df2b5cf24b45, 0x3da34d119b15d96c, 0x2e7307af220655c4, 0x230c173aa822bca7, 
	0x2c312e36e599b689, 0xdc95d359e580d91b, 0x6b4de1596486395b, 0x81013762a3bc16c2, 
	0x68e03fa9e8e13f8e, 0x6f6eac4630b21390, 0xdde7d1e8c0009c03, 0xab72be9efc3fa800
	},
	{
	0x3c75e9809e1c83a1, 0x508747d469b6e9d9, 0xc3542318e2e80725, 0xb6148fa6063fec9a, 
	0x6857ac53b0357089, 0xba35b7b5243de0aa, 0x6ea6577c72377844, 0x46ff614420913e41, 
	0xe6ffaf45e1402f4e, 0x251f83460180deb1, 0xb42dc5169d082837, 0xa9093a031575811c, 
	0x1a7d3d95d3fb0875, 0x1d9d33e2eae2ec3c, 0x35b7ec273af676b7, 0x87d6dc7c61a5c3e4, 
	0x80f2989e8a577fd6, 0x913b37bf04c9bb1b, 0x81db4f66de194eec, 0x829bbebf7338868f, 
	0xd11fd2db5f3d3b70, 0x5bda9369442dc352, 0x5d817a3ca2cdadc , 0xbb498dc256d43898, 
	0x18c4c91dcfa4b5a1, 0x4997ab0f99248b51, 0x8ec36afad70fd06b, 0x112848887247c47d, 
	0x4034effe1b7e7b17, 0x39d1334612b4903d, 0xa96fd0fe5429abef, 0xb86b2a77dfa51ce , 
	0xf09d201579dd6bb3, 0xdc073c61aabb00c4, 0x83d9973216d2fd9a, 0x506bdcdaee5ad2be, 
	0xbbf3c510559060db, 0xb517a74ebeab2035, 0xc2d6e4da4a3c4f0e, 0xcc312cf959841cb3, 
	0xf64602f0ee0367d7, 0x75432bd824c258b1, 0xf231e1b19336705d, 0x878f62138d5ab751, 
	0x92f8e0e0fe1219c8, 0xca264a4020530e7b, 0xf4ee7aadd5c3594c, 0x14440ac8a8ba7a13, 
	0x7962324f9e022be9, 0xe62440a913b3bdbc, 0x1d25f4957d07b61b, 0x2f8720edb93b5076, 
	0x3922591c8c7c501f, 0xffcfb4c540f78ff3, 0x7ed8c1a92d814889, 0x4f0248e6a1be2f84, 
	0x39e3f58d36d2d033, 0x4e413296c3cefb03, 0x1d42488b3afa746f, 0xe71c2769b7d3b790, 
	0x65a002774d0f54a3, 0x23daab2ced6f798 , 0x487bd2c23b24f23c, 0xe8f79abb8bd6b044
	},
	{
	0xbfb83f00eba73274, 0xd09756c2f1697665, 0x6c00bedc0b784ed6, 0x8de83c5d8f641ca2, 
	0x6b657695bbd80000, 0x74f525ddb1047fc , 0xadd6fcafc691114b, 0xd542787bec5dc3c4, 
	0x32d4f199f138b819, 0xf8c049bc68585ea6, 0x2e93e2f65f176ebe, 0x67489e28d3880228, 
	0xad04c593e5fd275f, 0x74b6d3e4ec3a37b1, 0x12a2d3e48ef9ad2f, 0x2af7e36c3db3c4  , 
	0xe373808897ba092c, 0x99a2626b0e766383, 0x523ebe7789ae133e, 0x466aedca95dd687e, 
	0x75f6a529f3fbf336, 0x9fbaa63dd18afbee, 0x52b939edd048b22c, 0xc8d655c62707e622, 
	0x10532c5d999792d3, 0x8672064062f22cb0, 0x124f0ad63aeb6b48, 0x998a3c97ee92a33f, 
	0x5e6146374cbecc2d, 0xa94ffa761094d84e, 0xa34df41145eb77e8, 0x4c137208d869dd6d, 
	0x3437cb1eb9797f44, 0x7bbbe2ff492ac41 , 0x1487879919198108, 0x6009df162e5fd99 , 
	0x237653952d0869ef, 0x1abfa534be11d757, 0x811f0b4b02600995, 0x89697772eb2bb8c2, 
	0x6c5f3366e0cd4385, 0x9894221d425c189b, 0xf550e1bf63603a2c, 0xaf9a37e029d6cf99, 
	0x3c26f86401fa1091, 0xed99f730fac3aa45, 0x441e2c2880715079, 0xe32717d42abd0c5a, 
	0x64571c9f081d416a, 0x99ecf900224002e8, 0xca539ac28f02b952, 0xab81a710426157df, 
	0x1d31c4268de6da7d, 0x321d2bf341dbf358, 0x731a343b85488892, 0xb07c095ef1c42cff, 
	0xf0b4b19b6a410d4e, 0x61337c829aa94c7 , 0x8a21d87d1e9366aa, 0x9b41d9f115ba4f24, 
	0x22c3369a9fb42682, 0x3f3e28a026c64ae , 0x4ae74fc9164ce714, 0xebd5497514cf2cf8
	},
	{
	0xee798dbef285e29d, 0x275ba43e488a890a, 0x9c7c2ae579c31dd8, 0xf1517548b3380af8, 
	0x409b289e32fb27c0, 0xb7850307b6c7dc6 , 0x5b52637177b7c5cc, 0x5c8895c808aa4358, 
	0xe7af095270a00f05, 0xb2169fce92f962fe, 0xeb59911d198d0ede, 0xcf9ed5a206552335, 
	0x4c80648e54154a49, 0xa61c6a0a033ed493, 0xbf99fdea3e8ece6e, 0xbd503ccd0e227944, 
	0x792c1fb39439d7fc, 0x7d81415fdfd7a3e9, 0xb5d148e2335d34ea, 0xff4dfa619cb1657e, 
	0x4c1481d0ddb028fc, 0x4f3f136b7ba207f7, 0x7949b88fe84b2f6e, 0xbae922d90decacaf, 
	0x9a308a1096311481, 0x6141c6b29fabcd90, 0x2b5eb2b0c01726fe, 0xdb4a84a51727474f, 
	0x34f91f1a27489331, 0xc351d1d906c7f1b7, 0x3229e6574a748fda, 0xaffaea52e4d45037, 
	0x642082099e71bbd8, 0xeb16d861710ff095, 0x362f62a20fe6f967, 0x4605496a2df1fe0e, 
	0xa610dce9469d7c4f, 0x31a4809151a139f7, 0x4548ca29d7a77a5e, 0xc8acc30549504c8a, 
	0xade70af239951379, 0xe051330dc202de09, 0xef8a10f9e8eecf6f, 0x68120944026e31f4, 
	0x700585f811bfda2c, 0x46fe84ff55729684, 0x2a4d4326dba8e042, 0x931de022389c8f60, 
	0x7ef19d514c2547c9, 0xe399c396c9a03e66, 0xc0644d534dbed8b5, 0xa165bc5836313fb3, 
	0x55dd240ffe997b57, 0x57143d0a0f42bb0e, 0x5ff42a207619ab87, 0xcaad7562494f6145, 
	0x3881e583288fa9d6, 0x14d02d33d3f1ac3a, 0xfae2d118629cafc0, 0x1fb2f04b7bd94799, 
	0x86925ca5a66f657c, 0xa1aafc7facd39943, 0xa0f7ba2b73da3325, 0xa89917b219f4eefa
	},
	{
	0xa3a5ed759033bd58, 0x884d9e18667d97a5, 0x5c957303cac47bf1, 0x68e492083a5e689, 
	0x37dca10162c31166, 0xb5c944cdbfc376e5, 0x4900f78c63f76391, 0x1f6ac42d88f3eae3, 
	0x9957ddc0f0bbeae6, 0xa652553d843e3308, 0xae6c33e9c5e8f53e, 0xa1aaffbcc77c0bb4, 
	0x470d3db55e6a0cee, 0xc8e030f9e65bd92f, 0x7a41b9ac97edffd7, 0x1a23751f3f45c8af, 
	0xc1f26e7a603721d , 0x10991c5999e5d4d7, 0x48f711ccd11e308f, 0xb162b50ca46009e , 
	0x2e0aa5ef94a00010, 0x51c8abc9a645da6e, 0xc0fdb240c886f0fd, 0xb42b9ed8a3f2ed92, 
	0x229ec40c7b7c69  , 0x9fc5831c64bd4d42, 0xb2c73c2aa891a7b7, 0x7fba26967e67f143, 
	0xa8493ede1313c32b, 0xcd6c3f2d5a6f5094, 0x97bc3f356e6f8879, 0x6adf2c6cdf482831, 
	0x8f22a4eb1376c762, 0xe11c1bb455bfe1fd, 0x2759d9e929c95e8b, 0x19b8808a72564e73, 
	0x6e015465ef2a289a, 0xad354d07843c5090, 0xb6bf2e9d3a80b139, 0x23f6d6e782b3357b, 
	0x9b2e9b246112105e, 0xcad2af3c9b863720, 0x548e12edf98bb575, 0xbf6e03ec598c984b, 
	0x1452b331a1e4b8d3, 0xd7dda6bea8216e3 , 0x512eeb8f2aa7b2b , 0xc7311700e06157c8, 
	0x64716f9d4d8c8192, 0x25cb0a5a8b4a2918, 0x16541289e3e04ef8, 0x64172e19ebeef343, 
	0x20dd08d449bbf066, 0xda354f9c5ce3760 , 0x1e95b1630418b5c2, 0xd3d6bcd49116e7f6, 
	0xd2bfa6a51861b3e , 0x7d5ca64eedf7f752, 0xe422e717e4d46d75, 0x29c7fba4492f40d3, 
	0xdbb823207577c1ff, 0x91af9cc1d81d7974, 0xb68d94f550688f61, 0x5974b0a571cfcb2a
	},
	{
	0x1b810b8612130eca, 0x640c7f815afbb8ce, 0x8ccf8d203ee017a9, 0xc3cb88c9acea53f5, 
	0xb635bf842956be42, 0x9acd5fedeea607c9, 0xd58940edff8b6605, 0x8197fda7bd18e954, 
	0x29cc455842d4c8d6, 0xa2b42aed0a4aba00, 0xabbdb252ed07551e, 0x193b8eb98cef5302, 
	0x78ab9a8ed484ca8f, 0xf20c489367236f1 , 0x23cc23aa9318eff0, 0x1b1f278f3805771f, 
	0xdae4da923081f595, 0xb9714f3c606eba4c, 0x9adbb3a7b3bab887, 0x600110a7f421e0ab, 
	0x9684bb36e7726a1a, 0x5cfc64b791c50a94, 0x873ec9ff8430aa11, 0x6277e578103b63ce, 
	0xac045d70a3dd2f1e, 0x807190023ea7976 , 0x204813cad99283aa, 0x507f70f13a4d0fb9, 
	0x2a5ccd349e379717, 0xe46dbd8a55baa926, 0x5826f29752a6110f, 0x210977e1a70cdd9d, 
	0xe321ccea2deac79e, 0x2b3f7ed1261fe6bd, 0x7d2e16603c968eee, 0x4f23531b1aa57e74, 
	0xbbec94f0aef2bd18, 0x19131ccd7909b0e1, 0x682ee85936034322, 0x930c3cec28b43cb2, 
	0x3c5f4573db675791, 0xcfe5b4423b23e8de, 0xe6db181154d9439b, 0x79eb0b6ffd0c3256, 
	0x583652e6ea2eec02, 0x25891220af5a608f, 0x7d0270ad0494071c, 0xf987491d9a5dd53f, 
	0x5488eaaed1268543, 0x68e30907dca8cfb1, 0x1696b4d031069533, 0x3428a93fb5b23fe3, 
	0xde954b0d7c70da57, 0x9d1b643809730e9e, 0x1232da39ecf44088, 0x69e00a814ad19f31, 
	0xec2a0e0fc6f5a411, 0xb958f8af3af2d2d2, 0xe1ec3b50e1163b2e, 0x9040a3295797743b, 
	0xe8c3edc2f4312721, 0x839d0db4898964ed, 0x1d57ae03ead0b3e , 0x68f022f4186e1b1
	}
};

u64 SideKey = 0xfd0922733836cf03;

u64 CastleKeys[16] = {
	0xa271f20c5f536223,
	0x45ecc414d1821985,
	0xe3662d7e3db5b240,
	0x13f14b151cf7dd37,
	0xffbd0532a54bdd8a,
	0x39d598c37f65efed,
	0x7e9a4b3a06ad6bf0,
	0x5df0d0d976463a72,
	0x3eb441b1a3099f0e,
	0x9acfa9330a03cc1a,
	0x8a795a451563ad9a,
	0x333a6707f483a41c,
	0x12154027abf7b6bd,
	0xa0912ec3561dbabc,
	0xf3371d88c41c355c,
	0xb2ea157f529bf64f
};


u8 FilesBrd[64];
u8 RanksBrd[64];

S_OPTIONS EngineOptions[1];

 
static void InitFilesRanksBrd(){
	u8 sq, file, rank;
	
	for(rank = RANK_1; rank <= RANK_8; rank++){
		for(file = FILE_A; file <= FILE_H; file++){
			sq = FR2SQ(file, rank);
			FilesBrd[SQ64(sq)] = file;
			RanksBrd[SQ64(sq)] = rank;
		}
	}
	
}

static void InitSq120To64(){
	
	u8 index = 0;
	u8 file = FILE_A;
	u8 rank = RANK_1;
	u8 sq = A1;
	u8 sq64 = 0;
	
	for(index = 0; index < BRD_SQ_NUM; index++) Sq120ToSq64[index] = OFF_BOARD;
	for(index = 0; index < 64; index++) Sq64ToSq120[index] = 120;
	
	for(rank = RANK_1; rank <= RANK_8; rank++){
		for(file = FILE_A; file <= FILE_H; file++){
			sq = FR2SQ(file, rank);
			Sq120ToSq64[sq] = sq64;
			Sq64ToSq120[sq64] = sq;
			sq64++;
		}
	}

}

/**
 * Initializing all used information arrays.
 *
 */
void AllInit(){
	
	InitSq120To64();
	InitPolyBook();
	InitFilesRanksBrd();
	
}

