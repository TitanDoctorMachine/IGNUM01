#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>
#include "FS.h"

SHA256 sha256, userhash;

int last_User;
byte UserSessionid[32], Challenge[32];
String STRKeyChallenge, UsersHash[32], ValidTokens[32];

String Users[32] = {"docmac0522105v1418df4v15v4df8","hellodarkenssmyoldfriend","ivecometotalktoyouagain", "andinthedarkens"};
  

void setup() {

  SPIFFS.begin();
  RNG.begin("teste"); //mudar para uma hardware key
  RNG.rand(Challenge, 10);
  Serial.begin(115200);
  Serial.println(" ");

  Start_New_Session();
}

void loop() {
  
  delay (500);
  RNG.loop();

  String Termial = Serial.readString();
  if ( Termial != ""){ 
  //InputPlainCode(" info_to_transmitt_buay " + ValidTokens[3] + " liga_ssa_porra_aê_mermão" ); //teste
  String result = InputPlainCode(Termial);
  Serial.println("\n" + result);
  
  Start_New_Session();
  }


}

void Start_New_Session() {

  Serial.println(" ");
  Serial.println("ChallengeKey:");
  STRKeyChallenge = NewChallenge();
  Serial.println(STRKeyChallenge);
  
  ValidateLoadedUsers();
  
  }


String NewChallenge() {
  
  String internalSTRKeyChallenge = "";
  RNG.rand(Challenge, 32); //gera pseudo random de 32 bytes
  
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



String loadFile(String InputFile){
  
  String inFile;
  String inFileAuthCopy;
  String inFileBKP1;
  String inFileBKPOLD;
  String Output;
  
  inFile = SPIFFS.open(InputFile, "r"); //Read Main File
  inFileAuthCopy = SPIFFS.open(InputFile + ".AuthCopy", "r"); //Read Auth File
      
      if(inFile == inFileAuthCopy){
        //integrity check
        inFile = Output;
        
        } else {
          
          inFileBKP1 = SPIFFS.open(InputFile + ".BKP1", "r"); //Read Backup File

          //inFile input fault
          if (!inFile){
               if(inFileBKP1 == inFileAuthCopy){
               inFileBKP1 = SPIFFS.open(InputFile, "w"); //fix fail
               inFileBKP1 = Output;
               }
              } 
          
          //inFileAuthCopy input fault
          if (!inFileAuthCopy){
               if(inFileBKP1 == inFile){
               inFile = SPIFFS.open(InputFile + ".AuthCopy", "w"); //fix fail
               inFile = Output;
               }
             } 
          
          //failed to load backup1
          if (!inFileBKP1){
             inFileBKPOLD = SPIFFS.open(InputFile + ".BKPOLD", "r"); //Read Backup File
             ///TEM QUE SE ELABORAR DESTA PARTE PRA FRENTE......  
               
             } 
          

        }
      
      
      
      

  
    
  return Output;

}











void loadUsers(String usrsFile, String rootFile){

  String loadedUsers;
  String fileToWrite;

    
 
    // open the file in write mode
    fileToWrite = SPIFFS.open("/myfile.txt", "w"); //write file
      if (!fileToWrite) 
      {
        //caso der ruim
      }
    
    // conseguimos abrir
    while(testFile.available()) 
    {
      String line = testFile.readStringUntil('\n');
    }
 

  loadedUsers.close();
  fileToWrite.close();
  
}



void ValidateLoadedUsers(){

  Serial.println(" ");
  
  //Load last User
  
  for(int i = 0; i < sizeof(Users)/sizeof(Users[0]); i++){
    if (Users[i] == "\0"){
      last_User = i;
      i = sizeof(Users)/sizeof(Users[0]);
      }  
  }
  
  
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
  /*  
  Serial.print("User: ");
  Serial.println(Users[i]);
  Serial.print("UserHash: ");//DEBUG
  Serial.println(UsersHash[i]);//DEBUG
  Serial.print("UserToken: ");//DEBUG
  Serial.println(ValidTokens[i]);//DEBUG  
  */
  
  }  
  
}



void InputCypherCode(){
  
  }


String InputPlainCode(String inputPack){

  String PlainPackage[6];
  String OUTPUT_Function;
  String User_From_Command;
  int CountNumber = 0;
  bool Accepeted = false;
  
  inputPack = " " + inputPack; //correção de syntax_input
     
  while (inputPack.length() > 0)
   {  int index = inputPack.indexOf(' ');
      if (index == -1){
      PlainPackage[CountNumber++] = inputPack;
      break;
      } else {
      PlainPackage[CountNumber++] = inputPack.substring(0, index);
      inputPack = inputPack.substring(index+1);
      }
    }
   
  String ReceivedKeyChallenge = PlainPackage[1]; 
  String Command = PlainPackage[2]; 
  String Condition_1 = PlainPackage[3]; 
  String Condition_2 = PlainPackage[4]; 
    
  for (int i = 0; i != last_User; i++){
    if (ValidTokens[i] == ReceivedKeyChallenge){
      
      User_From_Command = Users[i];
      Accepeted = true;
      
      OUTPUT_Function += "Acces_Granted ";
      OUTPUT_Function += Command; 
      OUTPUT_Function += " "; 
      OUTPUT_Function += Condition_1; 
      OUTPUT_Function += " ";
      OUTPUT_Function += Condition_2;
          
    } else {
      OUTPUT_Function = "";
      } 
  }
  if (!Accepeted){
    OUTPUT_Function += "Acces_Denied";
    }

    return OUTPUT_Function;

}


void GeneratePUBKEY(){
  
  }
