#include "channel.h"
#include <cmath>
#include <iostream>
using namespace std;
Channel::Channel(double snr) {
   variance = sqrt(2.0*(2.0 / 3))*pow(10.0, -snr / 10.0);
   //= 1/()*2.0);
   seed_ = 420; 
   dist_ = new  normal_distribution<double>(0.0, variance);
   generator_ = new default_random_engine;
   gen_ = new Generator(seed_);
}

Channel::~Channel() {

}

std::complex<double> Channel::noisify(std::complex<double> input) {
   // TODO: zaimplementowa� poprawn� wersje kana�u z szumem AWGN
   
   double real_value = input.real() + (*dist_)(*generator_);
   double imag_value = input.imag() + (*dist_)(*generator_);
   //cout << "Noise values: " << endl << "real: " << real_value << " imag: " << imag_value << endl;
   return std::complex<double>(real_value, imag_value);
}

