#include "IGNUM01_AUTH_DECODER.h"
#include <cstring>
#include <string>

IGNUM IGNUM;

//need to be self generated in next versions
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

  //Serial.println("\n---------------------------------DEBUG_MODE---------------------------------\n ");

  IGNUM.NewChallenge();
  IGNUM.loadUsers(User_Group, Root_Group);
  IGNUM.ValidateLoadedUsers();

  //Serial.println("\n---------------------------------DEBUG_MODE---------------------------------\n ");
  
  Serial.println(" ");
  Serial.println("ChallengeKey:");
  Serial.println(IGNUM.GetChallenge());
    
}

String Commands[16] = {"HELP", "ROOT?"}; //Native commands;

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

  //Syntax = return(inputCommand(IGNUM.InputPlainCode(plain_requisition_package)));
 
  String RootKey, Command, Cond1, Cond2, Cond3, command_response;

  RootKey = IGNUM.GetRxRootKey();
  Command = IGNUM.GetRxCommand();
  Cond1 = IGNUM.GetRxCondit1();
  Cond2 = IGNUM.GetRxCondit2();
  Cond3 = IGNUM.GetRxCondit3();
  IGNUM.EndRxCommand();     

    if(!allowed) {
    return "Access_Denied!";
    } else if (allowed = 1){
           String Refined_Result = "Access_Granted >> ";
                 
           for(int X = 0; X != Last_Commands_NUM(); X++){
               if (strstr(Command.c_str(),Commands[X].c_str())){ /// used that for an non identified error, need to remake this part later;
                  switch(X+1){
                    case 1:
                      //HELP
                      Refined_Result += "\n" + HelpWhatCommands();
                      return Refined_Result;
                    break;
                  
                    case 2:
                      //ROOT?
                      Refined_Result += RootKey;
                      return Refined_Result;
                    break;

                    case 3:
                      // Can be added as user's wish;
                    
                    break;
                  
                  
               }
            }
          } 
          
    Refined_Result += "Syntax_Error!";
    return Refined_Result;
    } 

}

  
  
  
//Next Versions need to add assymetrical crypto to command, pub key will be in main page, and will need to encrypted for receiving commands;  
    
void GeneratePUBKEY(){
  
}

void InputCypherCode(){
  
}
  
