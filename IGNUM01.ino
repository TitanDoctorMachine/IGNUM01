#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>
#include <cstring>

SHA256 sha256, userhash;

byte UserSessionid[32], Challenge[32];
String STRKeyChallenge;


void setup() {

  RNG.begin("teste"); //mudar para uma hardware key
  RNG.rand(Challenge, 10);
  Serial.begin(115200);
  Serial.println(" ");
  newchallenge();
  validateuser();
}

void loop() {
  RNG.loop();
}

void newchallenge() {
  
  Serial.println(" ");
  RNG.rand(Challenge, 32); //gera pseudo random de 32 bytes
  sha256.reset();
  sha256.update(Challenge, 32);
  //const char* maluco = "you are not close enough to Gauss";
  //sha256.update(maluco, strlen(maluco));
  sha256.finalize(Challenge, 32); //dá a saída hash na própria var -- 32 by == 256 bits

  for (int i = 0; i != 32; i++) {
    if(Challenge[i] < 0x10) {
      STRKeyChallenge += '0';
    }
    STRKeyChallenge += String (Challenge[i], HEX);
  }
     
    Serial.println("ChallengeKey:");

    Serial.println(STRKeyChallenge);

}

void validateuser(){
  
  String user = "00c95efe198f454eadb6811831eac0d93b8c12476bd88b558abfb68b1251d2ba";
  String Auth = STRKeyChallenge + user;
 
  const char* UserAuth = Auth.c_str(); // DOCMAC012021111123654825362425623
  
  //Debug
    
    Serial.println("key-to-hash DocMac:");
    Serial.println(UserAuth);
      
  userhash.reset();
  userhash.update(UserAuth, strlen(UserAuth));
  userhash.finalize(UserSessionid, 32);
    
  String UserSessionidstr;
  for (int i = 0; i != 32; i++) {
    if(UserSessionid[i] < 0x10) {
      UserSessionidstr += '0';
    }
    UserSessionidstr += String (UserSessionid[i], HEX);
    }
    
    Serial.println("TokenAuth DocMac:");
    Serial.println(UserSessionidstr);
  
  
  }
