#include "console.H"

int main() {
    Console::puts("Kernel started!\n");  // Debugging output
    Console::puts("Initializing console...\n");
    Console::init(); 
    Console::puts("Console initialized!\n");
    Console::puts("WELCOME TO MY KERNEL!\n");
    Console::puts("      AMALESH ARIVANAN\n");
    for(;;);  // Infinite loop
}
