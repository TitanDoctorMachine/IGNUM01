#include "SpartanFileManager.h"
#include "FS.h"

String SpartanLoadFile(String InputFile){

  bool fail_file, fail_auth;
  String inFile;
  String inFileAuthCopy;
  String inFileBKP1;
  String inFileBKPOLD;
  String Output;
  
 File OBJinFile = SPIFFS.open(InputFile, "r"); //Read Main File
 File OBJinFileAuthCopy = SPIFFS.open(InputFile + ".AuthCopy", "r"); //Read Auth File
 inFile = OBJinFile.readStringUntil('\r'); //desconsidera '\r\n'
 inFileAuthCopy = OBJinFileAuthCopy.readStringUntil('\r'); //desconsidera '\r\n'
 OBJinFile.close();
 OBJinFileAuthCopy.close();     
      
    Serial.println(inFile); //DEBUG
    Serial.println(inFileAuthCopy); //DEBUG

      if(inFile == inFileAuthCopy){
        //integrity check
        inFile = Output;
        Serial.println("deu bom"); //DEBUG

        } else {
          
         File OBJinFileBKP1 = SPIFFS.open(InputFile + ".BKP1", "r"); //Read Backup File
         inFileBKP1 = OBJinFileBKP1.readStringUntil('\r'); //desconsidera '\r\n'
         OBJinFileBKP1.close();

         Serial.println(inFileBKP1); //DEBUG
     
          //inFile input fault
          if (!OBJinFile && inFileBKP1 == inFileAuthCopy){

              Serial.println("failed in load infile"); //DEBUG
              
              SPIFFS.remove(InputFile);
              File WOBJinFile = SPIFFS.open(InputFile, "w+"); //fix fail
              WOBJinFile.println(inFileBKP1);
              WOBJinFile.close();
              inFileBKP1 = Output;

             } else {
               fail_file = 1;
               }
          
          //inFileAuthCopy input fault
          if (!OBJinFileAuthCopy && inFileBKP1 == inFile){

              Serial.println("failed in load authfile"); //DEBUG
              
              SPIFFS.remove(InputFile + ".AuthCopy");
              File WOBJinFileAuthCopy = SPIFFS.open(InputFile + ".AuthCopy", "w+"); //fix fail
              WOBJinFileAuthCopy.println(inFileBKP1);
              WOBJinFileAuthCopy.close();
              inFileBKP1 = Output;
                           
             } else {
               fail_auth = 1;
               }
           
        }  
         
          if (fail_file && fail_auth){
          File OBJinFileBKPOLD = SPIFFS.open(InputFile + ".BKPOLD", "r"); //Read Auth File
          inFileBKPOLD = OBJinFileBKPOLD.readStringUntil('\r'); //desconsidera '\r\n'
          OBJinFileBKPOLD.close();  

             Serial.println(inFileBKPOLD); //DEBUG
             
             if (OBJinFileBKPOLD){
             SpartanWriteFile(InputFile, inFileBKPOLD);
             Output = inFileBKPOLD;
             
             } else {
               SPIFFS.remove(InputFile);
               SPIFFS.remove(InputFile + ".AuthCopy");
               SPIFFS.remove(InputFile + ".BKP1");
               SPIFFS.remove(InputFile + ".BKPOLD");

               File WDUMMYOBJ1 = SPIFFS.open(InputFile, "w+"); //fix fail
               WDUMMYOBJ1.println("");
               WDUMMYOBJ1.close();
               
               File WDUMMYOBJ2 = SPIFFS.open(InputFile + ".AuthCopy", "w+"); //fix fail
               WDUMMYOBJ2.println("");
               WDUMMYOBJ2.close();
               
               File WDUMMYOBJ3 = SPIFFS.open(InputFile + ".BKP1", "w+"); //fix fail
               WDUMMYOBJ3.println("");
               WDUMMYOBJ3.close();
               
               File WDUMMYOBJ4 = SPIFFS.open(InputFile + ".BKPOLD", "w+"); //fix fail
               WDUMMYOBJ4.println("");
               WDUMMYOBJ4.close();
               
               Output = "FATAL_ERROR! RELOAD DEFAULT VALUES MANUALLY, ERASING OLD_FILES FOR FAIL-SAFE MECHANISM";
               }
             }
          
  return Output;

}

void SpartanWriteFile(String File_to, String Input){

       SPIFFS.remove(File_to);
       SPIFFS.remove(File_to + ".AuthCopy");
       SPIFFS.remove(File_to + ".BKP1");
  
       File WINFOOBJ = SPIFFS.open(File_to, "w+"); //fix fail
       WINFOOBJ.println(Input);
       WINFOOBJ.close();
       Serial.println("loaded_base"); //DEBUG

       
       File WINFOOBJAUTH = SPIFFS.open(File_to + ".AuthCopy", "w+"); //fix fail
       WINFOOBJAUTH.println(Input);
       WINFOOBJAUTH.close();
       Serial.println("loaded_auth"); //DEBUG
       
      
       File WINFOOBJBKP = SPIFFS.open(File_to + ".BKP1", "w+"); //fix fail
       WINFOOBJBKP.println(Input);
       WINFOOBJBKP.close();
       Serial.println("loaded_bkp"); //DEBUG

       
       File OBJinFileBKPOLD = SPIFFS.open(File_to + ".BKPOLD", "r"); //Read BKPOLD File
       if(!OBJinFileBKPOLD){       
             
           File WINFOOBJOLD = SPIFFS.open(File_to + ".BKPOLD", "w+"); //fix fail
           WINFOOBJOLD.println(Input);
           WINFOOBJOLD.close();
           Serial.println("loaded_old"); //DEBUG

       
       }

       OBJinFileBKPOLD.close();
       
}

void SpartanRemoveFile(String File_to){
  SPIFFS.remove(File_to);
  SPIFFS.remove(File_to + ".AuthCopy");
  SPIFFS.remove(File_to + ".BKP1");
  SPIFFS.remove(File_to + ".BKPOLD");
}
