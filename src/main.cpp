#include <matdash.hpp>

// defines add_hook to use minhook
#include <matdash/minhook.hpp>

// lets you use mod_main
#include <matdash/boilerplate.hpp>

#include <fmt/format.h>
#include <gd.h>

#include <iostream>
#include <span>
#include <format>
#include <fstream>
#include <commdlg.h>
#include <cctype>

using namespace gd;
using namespace cocos2d;

//#define USE_WIN32_CONSOLE

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET

#if USE_WIN32_CONSOLE
void println(auto t)
{
		std::cout << t << '\n';
}
#else
#define println(n)
#endif

void SaveFileWithDialog(std::string_view fileName, std::string_view fileContents) {
    OPENFILENAMEA ofn;
    char szFileName[MAX_PATH] = "";
    for(int i = 0; i < fileName.size(); i++)
    {
		szFileName[i] = fileName[i];
	}

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName);
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn) == TRUE) {
        HANDLE hFile = CreateFileA(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD bytesWritten;
            WriteFile(hFile, fileContents.data(), static_cast<DWORD>(fileContents.size()), &bytesWritten, NULL);
            CloseHandle(hFile);
        }
    }
}


std::string formatGrid(const std::set<short>& data, size_t rowLength)
{
	if(data.empty()) {
		return {};
	}
	std::string result;
	size_t row = 1;
	for(short n : data)
	{
		if(row != rowLength) {
			result += std::format("{}, ", n);
		}
		else
		{
			result += std::format("{}\n", n);
			row = 0;
		}
		row++;
	}
	while(!isdigit(result.back())) {
		result.pop_back();
	}

    return result;
}

std::string getReadyString(std::string_view levelName, const std::set<short>& used, const std::set<short>& unused)
{
	constexpr const char* FORMAT_STR = 
	"{}"
	"\n\n"
	"Free({}):"
	"\n\n"
	"{}"
	"\n\n"
	"Used({}):"
	"\n\n"
	"{}";
	size_t rows = 3;
	std::string freeStr = formatGrid(unused, rows);
	std::string usedStr = formatGrid(used, rows);
	return std::format(FORMAT_STR, levelName, unused.size(), freeStr, used.size(), usedStr);
}

void CustomSongLayer_onNewgrounds(void* self, void* sender)
{
	println("enter");
	CCArray* objects = LevelEditorLayer::get()->getAllObjects();
	if(!objects) {
		println("no objects");
		return;
	}
	int c = objects->count();
	std::set<short> used;
	std::set<short> free;
	
	for(int i = 0; i < c; i++)
	{
		GameObject* obj = reinterpret_cast<GameObject*>(objects->objectAtIndex(i));
		if(!obj) {
			println("no obj");
			continue;
		}
		std::vector<short> groups = obj->getGroupIDs();
		used.insert(groups.begin(), groups.end());
	}
	if(used.empty()) {
		return;
	}
	
	for(short i = 1; i < 1001; i++)
	{
		if(!used.contains(i)) {
			free.insert(i);
		}
	}
	std::string_view levelName = LevelEditorLayer::get()->m_pLevel->m_sLevelName;
	std::string fullStr = getReadyString(levelName, used, free);
	SaveFileWithDialog(std::format("{}-groups.txt", levelName), fullStr);
	
}


void mod_main(HMODULE)
{
	
	#ifdef USE_WIN32_CONSOLE
		if(AllocConsole()) {
			freopen("CONOUT$", "wt", stdout);
			freopen("CONIN$", "rt", stdin);
			freopen("CONOUT$", "w", stderr);
			std::ios::sync_with_stdio(1);
		}
	#endif
	println("hooking");
	matdash::add_hook<&CustomSongLayer_onNewgrounds>(base + 0x671b0);
}
