#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>

SHA256 sha256, userhash;

int last_User;
byte UserSessionid[32], Challenge[32];
String STRKeyChallenge, UsersHash[32], ValidTokens[32], PlainPackage[6];

String Users[32] = {"docmac0522105v1418df4v15v4df8","hellodarkenssmyoldfriend","ivecometotalktoyouagain", "andinthedarkens", "andiknow!", "itwillcome", "forsometime", "andicantbermorehnfindjkvn", "ndijvndinvir", "ndfvieriv", "iknfivnr"};
  

void setup() {

  RNG.begin("teste"); //mudar para uma hardware key
  RNG.rand(Challenge, 10);
  Serial.begin(115200);
  Serial.println(" ");
  NewChallenge();
  GenValidatUsers();
  GenValidTokens(); 

}

void loop() {
  delay (500);
  RNG.loop();

  String Termial = Serial.readString();
  if ( Termial != ""){ 
  //InputPlainCode(" info_to_transmitt_buay " + ValidTokens[3] + " liga_ssa_porra_aê_mermão" ); //teste
  InputPlainCode(Termial);
  }


}

void NewChallenge() {
  
  Serial.println(" ");
  RNG.rand(Challenge, 32); //gera pseudo random de 32 bytes
  
  sha256.reset();
  sha256.update(Challenge, 32);
  sha256.finalize(Challenge, 32); //dá a saída hash na própria var -- 32 by == 256 bits

  for (int i = 0; i != 32; i++) {
    if(Challenge[i] < 0x10) {
      STRKeyChallenge += '0';
    }
    STRKeyChallenge += String (Challenge[i], HEX);
    }
     
    Serial.println("ChallengeKey:");
    Serial.println(STRKeyChallenge);
    Serial.println(" ");


}

void GenValidatUsers(){

  for(int i = 0; i < sizeof(Users)/sizeof(Users[0]); i++){
    if (Users[i] == "\0"){
      last_User = i;
      i = sizeof(Users)/sizeof(Users[0]);
      }  
  }
  
  for(int i = 0; i < last_User; i++){
   
     
    const char* usernow = Users[i].c_str();
    userhash.reset();
    userhash.update(usernow, strlen(usernow));
    userhash.finalize(UserSessionid, 32);

    String UserSessionidstr;
    for (int x = 0; x != 32; x++) {
      if(UserSessionid[x] < 0x10) { UserSessionidstr += '0';}
      UsersHash[i] += String (UserSessionid[x], HEX);
    }    
  
  Serial.print("User: ");
  Serial.println(Users[i]);
  Serial.print("UserHash: ");
  Serial.println(UsersHash[i]);
  
  }
}


void GenValidTokens(){
  
  for(int i; i < last_User; i++){
    
    String Auth = STRKeyChallenge + UsersHash[i];
    
    const char* UserAuth = Auth.c_str();
    
    //Serial.println(UserAuth);
    userhash.reset();
    userhash.update(UserAuth, strlen(UserAuth));
    userhash.finalize(UserSessionid, 32);
    
    for (int x = 0; x != 32; x++) {
    if(UserSessionid[x] < 0x10) {
        ValidTokens[i] += '0';
    }
      ValidTokens[i] += String (UserSessionid[x], HEX);
    }
    
  //Serial.println(ValidTokens[i]);

  }  
}


void InputPlainCode(String inputPack){

  Serial.println(" ");
  Serial.println(inputPack); // Debug

  //necessário espaço antes do comp 1
  //separação dos comps da entrada na array PlainPackage
  int CountNumber = 0;
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
    
  String User_From_Command;
  bool Accepeted = false;
  
  String InfoHeader = PlainPackage[1]; 
  String ReceivedKeyChallenge = PlainPackage[2]; 
  String Message = PlainPackage[3]; 
    
  for (int i = 0; i != last_User; i++){
    if (ValidTokens[i] == ReceivedKeyChallenge){
      User_From_Command = Users[i];
      Accepeted = true;
      
      Serial.println("Acces_Granted!");
      Serial.println("Command_From:");
      Serial.println(User_From_Command);
      Serial.println("Command:");
      Serial.println(Message);
    } 
  }
  if (Accepeted == false){
    Serial.println("Acces_Denied!");
    }

}


void GeneratePUBKEY(){
  
  }


void InputCypherCode(){
  
  }
