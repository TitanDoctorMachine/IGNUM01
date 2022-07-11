#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>
#include "FS.h"
#include "SpartanFileManager.h"


SHA256 sha256, userhash;

int last_User, last_Root_user;
byte UserSessionid[32], Challenge[32];
String STRKeyChallenge, UsersHash[32], ValidTokens[32], Users[32], RootUsers[32];

String User_Group = "docmac0522105v1418df4v15v4df8 hellodarkenssmyoldfriend ivecometotalktoyouagain andinthedarkens"; // has to be loaded by file 
String Root_Group = "docmac0522105v1418df4v15v4df8 ivecometotalktoyouagain"; // has to be loaded by file
  

void setup() {

  SPIFFS.begin();
  RNG.begin("teste"); //mudar para uma hardware key
  RNG.rand(Challenge, 10);
  Serial.begin(115200);
  Serial.println(" ");

  Start_New_Session();
}

void loop() {
  
  delay (5);
  RNG.loop();

  String Termial = Serial.readString();
  if ( Termial != ""){ 
  String result = InputPlainCode(Termial);
  Serial.println(result);
  
  Start_New_Session();
  }


}

void Start_New_Session() {

  Serial.println(" ");
  Serial.println("ChallengeKey:");
  STRKeyChallenge = NewChallenge();
  Serial.println(STRKeyChallenge);

  loadUsers(User_Group, Root_Group);
  ValidateLoadedUsers();
  
  }


String NewChallenge() {
  
  String internalSTRKeyChallenge = "";
  RNG.rand(Challenge, 32); //gera pseudo random de 32 bytes // i wanna see chaos
  
  sha256.reset();
  sha256.update(Challenge, 32);
  sha256.finalize(Challenge, 32); //dá a saída hash na própria var -- 32 by == 256 bits

  for (int i = 0; i != 32; i++) {
    if(Challenge[i] < 0x10) {
      internalSTRKeyChallenge += '0';
    }
    internalSTRKeyChallenge += String (Challenge[i], HEX);
    }
     
    return internalSTRKeyChallenge;  
}


void loadUsers(String usrsFile, String rootFile){

  String loadedUsers = usrsFile + " \0"; //correção de syntax_input
  String loadedUsersRoot = rootFile + " \0"; //correção de syntax_input
     
  //load normal users
  int CountNumber = 0;
  while (loadedUsers.length() > 0){  
      int index = loadedUsers.indexOf(' ');
      if (index == -1){
      Users[CountNumber] = loadedUsers; // load to user[32]
      break;
      } else {
      Users[CountNumber] = loadedUsers.substring(0, index); // load to user[32]
      loadedUsers = loadedUsers.substring(index+1);
      //Serial.println("users:");
      //Serial.println(Users[CountNumber]); DEBUG
      }
   CountNumber++;
   }
   last_User = CountNumber;
  
  
  
  //load root usersRootUsers
  int CountNumber1 = 0;
  while (loadedUsersRoot.length() > 0){  
      int index = loadedUsersRoot.indexOf(' ');
      if (index == -1){
      RootUsers[CountNumber1] = loadedUsersRoot; // load to user[32]
      break;
      } else {
      RootUsers[CountNumber1] = loadedUsersRoot.substring(0, index); // load to user[32]
      loadedUsersRoot = loadedUsersRoot.substring(index+1);
      //Serial.println("Root:");
      //Serial.println(RootUsers[CountNumber1]); DEBUG
      }
   CountNumber1++;
   }
   last_Root_user = CountNumber1;
   
}



