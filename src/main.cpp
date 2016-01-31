/// EzSkinz ~ Fully External SkinChanger
/// Coded by Double V
/// Actual version: 1.0.1

#include "Essentials.h"
#include <array>
#include <string>
using Essentials::MemoryManager;
using std::array;
using std::thread;
using std::cout;
using std::cin;
using std::endl;

#pragma region OFFSETS
//Credits to Shadiku for the NetVar dump
const DWORD dwClientState = 0x6072C4;
const DWORD dwLocalPlayer = 0xA6E444;
const DWORD dwEntityList = 0x4A5C9C4;
const DWORD hActiveWeapon = 0x2EE8;
const DWORD EntLoopDist = 0x10;
const DWORD m_nFallbackPaintKit = 0x3160;
const DWORD m_nFallbackSeed = 0x3164;
const DWORD m_flFallbackWear = 0x3168; // ( float )
const DWORD m_nFallbackStatTrak = 0x316C;
const DWORD m_iItemDefinitionIndex = 0x2F80;
const DWORD m_iEntityQuality = 0x2F84;
const DWORD m_iEntityLevel = 0x2F88;
const DWORD m_iItemIDHigh = 0x2F98;
const DWORD m_iItemIDLow = 0x2F9C;
const DWORD m_iAccountID = 0x2FA0;
const DWORD m_szCustomName = 0x3014; //( char[161] )
const DWORD m_iAccount = 0xA8D8;
const DWORD m_iWeaponID = 0x32DC;
const DWORD m_OriginalOwnerXuidLow = 0x3158;
#pragma endregion

#define SKIN_KEY VK_F1
#define DEBUG_KEY VK_F2

MemoryManager* Mem = new MemoryManager();
array<int, 2> PaintKit;
array<float, 2> Wear;
array<int, 2> Seed;
array<int, 2> StatTrak;
array<char[161], 2> Name;

int WeaponID_Previous = NULL;
int WeaponID = NULL;
int WeapPrev = NULL;

void ForceFullUpdate()
{
	DWORD _dwClientState = Mem->Read<DWORD>(Mem->EngineDLL_Base + dwClientState);
	Mem->Write<int>(_dwClientState + 0x16C, -1);
}

void ResetArrays()
{
	PaintKit[0] = 0; PaintKit[1] = 0;
	Wear[0] = 0.01f; Wear[1] = 0.01f;
	Seed[0] = 0; Seed[1] = 0;
	StatTrak[0] = -1; StatTrak[1] = -1;
	strcpy(Name[0], "0"); strcpy(Name[1], "0");
}

#pragma region GUI FUNCTIONS

void PrintWelcome()
{
	/*
	cout<<"||                          ||"<<endl;
	*/
	ForceFullUpdate();
	system("CLS");
	cout<<"=============================="<<endl;
	cout<<"||    Welcome to EzSkinz.   ||"<<endl;
	cout<<"=============================="<<endl;
	cout<<"|| Credits:                 ||"<<endl;
	cout<<"|| Double V - Code          ||"<<endl;
	cout<<"|| v1c - Skins Demystified  ||"<<endl;
	cout<<"|| UnknownCheats.me         ||"<<endl;
	cout<<"=============================="<<endl;
	cout<<"|| F1 -> Set Skin Data      ||"<<endl;
	cout<<"=============================="<<endl;
}

//Thread function
void PrintDebugInfo()
{
	while (true)
	{
		if (GetAsyncKeyState(DEBUG_KEY))
		{
			system("CLS");
			cout<<"Debug informations:"<<endl;
			cout<<"----------------------"<<endl;
			cout<<"Array Values:"<<endl;
			cout<<"PaintKit[0] = "<<PaintKit[0]<<endl;
			cout<<"PaintKit[1] = "<<PaintKit[1]<<endl;
			cout<<"Wear[0] = "<<Wear[0]<<endl;
			cout<<"Wear[1] = "<<Wear[1]<<endl;
			cout<<"Seed[0] = "<<Seed[0]<<endl;
			cout<<"Seed[1] = "<<Seed[1]<<endl;
			cout<<"StatTrak[0] = "<<StatTrak[0]<<endl;
			cout<<"StatTrak[1] = "<<StatTrak[1]<<endl;
			cout<<"Name[0] = "<<Name[0]<<endl;
			cout<<"Name[1] = "<<Name[1]<<endl;
			cout<<"----------------------"<<endl;
			cout<<"WeaponID Values:"<<endl;
			cout<<"WeaponID = "<<WeaponID<<endl;
			cout<<"WeaponID_Previous = "<<WeaponID_Previous<<endl;
			cout<<"WeapPrev = "<<WeapPrev<<endl;
			cout<<"----------------------"<<endl;
			Sleep(500);
		}
		Sleep(25);
	}
}

#pragma endregion

#pragma region SKIN_CHANGER FUNCTIONS

