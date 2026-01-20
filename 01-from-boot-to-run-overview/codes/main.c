
#include <stdio.h>

int main() {
    int x = 10 + 20;           // ← CPU executes addition directly
    printf("result: %d\n", x);   // ← Needs system call (write to terminal)
    return 0;
}
