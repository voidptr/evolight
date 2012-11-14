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
/*
  Serial.print("h s v ");
  Serial.print(h);
  Serial.print(" ");
  Serial.print(s);
  Serial.print(" ");
  Serial.println(v);
*/  
  byte r = 0, g = 0, b = 0;
  
  float H, S, V, F, M, N, K, R, G, B;
  int I;
  H = h;
  S = s;
  V = v;
/*    
  Serial.print("H S V ");
  Serial.print(H);
  Serial.print(" ");
  Serial.print(S);
  Serial.print(" ");
  Serial.println(V);
*/  
  H = h / 256.0;
  //S = s / 256.0;
  S = (s / 128.0) + 128.0; // tends to more saturated colors
  V = v / 256.0;
/*  
  Serial.print("H S V Post");
  Serial.print(H);
  Serial.print(" ");
  Serial.print(S);
  Serial.print(" ");
  Serial.println(V);
*/  
  if (s == 0) {
    R = v;
    G = v;
    B = v;
  } else {
    H = H * 6.0;
    I = int(H);
    F = H - I;
/*    
    Serial.print("H I F");
    Serial.print(H);
    Serial.print(" ");
    Serial.print(I);
    Serial.print(" ");
    Serial.println(F);
*/    
    
    M = V * (1.0 - S);
    N = V * (1.0 - S * F);
    K = V * (1.0 - S * (1.0 - F));
/*
    Serial.print("M N K");
    Serial.print(M);
    Serial.print(" ");
    Serial.print(N);
    Serial.print(" ");
    Serial.println(K);
*/
    if (I == 0) { R = V; G = K; B = M; }
    if (I == 1) { R = N; G = V; B = M; }
    if (I == 2) { R = M; G = V; B = K; }
    if (I == 3) { R = M; G = N; B = V; }
    if (I == 4) { R = K; G = M; B = V; }
    if (I == 5) { R = V; G = M; B = N; }
  }
/*
  Serial.print("R G B ");
  Serial.print(R);
  Serial.print(" ");
  Serial.print(G);
  Serial.print(" ");
  Serial.println(B);
*/  
  r = int(R * 256.0);
  g = int(G * 256.0);
  b = int(B * 256.0);
/*
  Serial.print("r g b ");
  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);
*/  
//  uint32_t bleh = Color(h, s, v);
//  Serial.print("Using Color ");
//  Serial.println(bleh);
  
  return Color(r, g, b);    

} 