void ValidateLoadedUsers(){
  
  //Generate Users
 
  for(int i = 0; i < last_User; i++){
    
    UsersHash[i] = "";
     
    const char* usernow = Users[i].c_str();
    userhash.reset();
    userhash.update(usernow, strlen(usernow));
    userhash.finalize(UserSessionid, 32);

    String UserSessionidstr;
    for (int x = 0; x != 32; x++) {
      if(UserSessionid[x] < 0x10) { 
         UsersHash[i] += '0';
      }
      UsersHash[i] += String (UserSessionid[x], HEX);
    }    

    //Generate Token
    
      String Auth = STRKeyChallenge + UsersHash[i];
    
      const char* UserAuth = Auth.c_str();
      
      userhash.reset();
      userhash.update(UserAuth, strlen(UserAuth));
      userhash.finalize(UserSessionid, 32);
      
      ValidTokens[i] = "";
      
      for (int x = 0; x != 32; x++) {
      if(UserSessionid[x] < 0x10) {
        ValidTokens[i] += '0';
      }
        ValidTokens[i] += String (UserSessionid[x], HEX);
      }
    
  Serial.print("User: ");
  Serial.println(Users[i]);
  Serial.print("UserHash: ");//DEBUG
  Serial.println(UsersHash[i]);//DEBUG
  Serial.print("UserToken: ");//DEBUG
  Serial.println(ValidTokens[i]);//DEBUG  
  
  
  }  
  
}



void InputCypherCode(){
  
  }


String InputPlainCode(String inputPack){

  String PlainPackage[5];
  String OUTPUT_Function;
  String User_From_Command;
  int CountNumberCommand = 0;
  bool Accepeted = false;
  bool root_permission = false;
  
  inputPack = " " + inputPack; //correção de syntax_input
     
  while (inputPack.length() > 0)
   {  int index = inputPack.indexOf(' ');
      if (index == -1){
      PlainPackage[CountNumberCommand] = inputPack;
      break;
      } else {
      PlainPackage[CountNumberCommand] = inputPack.substring(0, index);
      inputPack = inputPack.substring(index+1);
      }
    CountNumberCommand++;
    }
   
  String ReceivedKeyChallenge = PlainPackage[1]; 
  String Command = PlainPackage[2]; 
  String Condition_1 = PlainPackage[3]; 
  String Condition_2 = PlainPackage[4]; 
  String Condition_3 = PlainPackage[5]; 
    
  for (int i = 0; i != last_User; i++){
     if (ValidTokens[i] == ReceivedKeyChallenge){

      Accepeted = true;

      OUTPUT_Function += "\n"; //DEBUG
      
      User_From_Command = Users[i];

        for (int y = 0; y != last_Root_user+1; y++){        
            if (User_From_Command == RootUsers[y]){
              root_permission = true;
            }
        }
     }
  }
  
  if (Accepeted){
      
      if (root_permission){
      OUTPUT_Function += "root:true ";
      } else {
      OUTPUT_Function += "root:false ";
      }           
      OUTPUT_Function += "Acces_Granted ";
      OUTPUT_Function += Command; 
      OUTPUT_Function += " "; 
      OUTPUT_Function += Condition_1; 
      OUTPUT_Function += " ";
      OUTPUT_Function += Condition_2;
      OUTPUT_Function += " ";
      OUTPUT_Function += Condition_3;
      
     } else {
       OUTPUT_Function += "Acces_Denied";
     }

    return OUTPUT_Function;

}


/*
String InputCommand(String inputPack){

  String PlainPackage[6];
  String OUTPUT_Function;
  String User_From_Command;
  int CountNumberCommand = 0;
  bool Accepeted = false;
  bool root_permission = false;
  
  inputPack = " " + inputPack; //correção de syntax_input
     
  while (inputPack.length() > 0)
   {  int index = inputPack.indexOf(' ');
      if (index == -1){
      PlainPackage[CountNumberCommand] = inputPack;
      break;
      } else {
      PlainPackage[CountNumberCommand] = inputPack.substring(0, index);
      inputPack = inputPack.substring(index+1);
      }
    CountNumberCommand++;
    }
   
  String ReceivedKeyChallenge = PlainPackage[1]; 
  String Command = PlainPackage[2]; 
  String Condition_1 = PlainPackage[3]; 
  String Condition_2 = PlainPackage[4]; 
  String Condition_3 = PlainPackage[5]; 
    

*/



void GeneratePUBKEY(){
  
  }
