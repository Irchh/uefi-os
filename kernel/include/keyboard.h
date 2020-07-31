#include <efihelper.h>

char FromKeyCode(uint64_t code);
void KeyPressed(uint64_t code);
void InitKeyboard();
void AddKey(uint64_t key);
uint64_t ReadKey();