//Thread function
void SkinChanger()
{
	bool once = false;
	while (true)
	{
		#pragma region READ INFOS ABOUT WEAPON
		DWORD Player = Mem->Read<DWORD>(Mem->ClientDLL_Base + dwLocalPlayer);
		DWORD WeaponIndex = Mem->Read<DWORD>(Player + hActiveWeapon) & 0xFFF;
		DWORD WeapEnt = Mem->Read<DWORD>((Mem->ClientDLL_Base + dwEntityList + WeaponIndex * EntLoopDist) - EntLoopDist);
		WeaponID = Mem->Read<int>(WeapEnt + m_iItemDefinitionIndex);
		int WeaponAccountID = Mem->Read<int>(WeapEnt + m_iAccountID);
		int MyAccountID = Mem->Read<int>(WeapEnt + m_OriginalOwnerXuidLow);
		#pragma endregion

		#pragma region SET WEAPONID_PREVIOUS
		if (WeaponID != WeaponID_Previous && !once) 
		{
			WeaponID_Previous = WeaponID;
			ForceFullUpdate();
			once = true;
		}
		#pragma endregion

		if (WeaponID != WeapPrev)
		{
			WeapPrev = WeaponID;
			ForceFullUpdate();
		}

		#pragma region FORCES THE FALLBACK VALUES OF THE WEAPON

		//Force ItemIDLow = -1
		Mem->Write<int>(WeapEnt + m_iItemIDLow, -1);

		//Force the PaintKit value to be our PaintKit
		Mem->Write<int>(WeapEnt + m_nFallbackPaintKit, PaintKit[WeaponID_Previous!=WeaponID]);

		//Force StatTrak value
		Mem->Write<int>(WeapEnt + m_nFallbackStatTrak, StatTrak[WeaponID_Previous!=WeaponID]);

		//Force Seed value
		Mem->Write<int>(WeapEnt + m_nFallbackSeed, Seed[WeaponID_Previous!=WeaponID]);

		//Force Wear value
		Mem->Write<float>(WeapEnt + m_flFallbackWear, Wear[WeaponID_Previous!=WeaponID]);

		//Force Name value (this time directly with WPM)
		if (Name[WeaponID_Previous!=WeaponID][0] != '0')
		{
			WriteProcessMemory(Mem->GetProcHandle(), (LPVOID)(WeapEnt + m_szCustomName), Name[WeaponID_Previous!=WeaponID], sizeof(char[161]), NULL);
		}

		//Force AccountID to solve StatTrak problem
		Mem->Write<int>(WeapEnt + m_iAccountID, MyAccountID);

		#pragma endregion

		Sleep(10);
	}
}

//Thread function
void SetSkinData()
{
	while (true)
	{
		if (GetAsyncKeyState(SKIN_KEY))
		{
			try
			{
				system("CLS");
				cout<<"===================================================="<<endl;
				cout<<"|| List of Skin IDs: http://pastebin.com/FkkCcHaA ||"<<endl;
				cout<<"|| Set the PaintKit for the weapon                ||"<<endl;
				cout<<"===================================================="<<endl;
				//Set PaintKit
				cout<<"|| PaintKit = "; cin>>PaintKit[WeaponID_Previous!=WeaponID]; 
				cout<<"===================================================="<<endl;
				cout<<"|| Set the wear of the skin (between 0 and 1)     ||"<<endl;
				cout<<"===================================================="<<endl;
				//Set Wear
				cout<<"|| Wear = "; cin>>Wear[WeaponID_Previous!=WeaponID]; 
				cout<<"===================================================="<<endl;
				//Wear exception throw
				if (Wear[WeaponID_Previous!=WeaponID]<0||Wear[WeaponID_Previous!=WeaponID]>1) throw 1;
				cout<<"|| Set the StatTrak of the skin (-1 = No ST)      ||"<<endl;
				cout<<"===================================================="<<endl;
				//Set StatTrak
				cout<<"|| StatTrak Counter = "; cin>>StatTrak[WeaponID_Previous!=WeaponID]; 
				cout<<"===================================================="<<endl;
				cout<<"|| Set the Seed of the skin (0 if you don't care) ||"<<endl;
				cout<<"===================================================="<<endl;
				//Set Seed
				cout<<"|| Seed = "; cin>>Seed[WeaponID_Previous!=WeaponID]; 
				cin.sync();
				cout<<"===================================================="<<endl;
				cout<<"|| Set the Name of the skin (0 if you don't care) ||"<<endl;
				cout<<"===================================================="<<endl;
				//Set name
				std::string sName;
				const char* chName = nullptr;
				cout<<"|| Name = "; std::getline(cin, sName);
				if (sName == "") sName = "0";
				chName = sName.c_str();
				strcpy(Name[WeaponID_Previous!=WeaponID], chName);
				cout<<"===================================================="<<endl;
				cout<<"|| Skin data set. Returning to main menu...       ||"<<endl;
				cout<<"===================================================="<<endl;
				Sleep(1000);
				system("CLS");
				ForceFullUpdate();
				PrintWelcome();
			}
		    catch (int Ex)
			{
				switch (Ex)
				{
				case 1: //Wear exception handle
					cout<<"Invalid float value!!!"<<endl; 
					ResetArrays();
					Sleep(1000); 
					system("CLS"); 
					PrintWelcome(); 
					continue;
				break;
				}
			}
			catch (...)
			{
				cout<<"Exception not handled by the application."<<endl;
				exit(0);
			}
		}
		Sleep(25);
	}
}

#pragma endregion

int main()
{
	ResetArrays();
	PrintWelcome();

	thread DEBUG_thread = thread(PrintDebugInfo);
	thread SKINCHANGER_thread = thread(SkinChanger);
	thread SETSKIN_thread = thread(SetSkinData);

	DEBUG_thread.join();
	SKINCHANGER_thread.join();
	SETSKIN_thread.join();

	delete Mem;
	return 0;
}
