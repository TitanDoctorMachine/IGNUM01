#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>
#include "IGNUM01_AUTH_DECODER.h"
#include <FS.h>

SHA256 sha256, userhash;

bool ValidCommand = 0, configmode = 0;
int last_User, last_Root_user;
byte UserSessionid[32], Challenge[32];
String STRKeyChallenge, UsersHash[32], ValidTokens[32], Users[32], RootUsers[32], CommandList[5], User_Group, Root_Group;

void REMOVEFILE(String path){SPIFFS.remove(path);}

void WRITETOFILE(String state, String path) { 
  
  SPIFFS.remove(path);
  File rFile = SPIFFS.open(path,"w+"); 
  rFile.print(state);
  rFile.close();
}
 
int try_read = 0;

String READFILE(String path) {
    
  File rFile = SPIFFS.open(path,"r");
  
  if (!rFile) {
       WRITETOFILE("", path);
       try_read = 0;
  }
 
  String content = rFile.readStringUntil('\n'); //desconsidera '\r\n'
  rFile.close();
  return content;
}

String loadUsers(){
  
        if (User_Group == ""){
          User_Group = "CONFIG_MODE";
          Root_Group = "CONFIG_MODE";
          configmode = 1;
          }
          
        String loadedUsers = User_Group + " \0"; //syntax_input correction;
        String loadedUsersRoot = Root_Group + " \0"; //syntax_input correction;
           
        //LOAD NORMAL USERS ARRAY
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
        
        //LOAD ROOT USERS ARRAY
        int CountNumber1 = 0;
        while (loadedUsersRoot.length() > 0){  
            int index = loadedUsersRoot.indexOf(' ');
            if (index == -1){
            RootUsers[CountNumber1] = loadedUsersRoot; // load to user[32]
            break;
            } else {
            RootUsers[CountNumber1] = loadedUsersRoot.substring(0, index); // load to user[32]
            loadedUsersRoot = loadedUsersRoot.substring(index+1);
            }
         CountNumber1++;
         }
         last_Root_user = CountNumber1;

    if (configmode){
    return "Config_Mode!";
    } else {
    return "All_OK!";
    }
  
}

String IGNUM::begin(String reboot){
  
  RNG.begin("fdbgorenjgkrng"); //CHANGE TO HARDWARE KWY
  RNG.rand(Challenge, 10);
  SPIFFS.begin();
  //uncomment this part and re-write the firmware in case of self locking outside;
  //REMOVEFILE("/USERS.txt");
  //REMOVEFILE("/ROOTUSERS.txt");
  User_Group = READFILE("/USERS.txt");
  Root_Group = READFILE("/ROOTUSERS.txt");
  if(reboot != "reboot"){  
  reload();
  }
  
  return loadUsers();
        
}

void IGNUM::SustainLoop(){ // LOOP FUNCTION
    RNG.loop();
   }

String NewChallenge() { //GENERATE NEW CHALLENGE
  
  String internalSTRKeyChallenge = "";
  RNG.rand(Challenge, 32); // i wanna see chaos
  
  sha256.reset();
  sha256.update(Challenge, 32);
  sha256.finalize(Challenge, 32); //RELOAD 256 HASH TO 256 RAND GENERATED

  for (int i = 0; i != 32; i++) {
    if(Challenge[i] < 0x10) {
      internalSTRKeyChallenge += '0';
    }
    internalSTRKeyChallenge += String (Challenge[i], HEX);
    }
    STRKeyChallenge = internalSTRKeyChallenge; 
    return internalSTRKeyChallenge;  
}


