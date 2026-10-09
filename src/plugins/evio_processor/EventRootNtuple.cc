#include "EventRootNtuple.h"
#include <JANA/JEventSource.h>
#include "CAEN1190Hit.h"
#include "EventHits_FADC.h"
#include "HelicityDecoderData.h"
#include <type_traits>
#include <unordered_map>

void EventRootRecord::Load(const JEvent& event) {
    const auto& parent = event.HasParent(JEventLevel::Block) ? event.GetParent(JEventLevel::Block) : event;
    auto* source = parent.GetJEventSource();
    if (source != cached_source) {
        cached_source = source;
        source_file = source ? source->GetResourceName() : "";
    }
    run_number = event.GetRunNumber();
    block_number = parent.GetEventNumber();
    event_number = event.GetEventNumber();
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
    for (const auto& caen_hit : event.Get<CAEN1190Hit>("", false)) {
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


    // FADC250 waveform hits
    for (const auto& waveform_hit : event.Get<FADC250WaveformHit>("", false)) {
        const auto& waveform = waveform_hit->waveform;
        const auto sample_count = waveform.size();
        ev_slot.insert(ev_slot.end(), sample_count, waveform_hit->slot);
        ev_chan.insert(ev_chan.end(), sample_count, waveform_hit->chan);
        ev_rocid.insert(ev_rocid.end(), sample_count, waveform_hit->rocid);
        ev_waveform.insert(ev_waveform.end(), waveform.begin(), waveform.end());
    }

    // FADC250 pulse hits

    int nn=0;
    for (const auto& pulse_hit : event.Get<FADC250PulseHit>("", false)){
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






    hel_helicity_seed.clear();
    hel_n_tstable_fall.clear();
    hel_n_tstable_rise.clear();
    hel_n_pattsync.clear();
    hel_n_pairsync.clear();
    hel_time_tstable_start.clear();
    hel_time_tstable_end.clear();
    hel_last_tstable_duration.clear();
    hel_last_tsettle_duration.clear();
    hel_trig_tstable.clear();
    hel_trig_pattsync.clear();
    hel_trig_pairsync.clear();
    hel_trig_helicity.clear();
    hel_trig_pat0_helicity.clear();
    hel_trig_polarity.clear();
    hel_trig_pat_count.clear();
    hel_last32wins_pattsync.clear();
    hel_last32wins_pairsync.clear();
    hel_last32wins_helicity.clear();
    hel_last32wins_pattsync_hel.clear();

    // Helicity decoder data
    for(const auto& heldec_hit : event.Get<HelicityDecoderData>("", false)){
	hel_helicity_seed.push_back(heldec_hit->helicity_seed);
        hel_n_tstable_fall.push_back(heldec_hit->n_tstable_fall);
        hel_n_tstable_rise.push_back(heldec_hit->n_tstable_rise);
        hel_n_pattsync.push_back(heldec_hit->n_pattsync);
        hel_n_pairsync.push_back(heldec_hit->n_pairsync);
        hel_time_tstable_start.push_back(heldec_hit->time_tstable_start);
        hel_time_tstable_end.push_back(heldec_hit->time_tstable_end);
        hel_last_tstable_duration.push_back(heldec_hit->last_tstable_duration);
        hel_last_tsettle_duration.push_back(heldec_hit->last_tsettle_duration);
        hel_trig_tstable.push_back(heldec_hit->trig_tstable);
        hel_trig_pattsync.push_back(heldec_hit->trig_pattsync);
        hel_trig_pairsync.push_back(heldec_hit->trig_pairsync);
        hel_trig_helicity.push_back(heldec_hit->trig_helicity);
        hel_trig_pat0_helicity.push_back(heldec_hit->trig_pat0_helicity);
        hel_trig_polarity.push_back(heldec_hit->trig_polarity);
        hel_trig_pat_count.push_back(heldec_hit->trig_pat_count);
        hel_last32wins_pattsync.push_back(heldec_hit->last32wins_pattsync);
        hel_last32wins_pairsync.push_back(heldec_hit->last32wins_pairsync);
        hel_last32wins_helicity.push_back(heldec_hit->last32wins_helicity);
        hel_last32wins_pattsync_hel.push_back(heldec_hit->last32wins_pattsync_hel);

    }

}

EventRootNtuple::Worker::Worker(ROOT::Experimental::RNTupleParallelWriter& writer)
    : context(writer.CreateFillContext()), entry(context->GetModel().CreateBareEntry()) {
    record.Fields([&](const char* name, auto& value) { entry->BindRawPtr(name, &value); });
}

EventRootNtuple::EventRootNtuple(TFile& file, bool parallel) {
    if (parallel) {
        auto model = ROOT::Experimental::RNTupleModel::CreateBare();
        m_record.Fields([&](const char* name, auto& value) {
            model->MakeField<std::decay_t<decltype(value)>>(name);
        });
        ROOT::Experimental::RNTupleWriteOptions options;
        options.SetCompression(file.GetCompressionSettings());
        m_parallel = ROOT::Experimental::RNTupleParallelWriter::Append(std::move(model), "events", file, options);
    } else {
        m_record.Fields([&](const char* name, auto& value) { m_ntuple.Field(name, &value); });
        m_ntuple.Open("events", file);
    }
}

std::shared_ptr<EventRootNtuple::Worker> EventRootNtuple::GetWorker() {
    // Weak caches expire at Finish and cannot retain ROOT contexts or stale owners.
    thread_local std::unordered_map<const EventRootNtuple*, std::weak_ptr<Worker>> cache;
    if (auto worker = cache[this].lock()) return worker;
    std::lock_guard<std::mutex> lock(m_workers_mutex);
    auto worker = std::make_shared<Worker>(*m_parallel);
    m_workers.push_back(worker);
    cache[this] = worker;
    return worker;
}

void EventRootNtuple::Fill(const JEvent& event) {
    if (m_parallel) GetWorker()->Fill(event);
    else { m_record.Load(event); m_ntuple.Fill(); }
}

void EventRootNtuple::Finish() {
    // Finish runs after the JANA workers stop. Contexts must precede the writer.
    for (auto& worker : m_workers) worker->context->FlushCluster();
    m_workers.clear();
    m_parallel.reset();
    m_ntuple.Finish();
}
