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
      String begin(String reboot); // 1
      void reload(); // 2
      bool InputPlainCode(String inputPack); // 3
  
      String addUser(String credential, String level, String user);
      String ResetUsers(String credential);
      void setChaosFactor(String input);
};


#endif