void ValidateLoadedUsers(){ //VALIDATE LOADED USERS WITH KEYCHALLENGE
   

  for(int i = 0; i < last_User; i++){
    
    UsersHash[i] = "";
     
    const char* usernow = Users[i].c_str();
    userhash.reset();
    userhash.update(usernow, strlen(usernow));
    userhash.finalize(UserSessionid, 32);

    String UserSessionidstr;
    for (int x = 0; x < 32; x++) {
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
      
      for (int x = 0; x < 32; x++) {
      if(UserSessionid[x] < 0x10) {
        ValidTokens[i] += '0';
      }
        ValidTokens[i] += String (UserSessionid[x], HEX);
      }
    
  
  Serial.print("User: ");//DEBUG
  Serial.println(Users[i]);//DEBUG
  Serial.print("UserHash: ");//DEBUG
  Serial.println(UsersHash[i]);//DEBUG
  Serial.print("UserToken: ");//DEBUG
  Serial.println(ValidTokens[i]);//DEBUG  
  
  
  }  
  
}



bool IGNUM::InputPlainCode(String inputPack){ //INPUT PLAIN CODE
 
  ///Syntax = ValidUserToken+//+command+//+cond1+//+cond2+//+cond3+//+SALT 

  CommandList[0] = "";
  String PlainPackage[6];
  bool OUTPUT_Function = 0;
  String User_From_Command;
  int CountNumberCommand = 0;
  bool Accepeted = false;
  bool root_permission = false;
  
  inputPack.replace("//", " ");
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

       if (Command != " "){ //Bug Fixing;
          Accepeted = true;
       }
      
      User_From_Command = Users[i];

        for (int y = 0; y != last_Root_user+1; y++){        
            if (User_From_Command == RootUsers[y]){
              root_permission = true;
            }
        }
     }
  }
  
  if (Accepeted){

      OUTPUT_Function = 1;
      
      if (root_permission){
      CommandList[1] = "root:true";
      } else {
      CommandList[1] = "root:false";
      }           
      CommandList[2] = Command; 
      CommandList[3] = Condition_1; 
      CommandList[4] = Condition_2;
      CommandList[5] = Condition_3;
      
     } else {
       OUTPUT_Function = 0;
     }
        
    return OUTPUT_Function;

}

void IGNUM::reload(){
  
  loadUsers();
  NewChallenge();
  ValidateLoadedUsers();
  
}

String IGNUM::addUser(String credential, String level, String user){
    
    bool valid = true;
    String User_Group0, Root_Group0;

    if(credential != "root:true"){
      valid = 0;
      return "NO_ROOT!";
    }
      
    for(int i = 0; i != last_User; i++){
      if (strstr(user.c_str(),Users[i].c_str())){
          valid = 0;                  
          return "USER_ALREADY_EXIST!";
        }}
         
        
        
    if (valid){ //and (user != " " or "") 
        
        if (level == "NORMAL"){
          if (configmode){
          User_Group0 = user;
          WRITETOFILE(User_Group0,"/USERS.txt");
          } else{
          User_Group0 = READFILE("/USERS.txt");
          if (User_Group0 != ""){
          User_Group0 += " ";
          }
          User_Group0 += user;
          WRITETOFILE(User_Group0,"/USERS.txt");  
          }
        } 
        else if (level == "ROOT"){
        if (configmode){
          Root_Group0 = user;
          User_Group0 = user;
          WRITETOFILE(User_Group0,"/USERS.txt");
          WRITETOFILE(Root_Group0,"/ROOTUSERS.txt");
          } else{
          User_Group0 = READFILE("/USERS.txt");
          Root_Group0 = READFILE("/ROOTUSERS.txt");
          
          if (User_Group0 != ""){
          User_Group0 += " ";
          }
          if (Root_Group0 != ""){
          Root_Group0 += " ";
          }
          Root_Group0 += user;
          User_Group0 += user;
          WRITETOFILE(User_Group0,"/USERS.txt");
          WRITETOFILE(Root_Group0,"/ROOTUSERS.txt");
         }
        
       } 
      
        // ESP.restart();

       if (level == "HELP"){
          return "SYNTAX: ADDUSER//NORMAL(or ROOT)//NEWUSERNAME";
       }
    
       begin("reboot");
       return "OK!";
    }
}

String IGNUM::ResetUsers(String credential){ //for fail-safe uses it will erase all parts;
    
   if(credential == "root:true"){

     REMOVEFILE("/USERS.txt");
     REMOVEFILE("/ROOTUSERS.txt");
        
         
      return "WARNING! ERASING ALL USERS AND ROOT USERS! \nALL CHANGES WILL BE APPLIED IN THE NEXT BOOT, IN CONFIG MODE!";   
            
     } else {
      return "NO_ROOT!";
    }
}



//COMMANDS TO LOAD VALUES FROM COMMANDS
void IGNUM::EndRxCommand(){
  for(int y = 0; y != 6; y++){
  CommandList[y] = " ";
  }
}

bool IGNUM::GetRxValid() {return ValidCommand;} 
String IGNUM::GetRxRootKey(){return CommandList[1];}
String IGNUM::GetRxCommand(){return CommandList[2];}
String IGNUM::GetRxCondit1(){return CommandList[3];}
String IGNUM::GetRxCondit2(){return CommandList[4];}
String IGNUM::GetRxCondit3(){return CommandList[5];}
String IGNUM::GetChallenge() {return STRKeyChallenge;}
