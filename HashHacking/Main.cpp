#define _CRT_RAND_S

#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Wincrypt.h>
#include "Console.h"
#include "Hash.h"
#include "Rand.h"
#include "xxhash.h"

#define NUMBER_OF_THREADS 1

#define BIT_LENGTH 32
#define BYTE_LENGTH BIT_LENGTH / 8
#define TABLE_SIZE 65536	// 2 ^ (BIT_LENGTH / 2)

#define BIT_LENGTH_64 64
#define BYTE_LENGTH_64 BIT_LENGTH / 8
#define TABLE_SIZE_24 16777216	// 2 ^ 22


using namespace std;

typedef std::chrono::high_resolution_clock Clock;

struct threads {
	bool collision;
	thread::id thread_id;
};

threads T[NUMBER_OF_THREADS] = { false };
int _thread_id = 0;
int _thread_id_unique = 0;
bool found = false;
uint32_t _count = 0;
int _collision_count = 0;
bool one_second = true;

// вывести кол-во посчитанных хэшей
void showCount() {
	printf("\r %d", _count);
}
void showCount(string v1, int v2) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 7 });
	printf("\r %s: %d", v1, v2);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 6 });
}

// атака дней рождений
void birthdayAttack64(bool &found) {

	bool _found = false;

	uint32_t tab1[TABLE_SIZE_24];
	uint32_t tab2[TABLE_SIZE_24];

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[BYTE_LENGTH_64] = {};

	mt19937 gen;
	uint64_t seed = 0;
	CryptGenRandom(hProvider, BYTE_LENGTH_64, pbData);
	for (int i = 0; i < 8; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint64_t temp;
	string buf;

	do {
		// генерация таблиц
		for (int i = 0; i < TABLE_SIZE_24; i++) {
			temp = gen();
			buf = "";
			for (int i = 0; i < BYTE_LENGTH_64; i++) {
				buf += (char)((temp << 24 >> 24) & 0xFF);
				buf += (char)((temp << 16 >> 24) & 0xFF);
				buf += (char)((temp << 8 >> 24) & 0xFF);
				buf += (char)((temp >> 24) & 0xFF);
			}
			//tab1[i] = Crc32((u_char *)buf.c_str(), 32);
			tab1[i] = XXH64((u_char *)buf.c_str(), 32, 0);

			temp = gen();
			buf = "";
			for (int i = 0; i < BYTE_LENGTH_64; i++) {
				buf += (char)((temp << 24 >> 24) & 0xFF);
				buf += (char)((temp << 16 >> 24) & 0xFF);
				buf += (char)((temp << 8 >> 24) & 0xFF);
				buf += (char)((temp >> 24) & 0xFF);
			}
			//tab2[i] = Crc32((u_char *)buf.c_str(), 32);
			tab1[i] = XXH64((u_char *)buf.c_str(), 32, 0);
		}

		thread::id this_id = std::this_thread::get_id();
		T[_thread_id_unique++].thread_id = this_id;

		for (int i = 0; i < TABLE_SIZE_24; i++) {
			if (found)
				return;
			;
			_count++;
			//showCount();
			printf("\r %i", _count);
			for (int j = 0; j < TABLE_SIZE_24; j++) {
				if (tab1[i] == tab2[j]) {
					found = true;

					_collision_count++;
					showCount(" collisions found: ", _collision_count);

					T[_thread_id].collision = true;
					T[_thread_id].thread_id = this_id;
					++_thread_id;

					/*cout << "\n\r collision found. " << tab1[i] << " = " << tab2[j];
					cout << " in thread #" << T[_thread_id].thread_id << "\n";
					*/
				}
			}
		}
	} while (!found);
}
void birthdayAttack32(bool &found) {

	bool _found = false;

	uint32_t tab1[TABLE_SIZE];
	uint32_t tab2[TABLE_SIZE];

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[BYTE_LENGTH] = {};

	mt19937 gen;
	uint32_t seed = 0;
	CryptGenRandom(hProvider, BYTE_LENGTH, pbData);
	for (int i = 0; i < BYTE_LENGTH; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint32_t temp;
	string buf;
	
	// генерация таблиц
	for (int i = 0; i < TABLE_SIZE; i++) {
		buf = "";
		for (int i = 0; i < BYTE_LENGTH; i++) {
			temp = gen();
			buf += (char)((temp << 24 >> 24) & 0xFF);
			buf += (char)((temp << 16 >> 24) & 0xFF);
			buf += (char)((temp << 8 >> 24) & 0xFF);
			buf += (char)((temp >> 24) & 0xFF);
		}
		tab1[i] = Crc32((u_char *)buf.c_str(), 32);

		buf = "";
		for (int i = 0; i < BYTE_LENGTH; i++) {
			temp = gen();
			buf += (char)((temp << 24 >> 24) & 0xFF);
			buf += (char)((temp << 16 >> 24) & 0xFF);
			buf += (char)((temp << 8 >> 24) & 0xFF);
			buf += (char)((temp >> 24) & 0xFF);
		}
		tab2[i] = Crc32((u_char *)buf.c_str(), 32);
	}

	thread::id this_id = std::this_thread::get_id();
	T[_thread_id_unique++].thread_id = this_id;

	for (int i = 0; i < TABLE_SIZE; i++) {
		if (found)
			return;
			;
		_count++;
		//showCount();
		printf("\r %i", _count);
		for (int j = 0; j < TABLE_SIZE; j++) {
			if (tab1[i] == tab2[j]) {
				found = true;

				_collision_count++;
				showCount(" collisions found: ", _collision_count);

				T[_thread_id].collision = true;
				T[_thread_id].thread_id = this_id;
				++_thread_id;

				/*cout << "\n\r collision found. " << tab1[i] << " = " << tab2[j];
				cout << " in thread #" << T[_thread_id].thread_id << "\n";
				*/
			}
		}
	}
}
void birthdayAttack16(bool &found) {

	bool _found = false;

	uint16_t tab1[256];
	uint16_t tab2[256];

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[16] = {};

	mt19937 gen;
	uint16_t seed = 0;
	CryptGenRandom(hProvider, 2, pbData);
	for (int i = 0; i < 2; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint16_t temp;
	string buf;

	int _thread_id = 0;

	// генерация таблиц
	for (int i = 0; i < 256; i++) {
		temp = (uint16_t)gen();
		buf = "";
		for (int i = 0; i < 2; i++) {
			buf += (char)((temp << 24 >> 24) & 0xFF);
			buf += (char)((temp >> 8) & 0xFF);
		}
		tab1[i] = Crc16((u_char *)buf.c_str(), 16);

		temp = (uint16_t)gen();
		buf = "";
		for (int i = 0; i < 2; i++) {
			buf += (char)((temp << 24 >> 24) & 0xFF);
			buf += (char)((temp >> 8) & 0xFF);
		}
		tab2[i] = Crc16((u_char *)buf.c_str(), 16);
	}

	for (int i = 0; i < 256; i++) {
		if (found)
			break;
		_count++;
		showCount();
		for (int j = 0; j < 256; j++) {
			if (tab1[i] == tab2[j]) {
				Sleep(1);
				found = true;

				thread::id this_id = std::this_thread::get_id();
				if (T[_thread_id].collision)
					++_thread_id;
				T[_thread_id].collision = true;
				T[_thread_id].thread_id = this_id;

				/*cout << "\n\r collision found. " << tab1[i] << " = " << tab2[j];
				cout << " in thread #" << T[_thread_id].thread_id << "\n";
				*/
			}
		}
	}
}

// атака брутфорсом
void bruteforce64(bool &found, string v, uint64_t main) {

	srand(time(NULL));

	/*random_device r;
	default_random_engine e1(r());*/

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[32] = {};

	//cout << endl << " current hash: ";
	//cout << hex << main;

	//mt19937_64 gen(rd());
	//fast_srand(seed);

	mt19937_64 gen;
	uint64_t seed = 0;
	CryptGenRandom(hProvider, BYTE_LENGTH, pbData);
	for (int i = 0; i < BYTE_LENGTH; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint64_t n;
	unsigned long long hexx;

	string s;
	//for (int i = 0; i < 1000000; i++) {
	do {
		printf("\r %d", _count);
		for (int cycle = 0; cycle < 50000; cycle++) {
			s = "";
			n = gen();
			u_char* ch = new u_char();
			memcpy(ch, (char*)&n, 4);
			/*for (int i = 0; i < 4; i++) {
			s += (char)((n << 24 >> 24) & 0xFF);
			s += (char)((n << 16 >> 24) & 0xFF);
			s += (char)((n << 8 >> 24) & 0xFF);
			s += (char)((n >> 24) & 0xFF);
			}*/
			//hexx = XXH32((u_char *)s.c_str(), 16, 0);

			hexx = XXH64((u_char *)s.c_str(), 32, 0);
			//hexx = Crc32((u_char *)bytes, 32);
			hexx = Crc32((u_char *)s.c_str(), 16);
			_count++;
			//cout << "\n" << v << " " << hex << hexx;
			//printf("\n %s %d", v, hexx);

			if (main == hexx) {
				cout << endl << " found. number of checked hashes: " << _count;
				found = true;
				break;
			}
		}
	} while (!found);

	cout << " \nhash found: ";
	return;
}
void bruteforce32(bool &found, string v, uint32_t main) {

	srand(time(NULL));

	/*random_device r;
	default_random_engine e1(r());*/

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[BYTE_LENGTH] = {};

	//cout << endl << " current hash: ";
	//cout << hex << main;

	//mt19937_64 gen(rd());
	//fast_srand(seed);

	mt19937 gen;
	uint32_t seed = 0;
	CryptGenRandom(hProvider, BYTE_LENGTH, pbData);
	for (int i = 0; i < BYTE_LENGTH; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint32_t n;
	unsigned long hexx;

	string s;
	//for (int i = 0; i < 1000000; i++) {
	do {
		printf("\r %d", _count);
		//auto t1 = Clock::now();
		for (int cycle = 0; cycle < 50000; cycle++) {
			s = "";
			n = gen();

			//u_char* ch = new u_char();
			//memcpy(ch, (char*)&n, 4);

			/*for (int i = 0; i < 32; i++)
				s += (char)rand() % 256;*/

			/*for (int i = 0; i < 4; i++) {
				s += (char)((n << 24 >> 24) & 0xFF);
				s += (char)((n << 16 >> 24) & 0xFF);
				s += (char)((n << 8 >> 24) & 0xFF);
				s += (char)((n >> 24) & 0xFF);
			}*/
			//hexx = XXH32((u_char *)s.c_str(), 16, 0);

			//hexx = Crc32((u_char *)bytes, 32);
			hexx = Crc32((u_char *)s.c_str(), 16);
			//hexx = XXH32((u_char *)s.c_str(), 16, 0);
			_count++;
			//cout << "\n" << v << " " << hex << hexx;
			//printf("\n %s %d", v, hexx);

			if (main == hexx) {
				cout << endl << " found. number of checked hashes: " << _count;
				found = true;
				break;
			}
		}
		//auto t2 = Clock::now();
		//auto t0 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		//cout << t0;
	} while (!found);

	cout << " \nhash found: ";
	return;
}
void bruteforce16(bool &found, string v, uint16_t main) {

	srand(time(NULL));

	/*random_device r;
	default_random_engine e1(r());*/

	HCRYPTPROV hProvider = 0;
	CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	unsigned char pbData[2] = {};

	mt19937 gen;
	uint16_t seed = 0;
	CryptGenRandom(hProvider, 2, pbData);
	for (int i = 0; i < 2; i++) {
		seed = seed << 8;
		seed += pbData[i];
	}
	gen.seed(seed);

	uint16_t n;
	uint16_t hexx;

	string s;
	//for (int i = 0; i < 1000000; i++) {
	do {
		//printf("\r %d", _count);
		for (int cycle = 0; cycle < 50000; cycle++) {
			s = "";

			//u_char* ch = new u_char();
			//memcpy(ch, (char*)&n, 4);
			for (int i = 0; i < 2; i++) {
				n = gen();
				s += (char)((n << 8 >> 8) & 0xFF);
				s += (char)((n >> 8) & 0xFF);
			}

			hexx = Crc16(reinterpret_cast<const uint8_t*>(s.c_str()), 8);
			//hexx = Crc32((u_char *)s.c_str(), 16);
			_count++;
			//cout << "\n" << v << " " << hex << hexx;
			//printf("\n %s %d", v, hexx);

			if (main == hexx) {
				cout << endl << " found. number of checked hashes: " << dec << _count;
				found = true;
				break;
			}
		}
	} while (!found);

	cout << " \nhash found: ";

	return;
}


int main() {

	srand(time(NULL));
	ShowConsoleCursor(false);

	unsigned __int64 i = 0;

	int mode = 0;
	cout << endl << " choose mode:";
	cout << endl << " 1. birthday attack";
	cout << endl << " 2. hash bruteforce attack";
	cout << endl << " 0. exit";
	cout << endl << " >: ";
	cin >> mode;

	found = false;
	//bruteforce(found);

	std::thread thr[8];
	switch (mode) {
	case 1:
		for (int num = 0; num < NUMBER_OF_THREADS; num++) {
			//thr[num] = thread(birthdayAttack16, ref(found));
			//thr[num] = thread(birthdayAttack32, ref(found));
			thr[num] = thread(birthdayAttack64, ref(found));
			//thr[num].join();
		}
		break;
	case 2:
	{
		string str = "";
		cout << "\n    type some sort of string: \n >: ";
		cin >> str;
		//string str = "qwertyuiopasdfghjklzxcvbnm481516";
		//uint16_t main = Crc16((u_char *)str.c_str(), 8);
		uint32_t main = Crc32((u_char *)str.c_str(), 16);
		cout << " current hash: " << hex << main << "\n";

		for (int num = 0; num < NUMBER_OF_THREADS; num++) {
			//thr[num] = thread(bruteforce16, ref(found), "core " + to_string(num), main);
			thr[num] = thread(bruteforce32, ref(found), "core " + to_string(num), main);
		}
		/*thr[0] = thread(bruteforce32, ref(found), "core 1");
		thr[1] = thread(bruteforce32, ref(found), "core 2");*/

		break;
	}
	case 0:

		break;
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++) {
		Sleep(1);
		thr[i].join();
	}
	/*for (thread & t : thr) {
		Sleep(1);
		t.join();
	}*/

	/*for (i = 0; i < LLONG_MAX * 2; i++) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
		cout << i;
	}*/
	switch (mode) {
	case 1:
		for (int i = 0; i < NUMBER_OF_THREADS; i++) {
			if (T[i].collision)
				cout << "\n\r collision found" << " in thread #" << T[i].thread_id;
		}
		break;
	case 2:
		cout << "\n\r hash found at " << _count << " hash";
		break;
	default:
		break;
	}

	//system("pause");

	return 0;
}
