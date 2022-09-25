#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>
#include "IGNUM01_AUTH_DECODER.h"
#include <FS.h>
#include "Base64.h"
#include <ESP8266WiFi.h>

SHA256 sha256, userhash;

bool ValidCommand = 0, configmode = 0;
int last_User, last_Root_user;
byte UserSessionid[32], Challenge[32], BChaos_Factor[16];
String STRKeyChallenge, UsersHash[32], ValidTokens[32], Users[32], RootUsers[32], CommandList[5], User_Group, Root_Group, Chaos_Factor = "";

void IGNUM::setChaosFactor(String input){Chaos_Factor = input;}

void REMOVEFILE(String path){SPIFFS.remove(path);}

void WRITETOFILE(String state, String path) { 
  
  SPIFFS.remove(path);
  File rFile = SPIFFS.open(path,"w+"); 
  rFile.print(state);
  rFile.close();
}
 
String READFILE(String path) {
    
  File rFile = SPIFFS.open(path,"r");
  
  if (!rFile) {
       WRITETOFILE("", path);
  }
 
  String content = rFile.readStringUntil('\n'); //desconsidera '\r\n'
  rFile.close();
  return content;
}

/*
String encrypt(String plain_data, String SymKey, String Vector){
      
      // AES CBC Encryption
      //tool from Kakopappa (from Github).
      //Modified by DocMac.

      const char* Key_Man = SymKey.c_str();
      const char* IV_Man = Vector.c_str();

      int len = plain_data.length();
      int n_blocks = len / 16 + 1;
      uint8_t n_padding = n_blocks * 16 - len;
      uint8_t data[n_blocks*16];
      memcpy(data, plain_data.c_str(), len);
      for(int i = len; i < n_blocks * 16; i++){
        data[i] = n_padding;
      }
      
      uint8_t key[16], iv[16];
      memcpy(key, Key_Man, 16);
      memcpy(iv, IV_Man, 16);

      // encryption context
      br_aes_big_cbcenc_keys encCtx;

      // reset the encryption context and encrypt the data
      br_aes_big_cbcenc_init(&encCtx, key, 16);
      br_aes_big_cbcenc_run( &encCtx, iv, data, n_blocks*16 );

      // Base64 encode
      len = n_blocks*16;
      char encoded_data[ base64_enc_len(len) ];
      base64_encode(encoded_data, (char *)data, len);
      
      return String(encoded_data);
}

*/
String decrypt(String encoded_data_str, String SymKey, String Vector){  

      // AES CBC Decryption
      //tool from Kakopappa (from Github).
      //Modified by DocMac.

      const char* Key_Man = SymKey.c_str();
      const char* IV_Man = Vector.c_str();
      
      int input_len = encoded_data_str.length();
      char *encoded_data = const_cast<char*>(encoded_data_str.c_str());
      int len = base64_dec_len(encoded_data, input_len);
      uint8_t data[ len ];
      base64_decode((char *)data, encoded_data, input_len);
      
      uint8_t key[16], iv[16];
      memcpy(key, Key_Man, 16);
      memcpy(iv, IV_Man, 16);

      int n_blocks = len / 16;

      br_aes_big_cbcdec_keys decCtx;

      br_aes_big_cbcdec_init(&decCtx, key, 16);
      br_aes_big_cbcdec_run( &decCtx, iv, data, n_blocks*16 );

      uint8_t n_padding = data[n_blocks*16-1];
      len = n_blocks*16 - n_padding;
      char plain_data[len + 1];
      memcpy(plain_data, data, len);
      plain_data[len] = '\0';

      return String(plain_data);
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
  
  if (Chaos_Factor == "" or Chaos_Factor == " "){
    Chaos_Factor = "youhavetosetsomethingherebeforetheBEGINv65h41gm11gh54151m65gh1mn5gh5nm1gh964n1";
  }
  RNG.begin(Chaos_Factor.c_str()); //key to start RNG;
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
  RNG.rand(Challenge, 16); // 16 now, 16 from the Chaos_Factor;

  Chaos_Factor.getBytes(BChaos_Factor, 16);
  
  for (int i = 1; i < 17; i++){
    Challenge[i+16] = BChaos_Factor[i];   
  }
  
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

bool IGNUM::InputCyphercode(String CypherCode){

  if (!CypherCode.startsWith("IG01")){
    return InputPlainCode(CypherCode);
  } else {
    //thats where magic happens!
    CypherCode.remove(0,4);
    
    for (int i = 0; i != last_User; i++){

      String DecCypherCode = decrypt(CypherCode, ValidTokens[i], STRKeyChallenge); //InitVector = Challenge

      if (strstr(DecCypherCode.c_str(), ValidTokens[i].c_str())){
          return InputPlainCode(DecCypherCode);
      }
    }
  }
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
