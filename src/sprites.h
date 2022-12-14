#include <fxcg/display.h>

color_t SPRITE_SEGMENT[128] = {
	0x0447,0x0447,0x0447,0x0447,0x0447,0x0447,0x0447,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x1d6a,0x0447,
	0x0447,0x0447,0x0447,0x0447,0x0447,0x0447,0x0447,0x0447
};

color_t SPRITE_HEAD[128] = {
	0x1252,0x1252,0x1252,0x1252,0x1252,0x1252,0x1252,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x2b35,0x1252,
	0x1252,0x1252,0x1252,0x1252,0x1252,0x1252,0x1252,0x1252
};

color_t SPRITE_APPLE[512] = {
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xf7be,0x6207,0xf7de,0x25a9,0x55ed,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xf7be,0x6207,0x1da9,0x1da9,0x1da9,0x5dee,0x5dee,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xf7be,0x6207,0x1da9,0x4dec,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xf7be,0x6207,0x1da9,0x4dec,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xf7be,0x6207,0x4dec,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xf966,0xf966,0xf966,0x6207,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,
	0xffff,0xffff,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,0xffff,
	0xffff,0xffff,0xffdf,0xffdf,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffdf,0xf966,0xf966,0xf966,0xf966,0xf966,0xf966,0xffff,0xffff,0xffff,0xffff,0xffff
};

color_t SPRITE_SKULL[512] = {
	0xffff,0xffff,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xffff,0xffff,
	0xffff,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xffff,
	0xa0c3,0xa0c3,0xa0c3,0xa0c3,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xa0c3,0xa0c3,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0xa0c3,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x0000,0xa0c3,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x0000,0xa0c3,0xa0c3,
	0xa0c3,0x0000,0xffff,0xffff,0x0000,0x0000,0xffff,0xffff,0xffff,0xffff,0x0000,0x0000,0xffff,0xffff,0x0000,0xa0c3,
	0xa0c3,0x0000,0xffff,0x0000,0x0000,0x0000,0x0000,0xffff,0xffff,0x0000,0x0000,0x0000,0x0000,0xffff,0x0000,0xa0c3,
	0xa0c3,0x0000,0xffff,0x0000,0x0000,0x0000,0x0000,0xffff,0xffff,0x0000,0x0000,0x0000,0x0000,0xffff,0x0000,0xa0c3,
	0xa0c3,0x0000,0xffff,0xffff,0x0000,0x0000,0xffff,0xffff,0xffff,0xffff,0x0000,0x0000,0xffff,0xffff,0x0000,0xa0c3,
	0xa0c3,0xa0c3,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x0000,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0xa0c3,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x0000,0xa0c3,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0xa0c3,0xa0c3,0x0000,0xffff,0x0000,0xffff,0xffff,0x0000,0xffff,0x0000,0xa0c3,0xa0c3,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0xa0c3,0xa0c3,0x0000,0xffff,0x0000,0xffff,0xffff,0x0000,0xffff,0x0000,0xa0c3,0xa0c3,0xa0c3,0xa0c3,
	0xa0c3,0xa0c3,0xa0c3,0xa0c3,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xa0c3,0xa0c3,0xa0c3,0xa0c3,
	0xffff,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xffff,
	0xffff,0xffff,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xa0c3,0xffff,0xffff
};

