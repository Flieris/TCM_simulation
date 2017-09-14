#include "decoder.h"
#include <complex>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;
Decoder::Decoder() {
   _8psk_constelation = new map<int, complex<double>>;
   _8psk_constelation->operator[](0) = complex<double>(1.0, 0.0);
   _8psk_constelation->operator[](1) = complex<double>(0.707, 0.707);
   _8psk_constelation->operator[](2) = complex<double>(0.0, 1.0);
   _8psk_constelation->operator[](3) = complex<double>(-0.707, 0.707);
   _8psk_constelation->operator[](4) = complex<double>(-1.0, 0.0);
   _8psk_constelation->operator[](5) = complex<double>(-0.707, -0.707);
   _8psk_constelation->operator[](6) = complex<double>(0.0, -1.0);
   _8psk_constelation->operator[](7) = complex<double>(0.707, -0.707);
   decoded_data = 0;
   
   // trellis info:
   // next state
   next_state_.push_back({ 0,1,0,1 });
   next_state_.push_back({ 2,3,2,3 }); 
   next_state_.push_back({ 0,1,0,1 }); 
   next_state_.push_back({ 2,3,2,3 });
   // output state
   output_state_.push_back({ 0,2,4,6 });
   output_state_.push_back({ 1,3,5,7 }); 
   output_state_.push_back({ 2,0,6,4 }); 
   output_state_.push_back({ 3,1,7,5 });
   // create trellis vector
   for (int i = 0; i < num_of_states_; ++i)
      trellis_.push_back(vector<Path*>());
   for (int i = 0; i < num_of_states_; ++i) {
      metrics_.push_back(9999.0);
   }
   metrics_[0] = 0.0;
}

Decoder::~Decoder() {
   delete _8psk_constelation;
}

void Decoder::decode(complex<double> input_data) {
   //calculate Euclidian distances between input signal and every point on constellation
   auto distances = new vector<double>;
   for (int i = 0; i < 8; i++) {
      distances->push_back(getDistance(input_data, i));
   }
   int pathways[4] = { 0,0,0,0 };
   vector<Path*> *path_metrics = new vector<Path*>;
   path_metrics->clear();
   for (int from_state = 0; from_state < num_of_states_; ++from_state) {
      vector<Path*> *state_metrics = new vector<Path*>;
      state_metrics->clear();
      for (int i = 0; i < 4; i++) {
         state_metrics->push_back(nullptr);
      }
      Path *minimal_path = new Path();
      for (int input_state = 0; input_state < num_of_states_; ++input_state) {
         int next = next_state_[from_state][input_state];
         Path *new_path = new Path();
         new_path->input_ = input_state;
         new_path->previous_ = from_state;
         new_path->next_ = next;
         double output_ = output_state_[from_state][input_state];
         new_path->metric_ = getDistance(input_data, output_);
         if ((*state_metrics)[input_state] == nullptr) {
            (*state_metrics)[input_state] = new_path;
         }
         else {

         }
      }
       int min_index = 0;
       
       for (int j = 0; j < num_of_states_; ++j) {
          if (trellis_[(*state_metrics)[j]->next_].size() > 0) {
             double cost = (*state_metrics)[j]->metric_ + trellis_[from_state].back()->metric_;
             (*state_metrics)[j]->metric_ = cost;
          }
       }
       for (int i = 1; i < num_of_states_; ++i) {
          if ((*state_metrics)[i]->metric_ < (*state_metrics)[min_index]->metric_) {
             min_index = i;
         }
      }
      minimal_path = (*state_metrics)[min_index];
      path_metrics->push_back(minimal_path);
      pathways[from_state] = minimal_path->next_;
   }
   for (int i = 0; i < num_of_states_; ++i) {
      trellis_[pathways[i]].push_back(path_metrics->operator[](i));
   }
   for (int i = 0; i < trellis_.size(); ++i) {
      cout << "Path stats: from state: " << trellis_[i].back()->previous_ << " to state: " << trellis_[i].back()->next_ << " input: " << trellis_[i].back()->input_ << " metric: " << trellis_[i].back()->metric_ << endl;
     
   }
}

int Decoder::getDecodedData() {
   // implement trellis decision
   return decoded_data;
}

double Decoder::getDistance(complex<double> data, int constelation_point){
   double delta_x = data.real() - _8psk_constelation->operator[](constelation_point).real();
   double delta_y = data.imag() - _8psk_constelation->operator[](constelation_point).imag();
   double result = floorf(abs(complex<double>(delta_x, delta_y))*100)/100;
   return pow(result,2.0);
}
