#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "openssl/sha.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>
#include <openssl/evp.h>
using namespace std;

//COMPILE USING "$ g++ main.cpp -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto -lssl -Wall"

/*  
    * IGNUM TERMINAL, USE TO CONFIGURE AND OBTAIN INFORMATION 
    * ABOUT THE SERVER, USE 'START TO ESTABLISH CONNECTION'
*/

/*  * TO INSTALL, CREATE A USER FOLDER IN HOME FOLDER, NAMED IGNUM_CRED,
    * WITH ROOT FILLIATION, AND PERMIT ONLY THE USER THAT WILL RUN THE SOFTWARE
    * TO ACCESS THE FOLDER;
    * FILES TO BE CREATED: 
    * "/home/IGNUM_CRED/IPADDRESS >> ***.***.***.***"
    *  
    * "/home/IGNUM_CRED/USER_NATIVE >> f08acd567d8cd0bca35c79e23657a94d518b0136ab49c2223ca508c3c9b6b31c
    * BRUTE HASH OF THE REGISTERED USER: "DOCMAC_WAS_HERE52145151854148541"  
*/
string USER, globalChallenge;
string nominal_Native_IP;


static const char* B64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int B64index[256] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

const std::string b64encode(const void* data, const size_t &len)
{
    // code from https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c topic
    //thanks for all the contributors in the topic;
    
    std::string result((len + 2) / 3 * 4, '=');
    unsigned char *p = (unsigned  char*) data;
    char *str = &result[0];
    size_t j = 0, pad = len % 3;
    const size_t last = len - pad;

    for (size_t i = 0; i < last; i += 3)
    {
        int n = int(p[i]) << 16 | int(p[i + 1]) << 8 | p[i + 2];
        str[j++] = B64chars[n >> 18];
        str[j++] = B64chars[n >> 12 & 0x3F];
        str[j++] = B64chars[n >> 6 & 0x3F];
        str[j++] = B64chars[n & 0x3F];
    }
    if (pad)  /// Set padding
    {
        int n = --pad ? int(p[last]) << 8 | p[last + 1] : p[last];
        str[j++] = B64chars[pad ? n >> 10 & 0x3F : n >> 2];
        str[j++] = B64chars[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
        str[j++] = pad ? B64chars[n << 2 & 0x3F] : '=';
    }
    return result;
}



std::string b64encode(const std::string& str)
{
    return b64encode(str.c_str(), str.size());
}


string Crypt_It(string encryp, const char Plain_Key[256], const char Plain_Iv[256]){

    // code from https://stackoverflow.com/questions/4508749/aes-with-padding-pkcs7-c-code topic
    //thanks for all the contributors in the topic;
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    string plain(encryp);

    unsigned char key[16];
    unsigned char iv[16];
    memcpy(iv, Plain_Iv, 16);
    memcpy(key, Plain_Key, 16);
    assert(sizeof(key) == 16);  // AES128 key size
    assert(sizeof(iv) == 16);   // IV is always the AES block size

    std::vector<unsigned char> encrypted;
    size_t max_output_len = plain.length() + 16 - (plain.length() % 16);
    encrypted.resize(max_output_len);

    EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv, 1); // Enc is 1 to encrypt, 0 to decrypt, or -1 (see documentation).
    
    int actual_size = 0;
    EVP_CipherUpdate(ctx,
             &encrypted[0], &actual_size,
             reinterpret_cast<unsigned char *>(&plain[0]), plain.size());

    int final_size;
    EVP_CipherFinal_ex(ctx, &encrypted[actual_size], &final_size);
    actual_size += final_size;

    encrypted.resize(actual_size);

    
    string CYPHED;
    for( size_t index = 0; index < encrypted.size(); ++index )
        {
           CYPHED += encrypted[index];
        }

    EVP_CIPHER_CTX_cleanup(ctx);

    return b64encode(CYPHED);

}

string sha256(const string str)
{
    // code from https://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c topic
    //thanks for all the contributors in the topic;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string loadIPFile(){

    string IP_loaded;
    ifstream IPFILE ("/home/IGNUM_CRED/IPADDRESS");
    if (IPFILE.is_open())
    {
    getline (IPFILE,IP_loaded);
    IPFILE.close();
    }
    IP_loaded += "/";
    return IP_loaded;
}

string loadUser(string file){
    string USER_LOADED;
    ifstream LOADUSERFILE (file);
    if (LOADUSERFILE.is_open())
    {
    getline (LOADUSERFILE,USER_LOADED);
    LOADUSERFILE.close();
    }

   return USER_LOADED;
}

