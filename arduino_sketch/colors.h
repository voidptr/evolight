/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

/*

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
} 

// converting to RGB from hsv
// h represents 0-360 via 256 gradiations.
// s respresents 0-1 via 256 gradiations.
// v represents 0-1 via 256 gradiations.
uint32_t ToRGB(byte h, byte s, byte v)
{
  byte r = 0, g = 0, b = 0;
  
  float H, S, V, F, M, N, K;
  int I;
  H = h / 256;
  S = s / 256;
  V = v / 256;
  
  if (s == 0) {
    r = v;
    g = v;
    b = v;
  } else {
    H = H * 6;
    I = int(H);
    F = H - I;
    
    M = V * (1 - S);
    N = V * (1 - S * F);
    K = V * (1 - S * (1 - F));

    if (I == 0) { r = V; g = K; b = M; }
    if (I == 1) { r = N; g = V; b = M; }
    if (I == 2) { r = M; g = V; b = K; }
    if (I == 3) { r = M; g = N; b = V; }
    if (I == 4) { r = K; g = M; b = V; }
    if (I == 5) { r = V; g = M; b = N; }
  }
  
  return Color(r, g, b);    
} */
