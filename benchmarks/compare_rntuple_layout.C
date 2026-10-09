// Exact sequential four-dataset to event-row comparison, including sparse pulses.
#include <ROOT/RNTupleReader.hxx>
#include <TFile.h>
#include <TH1.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
void compare_rntuple_layout(const char* original, const char* eventFile) {
 auto require=[](bool ok,const std::string& why){if(!ok)throw std::runtime_error(why);};
 using ROOT::Experimental::RNTupleReader;
 auto events=RNTupleReader::Open("events",eventFile);
 auto waveform=RNTupleReader::Open("waveform_tree",original);
 auto pulse=RNTupleReader::Open("pulse_tree",original);
 auto caen1190=RNTupleReader::Open("caen1190_tree",original);
 auto helicity=RNTupleReader::Open("m_tree",original);
 auto size=events->GetNEntries();
 require(size==waveform->GetNEntries() && size==caen1190->GetNEntries(),"event counts");
 auto nhits=events->GetView<int>("pulse_nhits");
 std::vector<std::uint64_t> pulseRows;
 for(std::uint64_t i=0;i<size;++i) if(nhits(i)>0) pulseRows.push_back(i);
 require(pulseRows.size()==pulse->GetNEntries(),"pulse count");
 { auto a=waveform->GetView<std::vector<unsigned int>>("slot");auto b=events->GetView<std::vector<unsigned int>>("waveform_slot");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"waveform_slot"); }
 { auto a=waveform->GetView<std::vector<unsigned int>>("chan");auto b=events->GetView<std::vector<unsigned int>>("waveform_chan");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"waveform_chan"); }
 { auto a=waveform->GetView<std::vector<unsigned int>>("waveform");auto b=events->GetView<std::vector<unsigned int>>("waveform_waveform");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"waveform_waveform"); }
 { auto a=waveform->GetView<std::vector<unsigned int>>("rocid");auto b=events->GetView<std::vector<unsigned int>>("waveform_rocid");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"waveform_rocid"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("integral_sum");auto b=events->GetView<std::vector<unsigned int>>("pulse_integral_sum");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_integral_sum"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("coarse_time");auto b=events->GetView<std::vector<unsigned int>>("pulse_coarse_time");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_coarse_time"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("fine_time");auto b=events->GetView<std::vector<unsigned int>>("pulse_fine_time");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_fine_time"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("pulse_peak");auto b=events->GetView<std::vector<unsigned int>>("pulse_pulse_peak");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_pulse_peak"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("chan");auto b=events->GetView<std::vector<unsigned int>>("pulse_chan");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_chan"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("slot");auto b=events->GetView<std::vector<unsigned int>>("pulse_slot");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_slot"); }
 { auto a=pulse->GetView<std::vector<unsigned int>>("rocid");auto b=events->GetView<std::vector<unsigned int>>("pulse_rocid");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_rocid"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("rocid");auto b=events->GetView<std::vector<unsigned int>>("caen1190_rocid");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_rocid"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("slot");auto b=events->GetView<std::vector<unsigned int>>("caen1190_slot");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_slot"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("chan");auto b=events->GetView<std::vector<unsigned int>>("caen1190_chan");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_chan"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("measurement");auto b=events->GetView<std::vector<unsigned int>>("caen1190_measurement");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_measurement"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("opt");auto b=events->GetView<std::vector<unsigned int>>("caen1190_opt");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_opt"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("flags");auto b=events->GetView<std::vector<unsigned int>>("caen1190_flags");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_flags"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("trig_time");auto b=events->GetView<std::vector<unsigned int>>("caen1190_trig_time");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_trig_time"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("hdr_chip_id");auto b=events->GetView<std::vector<unsigned int>>("caen1190_hdr_chip_id");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_hdr_chip_id"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("hdr_event_id");auto b=events->GetView<std::vector<unsigned int>>("caen1190_hdr_event_id");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_hdr_event_id"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("hdr_bunch_id");auto b=events->GetView<std::vector<unsigned int>>("caen1190_hdr_bunch_id");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_hdr_bunch_id"); }
 { auto a=caen1190->GetView<std::vector<unsigned int>>("trl_status");auto b=events->GetView<std::vector<unsigned int>>("caen1190_trl_status");
 for(std::uint64_t i=0;i<size;++i)require(a(i)==b(i),"caen1190_trl_status"); }
 { auto a=pulse->GetView<unsigned int>("pedestal_sum");auto b=events->GetView<unsigned int>("pulse_pedestal_sum");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_pedestal_sum"); }
 { auto a=pulse->GetView<unsigned int>("pedestal_quality");auto b=events->GetView<unsigned int>("pulse_pedestal_quality");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_pedestal_quality"); }
 { auto a=pulse->GetView<int>("nhits");auto b=events->GetView<int>("pulse_nhits");
 for(std::uint64_t i=0;i<pulseRows.size();++i)require(a(i)==b(pulseRows[i]),"pulse_nhits"); }
 { auto a=helicity->GetView<unsigned int>("helicity_seed");auto b=events->GetView<std::vector<unsigned int>>("helicity_helicity_seed");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_helicity_seed");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("n_tstable_fall");auto b=events->GetView<std::vector<unsigned int>>("helicity_n_tstable_fall");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_n_tstable_fall");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("n_tstable_rise");auto b=events->GetView<std::vector<unsigned int>>("helicity_n_tstable_rise");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_n_tstable_rise");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("n_pattsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_n_pattsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_n_pattsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("n_pairsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_n_pairsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_n_pairsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("time_tstable_start");auto b=events->GetView<std::vector<unsigned int>>("helicity_time_tstable_start");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_time_tstable_start");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("time_tstable_end");auto b=events->GetView<std::vector<unsigned int>>("helicity_time_tstable_end");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_time_tstable_end");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last_tstable_duration");auto b=events->GetView<std::vector<unsigned int>>("helicity_last_tstable_duration");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last_tstable_duration");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last_tsettle_duration");auto b=events->GetView<std::vector<unsigned int>>("helicity_last_tsettle_duration");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last_tsettle_duration");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_tstable");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_tstable");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_tstable");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_pattsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_pattsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_pattsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_pairsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_pairsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_pairsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_helicity");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_helicity");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_helicity");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_pat0_helicity");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_pat0_helicity");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_pat0_helicity");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_polarity");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_polarity");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_polarity");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("trig_pat_count");auto b=events->GetView<std::vector<unsigned int>>("helicity_trig_pat_count");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_trig_pat_count");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last32wins_pattsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_last32wins_pattsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last32wins_pattsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last32wins_pairsync");auto b=events->GetView<std::vector<unsigned int>>("helicity_last32wins_pairsync");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last32wins_pairsync");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last32wins_helicity");auto b=events->GetView<std::vector<unsigned int>>("helicity_last32wins_helicity");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last32wins_helicity");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 { auto a=helicity->GetView<unsigned int>("last32wins_pattsync_hel");auto b=events->GetView<std::vector<unsigned int>>("helicity_last32wins_pattsync_hel");
 std::uint64_t index=0;for(std::uint64_t i=0;i<size;++i)for(auto value:b(i)){require(index<helicity->GetNEntries() && a(index)==value,"helicity_last32wins_pattsync_hel");++index;}require(index==helicity->GetNEntries(),"helicity count"); }
 auto blocks=events->GetView<std::uint64_t>("block_number");auto numbers=events->GetView<std::uint64_t>("event_number");
 std::vector<std::pair<std::uint64_t,std::uint64_t>> ids;ids.reserve(size);
 for(std::uint64_t i=0;i<size;++i)ids.emplace_back(blocks(i),numbers(i));
 std::sort(ids.begin(),ids.end());require(std::adjacent_find(ids.begin(),ids.end())==ids.end(),"duplicate block/event ID (single-source fixture)");
 TFile aFile(original),bFile(eventFile);
 auto* a=aFile.Get<TH1>("h_integral");auto* b=bFile.Get<TH1>("h_integral");
 require(a&&b&&a->GetEntries()==b->GetEntries()&&a->GetNcells()==b->GetNcells(),"histogram count");
 for(int i=0;i<a->GetNcells();++i)require(a->GetBinContent(i)==b->GetBinContent(i)&&a->GetBinError(i)==b->GetBinError(i),"histogram bin/error");
 for(int i=1;i<=a->GetNbinsX()+1;++i)require(a->GetXaxis()->GetBinLowEdge(i)==b->GetXaxis()->GetBinLowEdge(i),"histogram edge");
 std::cout<<"Every payload value, sparse pulse row, helicity hit and histogram bin matches across layouts; "<<size<<" event IDs unique"<<std::endl;
}
