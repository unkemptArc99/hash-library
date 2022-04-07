#include "hash.h"
#include "keccak.h"
#include <bits/stdc++.h>
#include <boost/multiprecision/cpp_int.hpp>

static unsigned char gethex(const char *s, char **endptr) {
  assert(s);
  while (isspace(*s)) s++;
  assert(*s);
  return strtoul(s, endptr, 16);
}

unsigned char *convert(const char *s, int *length) {
  unsigned char *answer = (unsigned char*)malloc((strlen(s) + 1) / 3);
  unsigned char *p;
  for (p = answer; *s; p++)
    *p = gethex(s, (char **)&s);
  *length = p - answer;
  return answer;
}

int main() {
    // our address, converted to a byte array
    // 0x82869bA6F714EEA1Fad1708f281485000c7A9A14
    const char *s = "82 86 9B A6 F7 14 EE A1 FA D1 70 8F 28 14 85 00 0C 7A 9A 14";
    int len;
    unsigned char *address = (unsigned char*)malloc((strlen(s) + 1) / 3);
    unsigned char *p;
    for (p = address; *s; p++)
        *p = gethex(s, (char **)&s);
    len = p - address;
    // const unsigned char address[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 222, 173, 190, 239}; 

    // after padding, our data is 64 characters long
    // allocates to 0s automatically
    unsigned char data[64] = {};

    // in abi.encode padding, address goes in the last 20 bytes
    for (int i = 0; i < 20; i++) {
    data[44 + i] = address[i];
    }

    // the nonce goes in the first 32 bytes, big-endian
    // 914 = 0x392 = bytes([3, 146])
    // data[30] = 3;
    // data[31] = 146;
    uint64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count();
    boost::multiprecision::uint256_t mask = 1;
    mask = mask << 34;
    std::cout<<mask<<std::endl;
    boost::multiprecision::uint256_t init = 7;
    boost::multiprecision::uint256_t j = init;
    boost::multiprecision::uint256_t checkpoint = boost::multiprecision::pow(boost::multiprecision::uint256_t(10),9);
    while (true) {
        std::stringstream stream;
        stream << std::hex<<j;
        std::string int_hex_str = stream.str();
        std::transform(int_hex_str.begin(), int_hex_str.end(), int_hex_str.begin(), ::toupper);
        for (int i = int_hex_str.length()-2; i >= 0; i-=2) {
            int_hex_str.insert(i, " ");
        }

        unsigned char *nonce = (unsigned char*)malloc((strlen(int_hex_str.c_str()) + 1) / 3);
        char *cstr = new char [int_hex_str.length() + 1];
        std::strcpy(cstr, int_hex_str.c_str());
        for (p = nonce; *cstr; p++)
            *p = gethex(cstr, (char **)&cstr);
        len = p - nonce;
        for (int i = len-1; i >= 0; i--) {
            data[32-(len-i)] = nonce[i];
        }

        Keccak keccak(Keccak::Bits::Keccak256);
        keccak.add(data, 64);
        std::string hash_str = keccak.getHash();
        boost::multiprecision::uint256_t hash_num = 0;
        std::stringstream(hash_str) >> std::hex >> hash_num;
        if (hash_num % mask == 0) {
          std::cout<<hash_str<<" "<<nonce<<std::endl;
          break;
        }
        j+=8;
        uint64_t endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()
          ).count();
        
        if ((j-init)/8 % checkpoint == 0) {
          uint64_t endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();
          std::cout<<endTime - startTime<< " "<< j<<std::endl;
        }
    }
        



    // initialize keccak and add the data
    return 0;
}