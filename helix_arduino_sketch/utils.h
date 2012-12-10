int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

// defaults
bool __using_delays__ = true;
void conditional_delay(int wait) {
  if (__using_delays__) {
    delay(wait);
  }
} 