void changeIP(){

    string IP_loaded, newIP;
    ifstream IPFILE ("/home/IGNUM_CRED/IPADDRESS");
    if (IPFILE.is_open())
    {
    getline (IPFILE,IP_loaded);
    IPFILE.close();
    }
    system("clear");

    printf("LOADED_IP:");
    puts(IP_loaded.c_str());
    printf("\nNEW_IP:");

    getline(std::cin, newIP); //input

    if (newIP != ""){

        ofstream IPFILEW("IPADDRESS");
        IPFILEW << newIP;
        IPFILEW.close();
        system("clear");
        printf("NEW IP SETED!");
        printf("\nNEW_IP:");
        puts(newIP.c_str());
        printf("\n");
    } else {
        printf("ABORTED!\n");
    }

    getline(std::cin, newIP);
    system("clear");

}

string GetChallenge(){

    string Internal_nominal_Native_IP, Challenge_loaded;

    Internal_nominal_Native_IP += "curl -o Challenge ";
    Internal_nominal_Native_IP += nominal_Native_IP;
    Internal_nominal_Native_IP += "Challenge";
    const char* Internal_nominal_Native_IP2 = Internal_nominal_Native_IP.c_str();

    system(Internal_nominal_Native_IP2);
    system("clear");
    ifstream challenge ("Challenge");
    if (challenge.is_open())
    {
    getline (challenge,Challenge_loaded);
    challenge.close();
    }

    return Challenge_loaded;
}

string VALIDATE_COMMAND(string challng, string USER_LOADED, string command){

    string plain_outputed_command, toConvertToken;
    toConvertToken += challng;
    toConvertToken += USER_LOADED;

        string BYTESHADIGESTED_std = sha256(toConvertToken);
 
        const char* shad = BYTESHADIGESTED_std.c_str();
        
        plain_outputed_command += (shad);
        plain_outputed_command += ("//");
        plain_outputed_command += (command.c_str());
        
        string cypher_outputed_command = "IG01";
        cypher_outputed_command += Crypt_It(plain_outputed_command, shad, challng.c_str()); // input, Key, IV
        
        return cypher_outputed_command;
}

void sendRequest(string code){
    string URL = "curl ";
    URL += nominal_Native_IP;
    URL += code;
    
    //cout << URL << "\n"; //DEBUG

    system(URL.c_str());
}

int START_CONNECTION_ETH( string state){
    globalChallenge = GetChallenge();
    system("clear");
    printf("SPARTAN_IGNUM01_TERMINAL\n");
    printf("Server_IP:");
    puts((nominal_Native_IP + "\n").c_str());

    if (state == "CONFIG"){
    USER = "4667211e149ac34e7869bf2aa0403354009df279ee5c83c0a388ac18990942cd"; // CONFIG_MODE;
    printf("CONFIG_MODE_ENABLED!\nUSE 'ADDUSER//ROOT//YOURUSER' AND EXIT!\n");
    }

    printf("CHALLENGE:");
    puts(globalChallenge.c_str());
    printf("\n");

    string command;

    printf(">>");
    getline(std::cin, command); //input
    if (command != "EXIT"){
    sendRequest (VALIDATE_COMMAND(globalChallenge, USER, command));
    getline(std::cin, command); // just to wait;
    START_CONNECTION_ETH(state);
    } else {
       system("clear");
       return 0;
    }
    return 0;

}


int main()
{
    nominal_Native_IP = loadIPFile();
    USER = loadUser("/home/IGNUM_CRED/USER_NATIVE");
    printf("SPARTAN_IGNUM01_TERMINAL\n");
    printf("Server_IP:");
    puts((nominal_Native_IP + "\n").c_str());
    printf("Server_Status:\n");
    printf("---------------\n");
    system(("curl " + nominal_Native_IP).c_str());
    string commands[10] = {"EXIT", "START", "SETIP", "HELP", "CONFIG"};
    string command;
    printf("\n---------------");
    printf("\n>>");
    getline(std::cin, command); //input

    for (int i = 0; i!= 10; i++){
        if (command == commands[i]){

            switch(i+1){
                case 1:
                return 0;
                break;

                case 2:
                START_CONNECTION_ETH("NORMAL");
                break;

                case 3:
                changeIP();
                break;

                case 4:
                //HELP
                for(int h = 0; h != 10; h++){
                    if (commands[h] != ""){
                        puts(commands[h].c_str());
                        printf("\n");
                    }
                }
                break;

                case 5:

                START_CONNECTION_ETH("CONFIG");

                break;

                default:
                printf("Syntax_ERROR!\n");
            }
        }
    }

    main();

}
