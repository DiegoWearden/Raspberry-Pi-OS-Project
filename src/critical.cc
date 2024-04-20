#include "critical.h"

Atomic<int> critical_flag{0};
volatile uint32_t critical_depth = 0;
volatile uint32_t critical_owner = uint32_t(-1);
