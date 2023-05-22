#include <matdash.hpp>

// defines add_hook to use minhook
#include <matdash/minhook.hpp>

// lets you use mod_main
#include <matdash/boilerplate.hpp>

#include <fmt/format.h>
#include <gd.h>

using namespace gd;
using namespace cocos2d;

#define USE_WIN32_CONSOLE

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET

void GJBaseGameLayer_collectItem(void* self, int item, int count)
{
	item = std::clamp(item, 0, 1099);
	matdash::orig<&GJBaseGameLayer_collectItem>(self, item, count);
}

void collectedObject(void* gjbgl, void* obj)
{
	int itemBlockAID = MBO(int, obj, 0x52C);
	MBO(int, obj, 0x52C) = std::clamp(itemBlockAID, 0, 1099);
	matdash::orig<&collectedObject>(gjbgl, obj);
}

void mod_main(HMODULE) {
	
	#ifdef USE_WIN32_CONSOLE
		if(AllocConsole()) {
			freopen("CONOUT$", "wt", stdout);
			freopen("CONIN$", "rt", stdin);
			freopen("CONOUT$", "w", stderr);
			std::ios::sync_with_stdio(1);
		}
	#endif

	matdash::add_hook<&GJBaseGameLayer_collectItem>(base + 0x111890);
	matdash::add_hook<&collectedObject>(base + 0x111830);
}