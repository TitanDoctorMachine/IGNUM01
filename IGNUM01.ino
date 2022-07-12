#include "FS.h"
#include "IGNUM01_AUTH_DECODER.h"
//#include "SpartanFileManager.h"

IGNUM IGNUM;

String User_Group = "docmac0522105v1418df4v15v4df8 hellodarkenssmyoldfriend ivecometotalktoyouagain andinthedarkens"; // has to be loaded by file 
String Root_Group = "docmac0522105v1418df4v15v4df8 ivecometotalktoyouagain"; // has to be loaded by file
  

void setup() {

  Serial.begin(115200);
  Serial.println(" ");
  IGNUM.begin();
  IGNUM_RELOAD();

}

void loop() {
  
  delay (5);
  IGNUM.SustainLoop(); //to RNG function
  String Termial = Serial.readString();
  if ( Termial != ""){ 
  Serial.println(" ");  
  Serial.println(InputCommand(IGNUM.InputPlainCode(Termial)));
  IGNUM_RELOAD();
  }

}

void IGNUM_RELOAD() {
  
  Serial.println(" ");
  Serial.println("ChallengeKey:");
  String outerSTRKeyChallenge = IGNUM.NewChallenge();
  Serial.println(outerSTRKeyChallenge);

  IGNUM.loadUsers(User_Group, Root_Group);
  IGNUM.ValidateLoadedUsers();
  
}



String InputCommand(bool allowed){ // syntax == InputCommand(InputPlainCode(requisition_package));
  
  String command_response = "";

  String RootKey = IGNUM.GetRxRootKey();
  String Command = IGNUM.GetRxCommand();
  String Cond1 = IGNUM.GetRxCondit1();
  String Cond2 = IGNUM.GetRxCondit2();
  String Cond3 = IGNUM.GetRxCondit3();
  IGNUM.EndRxCommand();     

    /// FROM THIS PART DOWN NEED TO REMAKE
    
    
    if (allowed == 1){

           Serial.println(Command);
           
           //if (Command == "ROOT?"){
           //  return RootKey;
           // }
      
           return "Syntax_Error!";
            
    } else {
               
    return "Access_Denied!";
       
    }

}

  

void GeneratePUBKEY(){
  
}

void InputCypherCode(){
  
}
  
