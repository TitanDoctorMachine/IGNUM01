#ifndef IGNUM01_AUTH_DECODER_H
#define IGNUM01_AUTH_DECODER_H

#include <Arduino.h>

class IGNUM{
      public:
            
      bool GetRxValid();
      String GetRxRootKey();
      String GetRxCommand();
      String GetRxCondit1();
      String GetRxCondit2();
      String GetRxCondit3();
      void EndRxCommand();

      bool InputPlainCode(String inputPack);
      void ValidateLoadedUsers();
      void loadUsers(String usrsFile, String rootFile);
      String NewChallenge();
      void begin();
      void SustainLoop();

};


#endif
