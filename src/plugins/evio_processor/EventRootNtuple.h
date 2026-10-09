#pragma once
#include "SequentialRootNtuple.h"
#include <JANA/JEvent.h>
#include <cstdint>
#include <string>
#include <vector>
#include <ROOT/RNTupleParallelWriter.hxx>
#include <mutex>

// One row per physics event; vectors keep each detector's original hit order.
struct EventRootRecord {
    // ROOT Tree variables
    //Waveform Tree Variables
    std::vector<uint32_t> ev_slot;
    std::vector<uint32_t> ev_chan;
    std::vector<uint32_t> ev_waveform;
    std::vector<uint32_t> ev_rocid;

    //Pulse Tree Variables
    uint32_t integral_sum;
    uint32_t coarse_time;
    uint32_t fine_time;
    uint32_t pulse_peak;
    uint32_t pedestal_sum;
    uint32_t pedestal_quality;
    int number_hit;
    std::vector<uint32_t> ev_integral_sum;
    std::vector<uint32_t> ev_coarse_time;
    std::vector<uint32_t> ev_fine_time;
    std::vector<uint32_t> ev_pulse_peak;
    std::vector<uint32_t> ev_pulse_slot;
    std::vector<uint32_t> ev_pulse_chan;
    std::vector<uint32_t> ev_pulse_rocid;

    //CAEN Tree Variables
    std::vector<uint32_t> ev_caen_rocid;
    std::vector<uint32_t> ev_caen_slot;
    std::vector<uint32_t> ev_caen_chan;
    std::vector<uint32_t> ev_caen_measurement;
    std::vector<uint32_t> ev_caen_opt;
    std::vector<uint32_t> ev_caen_flags;
    std::vector<uint32_t> ev_caen_trig_time;
    std::vector<uint32_t> ev_caen_hdr_chip_id;
    std::vector<uint32_t> ev_caen_hdr_event_id;
    std::vector<uint32_t> ev_caen_hdr_bunch_id;
    std::vector<uint32_t> ev_caen_trl_status;


    std::vector<uint32_t> hel_helicity_seed;
    std::vector<uint32_t> hel_n_tstable_fall;
    std::vector<uint32_t> hel_n_tstable_rise;
    std::vector<uint32_t> hel_n_pattsync;
    std::vector<uint32_t> hel_n_pairsync;
    std::vector<uint32_t> hel_time_tstable_start;
    std::vector<uint32_t> hel_time_tstable_end;
    std::vector<uint32_t> hel_last_tstable_duration;
    std::vector<uint32_t> hel_last_tsettle_duration;
    std::vector<uint32_t> hel_trig_tstable;
    std::vector<uint32_t> hel_trig_pattsync;
    std::vector<uint32_t> hel_trig_pairsync;
    std::vector<uint32_t> hel_trig_helicity;
    std::vector<uint32_t> hel_trig_pat0_helicity;
    std::vector<uint32_t> hel_trig_polarity;
    std::vector<uint32_t> hel_trig_pat_count;
    std::vector<uint32_t> hel_last32wins_pattsync;
    std::vector<uint32_t> hel_last32wins_pairsync;
    std::vector<uint32_t> hel_last32wins_helicity;
    std::vector<uint32_t> hel_last32wins_pattsync_hel;
    std::string source_file;
    int32_t run_number = 0;
    uint64_t block_number = 0, event_number = 0;
    const JEventSource* cached_source = nullptr;
    template<class F> void Fields(F&& field) {
        field("waveform_slot", ev_slot);
        field("waveform_chan", ev_chan);
        field("waveform_waveform", ev_waveform);
        field("waveform_rocid", ev_rocid);
        field("pulse_integral_sum", ev_integral_sum);
        field("pulse_pedestal_sum", pedestal_sum);
        field("pulse_coarse_time", ev_coarse_time);
        field("pulse_fine_time", ev_fine_time);
        field("pulse_pulse_peak", ev_pulse_peak);
        field("pulse_pedestal_quality", pedestal_quality);
        field("pulse_nhits", number_hit);
        field("pulse_chan", ev_pulse_chan);
        field("pulse_slot", ev_pulse_slot);
        field("pulse_rocid", ev_pulse_rocid);
        field("caen1190_rocid", ev_caen_rocid);
        field("caen1190_slot", ev_caen_slot);
        field("caen1190_chan", ev_caen_chan);
        field("caen1190_measurement", ev_caen_measurement);
        field("caen1190_opt", ev_caen_opt);
        field("caen1190_flags", ev_caen_flags);
        field("caen1190_trig_time", ev_caen_trig_time);
        field("caen1190_hdr_chip_id", ev_caen_hdr_chip_id);
        field("caen1190_hdr_event_id", ev_caen_hdr_event_id);
        field("caen1190_hdr_bunch_id", ev_caen_hdr_bunch_id);
        field("caen1190_trl_status", ev_caen_trl_status);
        field("helicity_helicity_seed", hel_helicity_seed);
        field("helicity_n_tstable_fall", hel_n_tstable_fall);
        field("helicity_n_tstable_rise", hel_n_tstable_rise);
        field("helicity_n_pattsync", hel_n_pattsync);
        field("helicity_n_pairsync", hel_n_pairsync);
        field("helicity_time_tstable_start", hel_time_tstable_start);
        field("helicity_time_tstable_end", hel_time_tstable_end);
        field("helicity_last_tstable_duration", hel_last_tstable_duration);
        field("helicity_last_tsettle_duration", hel_last_tsettle_duration);
        field("helicity_trig_tstable", hel_trig_tstable);
        field("helicity_trig_pattsync", hel_trig_pattsync);
        field("helicity_trig_pairsync", hel_trig_pairsync);
        field("helicity_trig_helicity", hel_trig_helicity);
        field("helicity_trig_pat0_helicity", hel_trig_pat0_helicity);
        field("helicity_trig_polarity", hel_trig_polarity);
        field("helicity_trig_pat_count", hel_trig_pat_count);
        field("helicity_last32wins_pattsync", hel_last32wins_pattsync);
        field("helicity_last32wins_pairsync", hel_last32wins_pairsync);
        field("helicity_last32wins_helicity", hel_last32wins_helicity);
        field("helicity_last32wins_pattsync_hel", hel_last32wins_pattsync_hel);
        field("source_file", source_file);
        field("run_number", run_number);
        field("block_number", block_number);
        field("event_number", event_number);
    }
    void Load(const JEvent& event);
};

// ROOT owns cluster synchronization; each JANA worker owns its fill buffers.
class EventRootNtuple {
    struct Worker {
        EventRootRecord record;
        std::shared_ptr<ROOT::Experimental::RNTupleFillContext> context;
        std::unique_ptr<ROOT::Experimental::REntry> entry;
        explicit Worker(ROOT::Experimental::RNTupleParallelWriter& writer);
        void Fill(const JEvent& event) { record.Load(event); context->Fill(*entry); }
    };
    EventRootRecord m_record;
    SequentialRootNtuple m_ntuple;
    std::unique_ptr<ROOT::Experimental::RNTupleParallelWriter> m_parallel;
    std::mutex m_workers_mutex;
    std::vector<std::shared_ptr<Worker>> m_workers;
    std::shared_ptr<Worker> GetWorker();
public:
    EventRootNtuple(TFile& file, bool parallel);
    void Fill(const JEvent& event);
    void Finish();
};
