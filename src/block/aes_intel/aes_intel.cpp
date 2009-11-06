/**
* AES
* (C) 1999-2009 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/aes_intel.h>
#include <wmmintrin.h>

namespace Botan {

namespace {

__m128i aes_128_key_expansion(__m128i key, __m128i key_with_rcon)
   {
   key_with_rcon = _mm_shuffle_epi32(key_with_rcon, _MM_SHUFFLE(3,3,3,3));
   key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
   key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
   key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
   return _mm_xor_si128(key, key_with_rcon);
   }

}

/**
* AES Encryption
*/
void AES_128_Intel::encrypt_n(const byte in[], byte out[], u32bit blocks) const
   {
   const __m128i* in_mm = (const __m128i*)in;
   __m128i* out_mm = (__m128i*)out;

   const __m128i* key_mm = (const __m128i*)&EK[0];

   __m128i K0  = _mm_loadu_si128(key_mm);
   __m128i K1  = _mm_loadu_si128(key_mm + 1);
   __m128i K2  = _mm_loadu_si128(key_mm + 2);
   __m128i K3  = _mm_loadu_si128(key_mm + 3);
   __m128i K4  = _mm_loadu_si128(key_mm + 4);
   __m128i K5  = _mm_loadu_si128(key_mm + 5);
   __m128i K6  = _mm_loadu_si128(key_mm + 6);
   __m128i K7  = _mm_loadu_si128(key_mm + 7);
   __m128i K8  = _mm_loadu_si128(key_mm + 8);
   __m128i K9  = _mm_loadu_si128(key_mm + 9);
   __m128i K10 = _mm_loadu_si128(key_mm + 10);

   for(u32bit i = 0; i != blocks; ++i)
      {
      __m128i B = _mm_loadu_si128(in_mm + i);

      B = _mm_xor_si128(B, K0);

      B = _mm_aesenc_si128(B, K1);
      B = _mm_aesenc_si128(B, K2);
      B = _mm_aesenc_si128(B, K3);
      B = _mm_aesenc_si128(B, K4);
      B = _mm_aesenc_si128(B, K5);
      B = _mm_aesenc_si128(B, K6);
      B = _mm_aesenc_si128(B, K7);
      B = _mm_aesenc_si128(B, K8);
      B = _mm_aesenc_si128(B, K9);
      B = _mm_aesenclast_si128(B, K10);

      _mm_storeu_si128(out_mm + i, B);

      in += BLOCK_SIZE;
      out += BLOCK_SIZE;
      }
   }

/**
* AES Decryption
*/
void AES_128_Intel::decrypt_n(const byte in[], byte out[], u32bit blocks) const
   {
   const __m128i* in_mm = (const __m128i*)in;
   __m128i* out_mm = (__m128i*)out;

   const __m128i* key_mm = (const __m128i*)&DK[0];

   __m128i K0  = _mm_loadu_si128(key_mm);
   __m128i K1  = _mm_loadu_si128(key_mm + 1);
   __m128i K2  = _mm_loadu_si128(key_mm + 2);
   __m128i K3  = _mm_loadu_si128(key_mm + 3);
   __m128i K4  = _mm_loadu_si128(key_mm + 4);
   __m128i K5  = _mm_loadu_si128(key_mm + 5);
   __m128i K6  = _mm_loadu_si128(key_mm + 6);
   __m128i K7  = _mm_loadu_si128(key_mm + 7);
   __m128i K8  = _mm_loadu_si128(key_mm + 8);
   __m128i K9  = _mm_loadu_si128(key_mm + 9);
   __m128i K10 = _mm_loadu_si128(key_mm + 10);

   for(u32bit i = 0; i != blocks; ++i)
      {
      __m128i B = _mm_loadu_si128(in_mm + i);

      B = _mm_xor_si128(B, K0);

      B = _mm_aesdec_si128(B, K1);
      B = _mm_aesdec_si128(B, K2);
      B = _mm_aesdec_si128(B, K3);
      B = _mm_aesdec_si128(B, K4);
      B = _mm_aesdec_si128(B, K5);
      B = _mm_aesdec_si128(B, K6);
      B = _mm_aesdec_si128(B, K7);
      B = _mm_aesdec_si128(B, K8);
      B = _mm_aesdec_si128(B, K9);
      B = _mm_aesdeclast_si128(B, K10);

      _mm_storeu_si128(out_mm + i, B);

      in += BLOCK_SIZE;
      out += BLOCK_SIZE;
      }
   }

/**
* AES Key Schedule
*/
void AES_128_Intel::key_schedule(const byte key[], u32bit length)
   {

   #define AES_128_key_exp(K, RCON) \
      aes_128_key_expansion(K, _mm_aeskeygenassist_si128(K, RCON))

   __m128i K0  = _mm_loadu_si128((const __m128i*)key);
   __m128i K1  = AES_128_key_exp(K0, 0x01);
   __m128i K2  = AES_128_key_exp(K1, 0x02);
   __m128i K3  = AES_128_key_exp(K2, 0x04);
   __m128i K4  = AES_128_key_exp(K3, 0x08);
   __m128i K5  = AES_128_key_exp(K4, 0x10);
   __m128i K6  = AES_128_key_exp(K5, 0x20);
   __m128i K7  = AES_128_key_exp(K6, 0x40);
   __m128i K8  = AES_128_key_exp(K7, 0x80);
   __m128i K9  = AES_128_key_exp(K8, 0x1B);
   __m128i K10 = AES_128_key_exp(K9, 0x36);

   __m128i* EK_mm = (__m128i*)&EK[0];
   _mm_storeu_si128(EK_mm     , K0);
   _mm_storeu_si128(EK_mm +  1, K1);
   _mm_storeu_si128(EK_mm +  2, K2);
   _mm_storeu_si128(EK_mm +  3, K3);
   _mm_storeu_si128(EK_mm +  4, K4);
   _mm_storeu_si128(EK_mm +  5, K5);
   _mm_storeu_si128(EK_mm +  6, K6);
   _mm_storeu_si128(EK_mm +  7, K7);
   _mm_storeu_si128(EK_mm +  8, K8);
   _mm_storeu_si128(EK_mm +  9, K9);
   _mm_storeu_si128(EK_mm + 10, K10);

   // Now generate decryption keys

   __m128i* DK_mm = (__m128i*)&DK[0];
   _mm_storeu_si128(DK_mm     , K10);
   _mm_storeu_si128(DK_mm +  1, _mm_aesimc_si128(K9));
   _mm_storeu_si128(DK_mm +  2, _mm_aesimc_si128(K8));
   _mm_storeu_si128(DK_mm +  3, _mm_aesimc_si128(K7));
   _mm_storeu_si128(DK_mm +  4, _mm_aesimc_si128(K6));
   _mm_storeu_si128(DK_mm +  5, _mm_aesimc_si128(K5));
   _mm_storeu_si128(DK_mm +  6, _mm_aesimc_si128(K4));
   _mm_storeu_si128(DK_mm +  7, _mm_aesimc_si128(K3));
   _mm_storeu_si128(DK_mm +  8, _mm_aesimc_si128(K2));
   _mm_storeu_si128(DK_mm +  9, _mm_aesimc_si128(K1));
   _mm_storeu_si128(DK_mm + 10, K0);
   }

/**
* Clear memory of sensitive data
*/
void AES_128_Intel::clear()
   {
   EK.clear();
   DK.clear();
   }

}
