#include "IGNUM01_AUTH_DECODER.h"
#include <cstring>
#include <string>

IGNUM IGNUM;

String User_Group = "docmac0522105v1418df4v15v4df8 hellodarkenssmyoldfriend ivecometotalktoyouagain andinthedarkens"; // has to be loaded by file 
String Root_Group = "docmac0522105v1418df4v15v4df8 ivecometotalktoyouagain"; // has to be loaded by file

void setup() {

  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
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


String Commands[16] = {"HELP", "ROOT?", "PINOUT", "NEWTASK", "POWERPINON", "POWERPINOFF"}; 

int Last_Commands_NUM() {
  int result = 0;
  for (int i = 0; i != 17; i++){
    if (Commands[i] != ""){
      result++;
      } else {
      break;  
      }
        
    }
  return result;
}


String HelpWhatCommands(){
  String suboutput = "";
  for (int k = 0; k != Last_Commands_NUM(); k++){
    
    suboutput += Commands[k] + "\n";
    
  }
return suboutput;
}

String inputCommand(bool allowed){

  // syntax = filterCommand(InputPlainCode(plain_requisition_package));

  String RootKey, Command, Cond1, Cond2, Cond3, command_response;

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
                 
            for(int X = 0; X != Last_Commands_NUM(); X++){
               if (strstr(Command.c_str(),Commands[X].c_str())){ /// used that for an non identified error, need to remake this part later;
                  switch(X+1){
                    case 1:
                      return HelpWhatCommands();
                    break;
                  
                    case 2:
                      return RootKey;
                    break;
                  
                    case 5:
                      digitalWrite(2, 0); //DEBUG
                      return "Pinout";
                    break;
                    
                    case 6:
                      digitalWrite(2, 1);//DEBUG
                      return "shutded";
                    break;
                  
                  
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
  
