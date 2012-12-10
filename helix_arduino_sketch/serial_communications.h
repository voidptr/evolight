void serial_setup() {
      
  // init the serial
  Serial.begin(9600);
}

void print_free_memory() {
  Serial.print("Free Ram: ");
  Serial.println( freeRam() );
}
