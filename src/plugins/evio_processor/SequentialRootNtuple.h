#pragma once
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleWriteOptions.hxx>
#include <TFile.h>

// Bind existing event buffers directly: Fill does not require another vector copy.
// This writer is used only by the sequential JANA callback.
class SequentialRootNtuple {
    std::unique_ptr<ROOT::Experimental::RNTupleModel> m_model = ROOT::Experimental::RNTupleModel::Create();
    std::unique_ptr<ROOT::Experimental::RNTupleWriter> m_writer;
public:
    template<class T> void Field(const char* name, T* value) {
        m_model->MakeField<T>(name);
        m_model->GetDefaultEntry().BindRawPtr<T>(name, value);
    }
    void Open(const char* name, TFile& file) {
        ROOT::Experimental::RNTupleWriteOptions options;
        // Match the TTree control's compression algorithm and level.
        options.SetCompression(file.GetCompressionSettings());
        m_writer = ROOT::Experimental::RNTupleWriter::Append(std::move(m_model), name, file, options);
    }
    void Fill() { m_writer->Fill(); }
    void Finish() { m_writer.reset(); }
};
