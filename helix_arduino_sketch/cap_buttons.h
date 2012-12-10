#include "CapacitiveSensor.h"

// CapSense
const int __pad_count__ = 21;
const int __locus_pad_count__ = 9;
const int __org_pad_count__ = 6;

CapacitiveSensor pads[__pad_count__] = {
  // Side Pads - 6, 7 -- top to bottom
  CapacitiveSensor(6, 22), // 1
  CapacitiveSensor(7, 23), // 2
  CapacitiveSensor(7, 24), // 3
  CapacitiveSensor(6, 25), // 4
  CapacitiveSensor(7, 26), // 5
  CapacitiveSensor(6, 27), // 6
  // Bottom Pads - 47, 48, 49, 50 -- left to right
  CapacitiveSensor(48, 36), // 15
  CapacitiveSensor(48, 35), // 14
  CapacitiveSensor(49, 34), // 13
  CapacitiveSensor(49, 33), // 12
  CapacitiveSensor(47, 32), // 11
  CapacitiveSensor(47, 31), // 10
  CapacitiveSensor(50, 30), // 9 -- aha, test
  CapacitiveSensor(50, 29), // 8
  CapacitiveSensor(50, 28), // 7
  // Control Pads - 51, 52, 53
  CapacitiveSensor(52, 37), // 16 -- bottom left -- reset
  CapacitiveSensor(51, 38), // 17 -- top inner left -- competition
  CapacitiveSensor(53, 39), // 18 -- top inner right -- selection
  CapacitiveSensor(52, 40), // 19 -- top outer left -- step
  CapacitiveSensor(53, 41), // 20 -- bottom right -- mutation rate
  CapacitiveSensor(51, 42) // 21 -- top outer right -- inheritance
};

bool OrganismPads[6] = {false, false, false, false, false, false};
bool LocusPads[9] = {false, false, false, false, false, false, false, false, false};
bool MutationRatePad = false;
bool ResetPad = false;
bool CompetitionPad = false;
bool SelectionPad = false;
bool InheritancePad = false;
bool StepPad = false;

void update_button_state() {
  long start = millis();
  
  long totals[__pad_count__];
  
  for (int i = 0; i < __pad_count__; i++) {
    totals[i] = pads[i].capacitiveSensor(30);
  }
  /*
  for (int i = 0; i < __pad_count__; i++) {
    Serial.print("Pad ");
    Serial.print(i + 1);
    Serial.print(" - ");
    Serial.println(totals[i]);
  }
  Serial.println(""); */
  
  // Organism Pads
  for (int i = 0; i < 6; i++) {
    if (totals[i] > 500 || totals[i] == -2) { OrganismPads[i] = true; }
    else { OrganismPads[i] = false; }
  }
  
  // Allele Pads
  for (int i = 0; i < 9; i++) {
    if (totals[i + 6] > 500 || totals[i + 6] == -2) { LocusPads[i] = true; }
    else { LocusPads[i] = false; }
  }
  
  // Reset Pad
  if (totals[15] > 500 || totals[15] == -2) { ResetPad = true; }
  else { ResetPad = false; }
  
  // Mut Rate Pad
  if (totals[19] > 500 || totals[19] == -2) { MutationRatePad = true; }
  else { MutationRatePad = false; }
  
  // Competition Pad
  if (totals[16] > 500 || totals[16] == -2) { CompetitionPad = true; }
  else { CompetitionPad = false; }  

  // Selection Pad
  if (totals[17] > 500 || totals[17] == -2) { SelectionPad = true; }
  else { SelectionPad = false; }

  // Inheritance Pad
  if (totals[20] > 500 || totals[20] == -2) { InheritancePad = true; }
  else { InheritancePad = false; }

  // Step Pad
  if (totals[18] > 500 || totals[18] == -2) { StepPad = true; }
  else { StepPad = false; }  
}
