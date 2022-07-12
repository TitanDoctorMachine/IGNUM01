#ifndef IGNUM01_AUTH_DECODER_H
#define IGNUM01_AUTH_DECODER_H

#include <Arduino.h>

class IGNUM{
      public:

      String GetChallenge();
      
      bool GetRxValid();
      String GetRxRootKey();
      String GetRxCommand();
      String GetRxCondit1();
      String GetRxCondit2();
      String GetRxCondit3();
      void EndRxCommand(); //CLEAN LOADED COMMAND

      void SustainLoop(); //LOOP
      void begin(); // 1
      String NewChallenge(); // 2
      void loadUsers(String usrsFile, String rootFile); // 3
      void ValidateLoadedUsers(); // 4
      bool InputPlainCode(String inputPack); // 5

};


#endif
