#include "IGNUM01_AUTH_DECODER.h"
#include <cstring>

IGNUM IGNUM;

String User_Group = "docmac0522105v1418df4v15v4df8 hellodarkenssmyoldfriend ivecometotalktoyouagain andinthedarkens"; // has to be loaded by file 
String Root_Group = "docmac0522105v1418df4v15v4df8 ivecometotalktoyouagain"; // has to be loaded by file

void setup() {

  Serial.begin(115200);
  Serial.println(" ");
  Serial.println("Starting_System...");
  IGNUM.begin();
  IGNUM_RELOAD();

}

void loop() {
  
  IGNUM.SustainLoop(); //to RNG function
  
  String Termial = Serial.readString();
  
  if ( Termial != ""){ 
      Serial.println(" ");  
      Serial.println(inputCommand(IGNUM.InputPlainCode(Termial)));
      IGNUM_RELOAD();
  }

  delay (100);

}

void IGNUM_RELOAD() {

  Serial.println("\n---------------------------------DEBUG_MODE---------------------------------\n ");

  IGNUM.NewChallenge();
  IGNUM.loadUsers(User_Group, Root_Group);
  IGNUM.ValidateLoadedUsers();

  Serial.println("\n---------------------------------DEBUG_MODE---------------------------------\n ");
  
  Serial.println(" ");
  Serial.println("ChallengeKey:");
  Serial.println(IGNUM.GetChallenge());
    
}



String inputCommand(bool allowed){

  // syntax = filterCommand(InputPlainCode(plain_requisition_package));
  String Commands[16] ={"ROOT?", "PINOUT", "NEWTASK"}; 

  String RootKey, Command, Cond1, Cond2, Cond3, command_response, teste;

  teste = "FOI?";
  RootKey = IGNUM.GetRxRootKey();
  Command = IGNUM.GetRxCommand();
  Cond1 = IGNUM.GetRxCondit1();
  Cond2 = IGNUM.GetRxCondit2();
  Cond3 = IGNUM.GetRxCondit3();
  IGNUM.EndRxCommand();     

    /// FROM THIS PART DOWN NEED TO REMAKE
    if(!allowed) {
    return "Access_Denied!";
    }
    
    else if (allowed = 1){

           //DEBUGGING
           Serial.println("Access_Granted!");
           Serial.println(" ");
           //Serial.println(Command);
           //Serial.println(Cond1);
           //Serial.println(Cond2);
           //Serial.println(Cond3);
                 
            for(int X = 0; X != 17; X++){
              if (Commands[X] != NULL){
               if (strstr(Command.c_str(),Commands[X].c_str())){ /// used that for an non identified error, need to remake this part later;
                  switch(X+1){
                    case 1:
                      return RootKey;
                    break;
                  
                    case 2:
                      return "Pinout";
                    break;
                  
                  }
               }
            }
          } 
          
    return "Syntax_Error!";
    } 

}
  

void GeneratePUBKEY(){
  
}

void InputCypherCode(){
  
}
  
