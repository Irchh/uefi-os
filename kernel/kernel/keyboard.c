#include <keyboard.h>

char CharMap[98*2];
char key_state[98] = {};
uint64_t buffer[0x100];
uint8_t len = 0;
uint8_t pos = 0;

void InitKeyboard() {
	len = 0; pos = 0;
	for (int i = 0; i < 0x100; ++i) buffer[i] = 0;
	for (int i = 0; i < sizeof(key_state); ++i) key_state[i] = 0;

	char* str = "1234567890";
	for (int i = 2; i < 12; ++i) CharMap[i] = str[i-2];

	str = "!\"#4%&/()=";
	for (int i = 2; i < 12; ++i) CharMap[i+98] = str[i-2];

	str = "qwertyuiop";
	for (int i = 16; i < 26; ++i) {
		CharMap[i] = str[i-16];
		CharMap[i+98] = str[i-16]-0x20;
	}

	str = "asdfghjkl";
	for (int i = 30; i < 39; ++i) {
		CharMap[i] = str[i-30];
		CharMap[i+98] = str[i-30]-0x20;
	}

	str = "zxcvbnm";
	for (int i = 44; i < 51; ++i) {
		CharMap[i] = str[i-44];
		CharMap[i+98] = str[i-44]-0x20;
	}

	str = ",.-";
	for (int i = 51; i < 54; ++i)
		CharMap[i] = str[i-51];

	str = ";:_";
	for (int i = 51; i < 54; ++i)
		CharMap[i+98] = str[i-51];

	CharMap[12] = '+';
	CharMap[12+98] = '?';
	CharMap[13] = '\\';
	CharMap[28] = '\n';
	CharMap[28+98] = '\n';
	CharMap[41] = '|';
	CharMap[43] = '\'';
	CharMap[43+98] = '*';
	CharMap[57] = ' ';
	CharMap[57+98] = ' ';
	CharMap[86] = '<';
	CharMap[86+98] = '>';
}

void AddKey(uint64_t key) {
	buffer[(pos+len)&0xFF] = key;
	pos -= (len == 0xFF);
	len += (len != 0xFF);
}

uint64_t ReadKey() {
	if (len == 0)
		return 0;
	pos++;
	len--;
	return buffer[pos-1];
}

void KeyPressed(uint64_t code) {
	int released = !((code&128)==128);
	key_state[code-(code&128)] = released;
}

char FromKeyCode(uint64_t code) {
	return CharMap[code+98*(key_state[42]|key_state[54])];
}