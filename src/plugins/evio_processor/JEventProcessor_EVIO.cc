#include "JEventProcessor_EVIO.h"
#include <JANA/JLogger.h>
#include <TROOT.h>

/**
 * @brief Constructor for JEventProcessor_EVIO
 * 
 * Initialize the processor with the appropriate type name, prefix, and callback style.
 */
JEventProcessor_EVIO::JEventProcessor_EVIO() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with this class's name
    SetPrefix("jeventprocessor_evio");            // Set unique prefix for parameters
    SetCallbackStyle(CallbackStyle::ExpertMode);  // Use expert mode for full control

    // All of these are optional because not all events will have these hits
    m_caen1190_hits_in.SetOptional(true);
    m_pulse_hits_in.SetOptional(true);
    m_waveform_hits_in.SetOptional(true);
    m_heldec_data_in.SetOptional(true);
}

/**
 * @brief Initialize the processor
 * 
 * Called once at the start of processing. Open the output files and set up
 * any necessary resources for event processing.
 */
void JEventProcessor_EVIO::Init() {
    LOG << "JEventProcessor_EVIO::Init" << LOG_END;
    
    if (m_root_imt_threads() > 0) {
        ROOT::EnableImplicitMT(m_root_imt_threads());
        if (!ROOT::IsImplicitMTEnabled()) {
            throw JException("ROOT_IMT_THREADS requires a ROOT build with implicit multithreading support");
        }
        LOG << "ROOT implicit multithreading enabled for branch compression: "
            << ROOT::GetThreadPoolSize() << " threads" << LOG_END;
    }

    if (m_root_format() != "ttree" && m_root_format() != "rntuple")
        throw JException("ROOT_FORMAT must be ttree or rntuple");
    m_use_rntuple = m_root_format() == "rntuple";

    // Open the ROOT output file
    m_root_output_file = new TFile(m_root_output_filename().c_str(), "RECREATE");
    if (m_root_output_file == nullptr || m_root_output_file->IsZombie()) {
        throw JException("Failed to open ROOT output file: " + m_root_output_filename());  
    }

    if (m_use_rntuple) {
        m_waveform_ntuple.Field("slot", &ev_slot);
        m_waveform_ntuple.Field("chan", &ev_chan);
        m_waveform_ntuple.Field("waveform", &ev_waveform);
        m_waveform_ntuple.Field("rocid", &ev_rocid);
        m_waveform_ntuple.Open("waveform_tree", *m_root_output_file);
        m_pulse_ntuple.Field("integral_sum", &ev_integral_sum);
        m_pulse_ntuple.Field("pedestal_sum", &pedestal_sum);
        m_pulse_ntuple.Field("coarse_time", &ev_coarse_time);
        m_pulse_ntuple.Field("fine_time", &ev_fine_time);
        m_pulse_ntuple.Field("pulse_peak", &ev_pulse_peak);
        m_pulse_ntuple.Field("pedestal_quality", &pedestal_quality);
        m_pulse_ntuple.Field("nhits", &number_hit);
        m_pulse_ntuple.Field("chan", &ev_pulse_chan);
        m_pulse_ntuple.Field("slot", &ev_pulse_slot);
        m_pulse_ntuple.Field("rocid", &ev_pulse_rocid);
        m_pulse_ntuple.Open("pulse_tree", *m_root_output_file);
        m_caen1190_ntuple.Field("rocid", &ev_caen_rocid);
        m_caen1190_ntuple.Field("slot", &ev_caen_slot);
        m_caen1190_ntuple.Field("chan", &ev_caen_chan);
        m_caen1190_ntuple.Field("measurement", &ev_caen_measurement);
        m_caen1190_ntuple.Field("opt", &ev_caen_opt);
        m_caen1190_ntuple.Field("flags", &ev_caen_flags);
        m_caen1190_ntuple.Field("trig_time", &ev_caen_trig_time);
        m_caen1190_ntuple.Field("hdr_chip_id", &ev_caen_hdr_chip_id);
        m_caen1190_ntuple.Field("hdr_event_id", &ev_caen_hdr_event_id);
        m_caen1190_ntuple.Field("hdr_bunch_id", &ev_caen_hdr_bunch_id);
        m_caen1190_ntuple.Field("trl_status", &ev_caen_trl_status);
        m_caen1190_ntuple.Open("caen1190_tree", *m_root_output_file);
        m_helicity_ntuple.Field("helicity_seed", &heldec.helicity_seed);
        m_helicity_ntuple.Field("n_tstable_fall", &heldec.n_tstable_fall);
        m_helicity_ntuple.Field("n_tstable_rise", &heldec.n_tstable_rise);
        m_helicity_ntuple.Field("n_pattsync", &heldec.n_pattsync);
        m_helicity_ntuple.Field("n_pairsync", &heldec.n_pairsync);
        m_helicity_ntuple.Field("time_tstable_start", &heldec.time_tstable_start);
        m_helicity_ntuple.Field("time_tstable_end", &heldec.time_tstable_end);
        m_helicity_ntuple.Field("last_tstable_duration", &heldec.last_tstable_duration);
        m_helicity_ntuple.Field("last_tsettle_duration", &heldec.last_tsettle_duration);
        m_helicity_ntuple.Field("trig_tstable", &heldec.trig_tstable);
        m_helicity_ntuple.Field("trig_pattsync", &heldec.trig_pattsync);
        m_helicity_ntuple.Field("trig_pairsync", &heldec.trig_pairsync);
        m_helicity_ntuple.Field("trig_helicity", &heldec.trig_helicity);
        m_helicity_ntuple.Field("trig_pat0_helicity", &heldec.trig_pat0_helicity);
        m_helicity_ntuple.Field("trig_polarity", &heldec.trig_polarity);
        m_helicity_ntuple.Field("trig_pat_count", &heldec.trig_pat_count);
        m_helicity_ntuple.Field("last32wins_pattsync", &heldec.last32wins_pattsync);
        m_helicity_ntuple.Field("last32wins_pairsync", &heldec.last32wins_pairsync);
        m_helicity_ntuple.Field("last32wins_helicity", &heldec.last32wins_helicity);
        m_helicity_ntuple.Field("last32wins_pattsync_hel", &heldec.last32wins_pattsync_hel);
        m_helicity_ntuple.Open("m_tree", *m_root_output_file);
    } else {
    // Create ROOT tree for waveform data
    m_waveform_tree = new TTree("waveform_tree", "FADC250 Waveform Data (slot, channel, waveform)");
    m_waveform_tree->Branch("slot", &ev_slot);
    m_waveform_tree->Branch("chan", &ev_chan);
    m_waveform_tree->Branch("waveform", &ev_waveform);
    m_waveform_tree->Branch("rocid", &ev_rocid);

    // Create ROOT tree for pulse data
    m_pulse_tree = new TTree("pulse_tree","FADC250 pulse data(slow, channel, integral, time)");
    m_pulse_tree->Branch("integral_sum", &ev_integral_sum);
    m_pulse_tree->Branch("pedestal_sum", &pedestal_sum);
    m_pulse_tree->Branch("coarse_time",&ev_coarse_time);
    m_pulse_tree->Branch("fine_time",&ev_fine_time);
    m_pulse_tree->Branch("pulse_peak",&ev_pulse_peak);
    m_pulse_tree->Branch("pedestal_quality",&pedestal_quality);
    m_pulse_tree->Branch("nhits",&number_hit);
    m_pulse_tree->Branch("chan",&ev_pulse_chan);
    m_pulse_tree->Branch("slot",&ev_pulse_slot);
    m_pulse_tree->Branch("rocid", &ev_pulse_rocid);

    // Create ROOT tree for CAEN1190 data
    m_caen1190_tree = new TTree("caen1190_tree","CAEN1190 data (slot, channel, time)");
    m_caen1190_tree->Branch("rocid", &ev_caen_rocid);
    m_caen1190_tree->Branch("slot", &ev_caen_slot);
    m_caen1190_tree->Branch("chan", &ev_caen_chan);
    m_caen1190_tree->Branch("measurement", &ev_caen_measurement);
    m_caen1190_tree->Branch("opt", &ev_caen_opt);
    m_caen1190_tree->Branch("flags", &ev_caen_flags);
    m_caen1190_tree->Branch("trig_time", &ev_caen_trig_time);
    m_caen1190_tree->Branch("hdr_chip_id", &ev_caen_hdr_chip_id);
    m_caen1190_tree->Branch("hdr_event_id", &ev_caen_hdr_event_id);
    m_caen1190_tree->Branch("hdr_bunch_id", &ev_caen_hdr_bunch_id);
    m_caen1190_tree->Branch("trl_status", &ev_caen_trl_status);

    // Create the Helicity Decoder Tree
    m_tree = new TTree("m_tree", "Physics Event Tree");
    m_tree->Branch(
        "heldec",
        &heldec,
        "helicity_seed/i:"
        "n_tstable_fall/i:"
        "n_tstable_rise/i:"
        "n_pattsync/i:"
        "n_pairsync/i:"
        "time_tstable_start/i:"
        "time_tstable_end/i:"
        "last_tstable_duration/i:"
        "last_tsettle_duration/i:"
        "trig_tstable/i:"
        "trig_pattsync/i:"
        "trig_pairsync/i:"
        "trig_helicity/i:"
        "trig_pat0_helicity/i:"
        "trig_polarity/i:"
        "trig_pat_count/i:"
        "last32wins_pattsync/i:"
        "last32wins_pairsync/i:"
        "last32wins_helicity/i:"
        "last32wins_pattsync_hel/i"
    );

    }

    // Create histogram for pulse integral distribution
    m_pulse_integral_hist = new TH1I("h_integral", "Pulse Integral Distribution;Integral Sum;Counts", 100, 0, 1);
    m_pulse_integral_hist->SetCanExtend(TH1::kAllAxes);  // Allow ROOT to automatically extend bins
}

