#include "IGNUM01_AUTH_DECODER.h"
#include <cstring>

IGNUM IGNUM;

String User_Group = "docmac0522105v1418df4v15v4df8 hellodarkenssmyoldfriend ivecometotalktoyouagain andinthedarkens"; // has to be loaded by file 
String Root_Group = "docmac0522105v1418df4v15v4df8 ivecometotalktoyouagain"; // has to be loaded by file

String RootKey, Command, Cond1, Cond2, Cond3;

void setup() {

  Serial.begin(115200);
  Serial.println(" ");
  IGNUM.begin();
  IGNUM_RELOAD();

}

void loop() {
  
  IGNUM.SustainLoop(); //to RNG function
  
  String Termial = Serial.readString();
  
  if ( Termial != ""){ 
      Serial.println(" ");  
      Serial.println(filterCommand(IGNUM.InputPlainCode(Termial)));
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



String filterCommand(bool allowed){

  // syntax = filterCommand(InputPlainCode(plain_requisition_package));

  String command_response = "";

  RootKey = (IGNUM.GetRxRootKey()).c_str();
  Command = (IGNUM.GetRxCommand()).c_str();
  Cond1 = (IGNUM.GetRxCondit1()).c_str();
  Cond2 = (IGNUM.GetRxCondit2()).c_str();
  Cond3 = (IGNUM.GetRxCondit3()).c_str();
  IGNUM.EndRxCommand();     

    /// FROM THIS PART DOWN NEED TO REMAKE
    
    if (allowed){

           Serial.println(Command);
           Serial.println(Cond1);
           Serial.println(Cond2);
           Serial.println(Cond3);
           command_response = inputCommand();
          
    } else {
           
    command_response = "Access_Denied!";
       
    }

    return command_response;

}


String inputCommand(){

  if (Command == "ROOT?"){

    return RootKey; 
    
    }

  else if(Command == "PINOUT"){
    
    return "pinoutbuai";
    
    }
  
  
    else {return "Syntax_Error!";}

}





  

void GeneratePUBKEY(){
  
}

void InputCypherCode(){
  
}
  
