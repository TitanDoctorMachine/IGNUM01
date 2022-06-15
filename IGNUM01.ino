#include <SHA256.h>
#include <Crypto.h>
#include <RNG.h>

SHA256 sha256, userhash;

byte UserSessionid[32], Challenge[32];
String KeyChallenge;

void setup() {

  RNG.begin("teste");
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
  sha256.finalize(Challenge, 32); //dá a saída hash na própria var -- 32 by == 256 bits
  for (int i = 0; i != 32; i++) {
    KeyChallenge += String (Challenge[i], HEX);
    }
    Serial.println("ChallengeKey:");
    Serial.println(KeyChallenge);
}

void validateuser(){
  
  byte* UserAuth = Challenge;
  //UserAuth += '00c95efe198f454eadb6811831eac0d93b8c12476bd88b558abfb68b1251d2ba'; // DOCMAC012021111123654825362425623
 
  //Debug
  String UserAuthStr;
  for (int i = 0; i != 32; i++) {
    UserAuthStr += String (UserAuth[i], HEX);
    }
    Serial.println("key-to-hash DocMac:");
    Serial.println(UserAuthStr);
    
  userhash.reset();
  userhash.update(UserAuth, 32);
  userhash.finalize(UserSessionid, 32);
  String UserSessionidstr;
  for (int i = 0; i != 32; i++) {
    UserSessionidstr += String (UserSessionid[i], HEX);
    }
    int UserSessionidint = UserSessionidstr.toInt();
    Serial.println("TokenAuth DocMac:");
    Serial.println(UserSessionidstr);
  
  
  }
