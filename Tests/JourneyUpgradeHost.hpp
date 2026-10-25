#pragma once
// Test-only model of original GuiGame event -> tick lifecycle; draw is read-only.
// The product UI remains authoritative; iOS tests compare this state with it.
struct JourneyUpgradeHost {
 float x=-240;
 int step=0;
 bool visible=false;
 void open(){visible=true;step=1;x=-240;}
 void close(){visible=false;step=2;}
 bool tick() {
  if(!step)return false;
  x+=((step==1?240.f:660.f)-x)/5;
  if(step==2&&x>659){step=0;return true;}
  return false;
 }
 // A deliberate controller wait for the opening slide to settle, not an
 // original input restriction: native OK is enabled throughout opening.
 bool readyForConfirmation()const{return step==1&&x>239;}
};
