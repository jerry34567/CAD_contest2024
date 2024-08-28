/****************************************************************************
  FileName     [ rnGen.h ]
  PackageName  [ util ]
  Synopsis     [ Random number generator ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef RN_GEN_H
#define RN_GEN_H

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <limits.h>
#include <cmath>

#define my_srandom  srandom
#define my_random   random
typedef std::vector<size_t> random_vec_t;

class RandomNumGen
{
   public:
      RandomNumGen() { my_srandom(getpid()); }
      RandomNumGen(unsigned seed) { my_srandom(seed); }
      const int operator() (const int range) const {
         return int(range * (double(my_random()) / INT_MAX));
      }
};

class RandomNumGen2
{
   public:
      RandomNumGen2() { my_srandom(getpid()); }
      RandomNumGen2(unsigned seed) { my_srandom(seed); }
      const random_vec_t operator () (const size_t length) const {
         random_vec_t random_vec;
         for(size_t i = 0; i < length; ++i)
         {
            random_vec.push_back(size_t(my_random() * (RAND_MAX + 1) + my_random()));
         }
         return random_vec;
      }
};


#define NUMBER1  3716960521u
#define NUMBER2  2174103536u

class my_RandomNumGen
{
   public:
      my_RandomNumGen(unsigned seed) {
         My_Random(1);
         for(int i = 0; i < seed; i++) {
            My_Random(0);
         }
      }
      const unsigned operator() () {
         return My_Random(0);
      }
      void reset(unsigned seed) {
         My_Random(1);
         for(int i = 0; i < seed; i++) {
            My_Random(0);
         }
      }
   private:
      unsigned My_Random( int fReset )
      {
         static unsigned int m_z = NUMBER1;
         static unsigned int m_w = NUMBER2;
         if ( fReset )
         {
            m_z = NUMBER1;
            m_w = NUMBER2;
         }
         m_z = 36969 * (m_z & 65535) + (m_z >> 16);
         m_w = 18000 * (m_w & 65535) + (m_w >> 16);
         return (m_z << 16) + m_w;
      }
};

#endif // RN_GEN_H

