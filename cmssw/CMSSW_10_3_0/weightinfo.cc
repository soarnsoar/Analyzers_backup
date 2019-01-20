#include <TString.h>
#include "GEN_Analyzer/JHanalyzer/interface/weightinfo.h"
using namespace std;

weightinfo::weightinfo(){
  _id="-1";
  _pdf="-1";
  _name="";
  _muR=0;
  _muF=0;
  _weight=0;
}
weightinfo::weightinfo(string id, string pdf, TString name, double muR, double muF){
  _id=id;
  _pdf=pdf;
  _name=name;
  _muR=muR;
  _muF=muF;

}
void weightinfo::set_id(string id){
  _id=id;
}
void weightinfo::set_pdf(string pdf){
  _pdf=pdf;
}
void weightinfo::set_name(TString name){
  _name=name;
}
void weightinfo::set_muR(double muR){
  _muR=muR;
}
void weightinfo::set_muF(double muF){
  _muF=muF;
}
void weightinfo::set_weight(double weight){
  _weight=weight;
}
string weightinfo::id(){
  return _id;
}
string weightinfo::pdf(){
  return _pdf;
}
TString weightinfo::name(){
  return _name;
}
double weightinfo::muR(){
  return _muR;
}
double weightinfo::muF(){
  return _muF;
}
double weightinfo::weight(){
  return _weight;
}