/**
 * @brief Process a single event sequentially
 * 
 * Processes FADC250 detector data for a single event. Fills ROOT tree with
 * waveform data and histogram with pulse integral values. This method is 
 * called for each event in the processing pipeline.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JEventProcessor_EVIO::ProcessSequential(const JEvent &event) {
    
    // Clear previous event data
    ev_slot.clear();
    ev_chan.clear();
    ev_waveform.clear();
    ev_rocid.clear();
    ev_coarse_time.clear();
    ev_pulse_chan.clear();
    ev_pulse_slot.clear();
    ev_fine_time.clear();
    ev_integral_sum.clear();
    ev_pulse_peak.clear();
    ev_pulse_rocid.clear();
    pedestal_sum= 0;
    pedestal_quality = 0;
    number_hit =0;

    // Clear previous event data - CAEN1190
    ev_caen_rocid.clear();
    ev_caen_slot.clear();
    ev_caen_chan.clear();
    ev_caen_measurement.clear();
    ev_caen_opt.clear();
    ev_caen_flags.clear();
    ev_caen_trig_time.clear();
    ev_caen_hdr_chip_id.clear();
    ev_caen_hdr_event_id.clear();
    ev_caen_hdr_bunch_id.clear();
    ev_caen_trl_status.clear();

    // CAEN1190 TDC hits
    for (const auto& caen_hit : m_caen1190_hits_in()) {
        ev_caen_rocid.push_back(caen_hit->rocid);
        ev_caen_slot.push_back(caen_hit->slot);
        ev_caen_chan.push_back(caen_hit->chan);
        ev_caen_measurement.push_back(caen_hit->measurement);
        ev_caen_opt.push_back(caen_hit->opt);
        ev_caen_flags.push_back(caen_hit->flags);
        ev_caen_trig_time.push_back(caen_hit->trig_time);
        ev_caen_hdr_chip_id.push_back(caen_hit->hdr_chip_id);
        ev_caen_hdr_event_id.push_back(caen_hit->hdr_event_id);
        ev_caen_hdr_bunch_id.push_back(caen_hit->hdr_bunch_id);
        ev_caen_trl_status.push_back(caen_hit->glb_trl_status);

    }
    if (m_use_rntuple) m_caen1190_ntuple.Fill();
        else m_caen1190_tree->Fill();

    // FADC250 waveform hits
    for (const auto& waveform_hit : m_waveform_hits_in()) {
        const auto& waveform = waveform_hit->waveform;
        const auto sample_count = waveform.size();
        ev_slot.insert(ev_slot.end(), sample_count, waveform_hit->slot);
        ev_chan.insert(ev_chan.end(), sample_count, waveform_hit->chan);
        ev_rocid.insert(ev_rocid.end(), sample_count, waveform_hit->rocid);
        ev_waveform.insert(ev_waveform.end(), waveform.begin(), waveform.end());
    }

    // FADC250 pulse hits
 
    int nn=0;
    for (const auto& pulse_hit : m_pulse_hits_in()){
        integral_sum = pulse_hit->integral_sum;
        pedestal_sum = pulse_hit->pedestal_sum;
        coarse_time = pulse_hit->coarse_time;
        fine_time = pulse_hit->fine_time;
        pulse_peak = pulse_hit->pulse_peak;
        pedestal_quality = pulse_hit->pedestal_quality;
        if(integral_sum!=0){
            nn++;
            ev_integral_sum.push_back(integral_sum);
            ev_coarse_time.push_back(coarse_time);
            ev_fine_time.push_back(fine_time);
            ev_pulse_peak.push_back(pulse_peak);
            ev_pulse_slot.push_back(pulse_hit->slot);
            ev_pulse_chan.push_back(pulse_hit->chan);
            ev_pulse_rocid.push_back(pulse_hit->rocid);
        }
       
        
    }

    number_hit = nn;
    if (m_use_rntuple) m_waveform_ntuple.Fill();
        else m_waveform_tree->Fill();
    if(nn>0){
        if (m_use_rntuple) m_pulse_ntuple.Fill();
        else m_pulse_tree->Fill();
    }


    // FADC250 pulse hits
    for (const auto& pulse_hit : m_pulse_hits_in()) {
        // Fill histogram with pulse integral values
        m_pulse_integral_hist->Fill(pulse_hit->integral_sum);
    }

    heldec = {};
    // Helicity decoder data
    for(const auto& heldec_hit : m_heldec_data_in()){
	heldec.helicity_seed          = heldec_hit->helicity_seed;
        heldec.n_tstable_fall         = heldec_hit->n_tstable_fall;
        heldec.n_tstable_rise         = heldec_hit->n_tstable_rise;
        heldec.n_pattsync             = heldec_hit->n_pattsync;
        heldec.n_pairsync             = heldec_hit->n_pairsync;
        heldec.time_tstable_start     = heldec_hit->time_tstable_start;
        heldec.time_tstable_end       = heldec_hit->time_tstable_end;
        heldec.last_tstable_duration  = heldec_hit->last_tstable_duration;
        heldec.last_tsettle_duration  = heldec_hit->last_tsettle_duration;
        heldec.trig_tstable           = heldec_hit->trig_tstable;
        heldec.trig_pattsync          = heldec_hit->trig_pattsync;
        heldec.trig_pairsync          = heldec_hit->trig_pairsync;
        heldec.trig_helicity          = heldec_hit->trig_helicity;
        heldec.trig_pat0_helicity     = heldec_hit->trig_pat0_helicity;
        heldec.trig_polarity          = heldec_hit->trig_polarity;
        heldec.trig_pat_count         = heldec_hit->trig_pat_count;
        heldec.last32wins_pattsync    = heldec_hit->last32wins_pattsync;
        heldec.last32wins_pairsync    = heldec_hit->last32wins_pairsync;
        heldec.last32wins_helicity    = heldec_hit->last32wins_helicity;
        heldec.last32wins_pattsync_hel= heldec_hit->last32wins_pattsync_hel;
        if (m_use_rntuple) m_helicity_ntuple.Fill();
        else m_tree->Fill();
    }

}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_EVIO::Finish() {
    LOG << "JEventProcessor_EVIO::Finish" << LOG_END;

    // Write ROOT objects and close ROOT file
    if (m_root_output_file) {
        if (m_use_rntuple) {
            // Commit all RNTuple footers before closing their shared ROOT file.
            m_waveform_ntuple.Finish();
            m_pulse_ntuple.Finish();
            m_caen1190_ntuple.Finish();
            m_helicity_ntuple.Finish();
        } else {
            m_waveform_tree->Write();
            m_tree->Write();
            m_pulse_tree->Write();
            m_caen1190_tree->Write();
        }
        m_root_output_file->cd();
        m_pulse_integral_hist->Write();
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }

}
