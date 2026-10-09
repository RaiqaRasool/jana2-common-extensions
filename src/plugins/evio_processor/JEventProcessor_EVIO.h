
#ifndef _JEventProcessor_EVIO_h_
#define _JEventProcessor_EVIO_h_

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include "SequentialRootNtuple.h"
#include "EventRootNtuple.h"

#include <string>
#include <vector>

#include <JANA/JEventProcessor.h>
#include "CAEN1190Hit.h"
#include "EventHits_FADC.h"
#include "HelicityDecoderData.h"

struct HelDec_t {
    uint32_t helicity_seed;
    uint32_t n_tstable_fall;
    uint32_t n_tstable_rise;
    uint32_t n_pattsync;
    uint32_t n_pairsync;
    uint32_t time_tstable_start;
    uint32_t time_tstable_end;
    uint32_t last_tstable_duration;
    uint32_t last_tsettle_duration;
    uint32_t trig_tstable;
    uint32_t trig_pattsync;
    uint32_t trig_pairsync;
    uint32_t trig_helicity;
    uint32_t trig_pat0_helicity;
    uint32_t trig_polarity;
    uint32_t trig_pat_count;
    uint32_t last32wins_pattsync;
    uint32_t last32wins_pairsync;
    uint32_t last32wins_helicity;
    uint32_t last32wins_pattsync_hel;
};

/**
 * @class JEventProcessor_EVIO
 * @brief Main event processor for FADC250 detector data analysis
 * 
 * This processor receives FADC250 detector hits (both waveform and pulse hits)
 * and outputs the data to a ROOT file containing:
 * - Waveform TTree: Channel-by-channel waveform data
 * - Pulse integral histogram: Distribution of pulse integral sums
 * 
 * The output filename can be customized via JANA2 parameters.
 */
class JEventProcessor_EVIO : public JEventProcessor {

private:
    // Declare Inputs
    Input<CAEN1190Hit>                 m_caen1190_hits_in {this};
    Input<FADC250WaveformHit>          m_waveform_hits_in {this}; 
    Input<FADC250PulseHit>             m_pulse_hits_in {this};
    Input<HelicityDecoderData>         m_heldec_data_in {this};

    /**
     * @brief ROOT output filename parameter
     * 
     * This parameter allows users to specify the ROOT output filename via JANA2 configuration.
     * The parameter constructor takes the following arguments:
     * - owner: Pointer to this component (for parameter registration)
     * - name: "ROOT_OUT_FILENAME" - the parameter name used in configuration files/command line
     * - default_value: "evio_processor.root" - default filename if not specified
     * - description: "Output file name for root data" - help text for the parameter
     * - is_shared: if true, the parameter name is used as-is;  
     *              if false (default), the component's prefix (set in the constructor) is prepended to the name.
     */
    Parameter<std::string> m_root_output_filename {this, "ROOT_OUT_FILENAME", "evio_processor.root", "Output file name for ROOT data", true};

    Parameter<unsigned int> m_root_imt_threads {
        this, "ROOT_IMT_THREADS", 0,
        "ROOT implicit-MT thread count for parallel branch compression (0 leaves ROOT unchanged)", true};

    Parameter<std::string> m_root_format {
        this, "ROOT_FORMAT", "ttree", "Output storage: ttree, rntuple, or rntuple_event", true};
    SequentialRootNtuple m_waveform_ntuple, m_pulse_ntuple, m_caen1190_ntuple, m_helicity_ntuple;
    Parameter<bool> m_root_rntuple_parallel {
        this, "ROOT_RNTUPLE_PARALLEL", false,
        "Worker-local RNTuple fill contexts; requires ROOT_FORMAT=rntuple_event; entries may reorder", true};
    bool m_use_rntuple = false;
    std::unique_ptr<EventRootNtuple> m_event_ntuple;

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

    // helicity decoder tree variables
    HelDec_t heldec{};

    // ROOT output objects
    TFile *m_root_output_file;                ///< ROOT file for histogram and tree storage
    TTree *m_waveform_tree;                   ///< ROOT tree for waveform data
    TTree *m_tree;                            ///< ROOT tree for physics event
    TH1I *m_pulse_integral_hist;              ///< Histogram of pulse integral sums
    TTree *m_pulse_tree;                      ///< ROOT tree for pulse hit
    TTree *m_caen1190_tree;                     ///< Root tree for CAEN1190 data
    

public:

    JEventProcessor_EVIO();
    virtual ~JEventProcessor_EVIO() = default;

    void Init() override;
    void ProcessParallel(const JEvent& event) override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};

#endif // _JEventProcessor_EVIO_h_

