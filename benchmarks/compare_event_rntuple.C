// Exact event-keyed comparison. Compare in target order to keep clustered reads local.
#include <ROOT/RNTupleReader.hxx>
#include <TFile.h>
#include <TH1.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace event_compare {
using ROOT::Experimental::RNTupleReader;
struct Key {
 std::uint32_t source;std::int32_t run;std::uint64_t block,event;
 auto Values()const{return std::tie(source,run,block,event);}
 bool operator<(const Key& other)const{return Values()<other.Values();}
 bool operator==(const Key& other)const{return Values()==other.Values();}
};
struct IndexedKey{Key key;std::uint64_t entry;};
std::vector<IndexedKey> Keys(RNTupleReader& reader,std::map<std::string,std::uint32_t>& sources) {
 auto source=reader.GetView<std::string>("source_file");auto run=reader.GetView<std::int32_t>("run_number");
 auto block=reader.GetView<std::uint64_t>("block_number");auto event=reader.GetView<std::uint64_t>("event_number");
 std::vector<IndexedKey> result;result.reserve(reader.GetNEntries());
 for(std::uint64_t i=0;i<reader.GetNEntries();++i) {
  auto item=sources.try_emplace(source(i),sources.size());
  result.push_back({{item.first->second,run(i),block(i),event(i)},i});
 }
 std::sort(result.begin(),result.end(),[](const auto& a,const auto& b){return a.key<b.key;});
 return result;
}
template<class T>void Field(RNTupleReader& a,RNTupleReader& b,const std::vector<std::uint64_t>& mapping,const char* name) {
 auto x=a.GetView<T>(name);auto y=b.GetView<T>(name);
 for(std::uint64_t i=0;i<mapping.size();++i) if(!(x(mapping[i])==y(i)))
  throw std::runtime_error(std::string("Event payload mismatch: ")+name+" target entry "+std::to_string(i));
 std::cout<<name<<": every value matches"<<std::endl;
}
}
void compare_event_rntuple(const char* reference,const char* target) {
 using namespace event_compare;
 auto require=[](bool ok,const std::string& why){if(!ok)throw std::runtime_error(why);};
 auto a=RNTupleReader::Open("events",reference);auto b=RNTupleReader::Open("events",target);
 require(a->GetNEntries()==b->GetNEntries(),"event counts");
 std::map<std::string,std::uint32_t> sources;
 auto ka=Keys(*a,sources);auto kb=Keys(*b,sources);
 std::vector<std::uint64_t> mapping(ka.size());
 for(std::size_t i=0;i<ka.size();++i) {
  require(ka[i].key==kb[i].key,"event identity mismatch");
  if(i)require(!(ka[i-1].key==ka[i].key)&&!(kb[i-1].key==kb[i].key),"duplicate event identity");
  mapping[kb[i].entry]=ka[i].entry;
 }
 ka.clear();ka.shrink_to_fit();kb.clear();kb.shrink_to_fit();
 std::cout<<mapping.size()<<" event identities match without duplicates"<<std::endl;
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"waveform_slot");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"waveform_chan");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"waveform_waveform");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"waveform_rocid");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_integral_sum");
 Field<std::uint32_t>(*a,*b,mapping,"pulse_pedestal_sum");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_coarse_time");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_fine_time");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_pulse_peak");
 Field<std::uint32_t>(*a,*b,mapping,"pulse_pedestal_quality");
 Field<std::int32_t>(*a,*b,mapping,"pulse_nhits");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_chan");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_slot");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"pulse_rocid");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_rocid");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_slot");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_chan");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_measurement");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_opt");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_flags");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_trig_time");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_hdr_chip_id");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_hdr_event_id");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_hdr_bunch_id");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"caen1190_trl_status");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_helicity_seed");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_n_tstable_fall");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_n_tstable_rise");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_n_pattsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_n_pairsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_time_tstable_start");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_time_tstable_end");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last_tstable_duration");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last_tsettle_duration");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_tstable");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_pattsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_pairsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_helicity");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_pat0_helicity");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_polarity");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_trig_pat_count");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last32wins_pattsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last32wins_pairsync");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last32wins_helicity");
 Field<std::vector<std::uint32_t>>(*a,*b,mapping,"helicity_last32wins_pattsync_hel");
 Field<std::string>(*a,*b,mapping,"source_file");
 Field<std::int32_t>(*a,*b,mapping,"run_number");
 Field<std::uint64_t>(*a,*b,mapping,"block_number");
 Field<std::uint64_t>(*a,*b,mapping,"event_number");
 TFile af(reference),bf(target);auto* x=af.Get<TH1>("h_integral");auto* y=bf.Get<TH1>("h_integral");
 require(x&&y&&x->GetEntries()==y->GetEntries()&&x->GetNcells()==y->GetNcells(),"histogram count");
 for(int i=0;i<x->GetNcells();++i)require(x->GetBinContent(i)==y->GetBinContent(i)&&x->GetBinError(i)==y->GetBinError(i),"histogram bin/error");
 for(int i=1;i<=x->GetNbinsX()+1;++i)require(x->GetXaxis()->GetBinLowEdge(i)==y->GetXaxis()->GetBinLowEdge(i),"histogram edge");
 std::cout<<"Every event-keyed field and histogram bin/error/edge matches"<<std::endl;
}
