// Exact ordered payload comparison across formats; helicity leaves become fields.
#include <ROOT/RNTupleReader.hxx>
#include <TBranchElement.h>
#include <TFile.h>
#include <TH1.h>
#include <TLeaf.h>
#include <TTree.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

void compare_ttree_rntuple(const char* treePath, const char* ntuplePath) {
    auto require=[](bool ok,const std::string& why){if(!ok) throw std::runtime_error(why);};
    TFile trees(treePath), ntuples(ntuplePath);
    require(!trees.IsZombie() && !ntuples.IsZombie(),"file open");
    for (const char* name : {"waveform_tree","pulse_tree","caen1190_tree","m_tree"}) {
        auto* tree=trees.Get<TTree>(name);
        auto reader=ROOT::Experimental::RNTupleReader::Open(name,ntuplePath);
        require(tree && tree->GetEntries()==static_cast<Long64_t>(reader->GetNEntries()),std::string(name)+" count");
        for(auto* obj:*tree->GetListOfBranches()) {
            auto* branch=static_cast<TBranch*>(obj);
            const std::string classname=branch->GetClassName();
            if(classname=="vector<unsigned int>") {
                auto view=reader->GetView<std::vector<unsigned int>>(branch->GetName());
                for(Long64_t i=0;i<tree->GetEntries();++i) {
                    require(branch->GetEntry(i)>0,"branch read");
                    auto* value=reinterpret_cast<const std::vector<unsigned int>*>(static_cast<TBranchElement*>(branch)->GetObject());
                    require(value && *value==view(i),std::string(name)+"/"+branch->GetName()+" entry "+std::to_string(i));
                }
            } else {
                require(classname.empty(),"unsupported class "+classname);
                for(auto* objLeaf:*branch->GetListOfLeaves()) {
                    auto* leaf=static_cast<TLeaf*>(objLeaf);
                    std::string type=leaf->GetTypeName();
                    if(type=="UInt_t") {
                        auto view=reader->GetView<unsigned int>(leaf->GetName());
                        for(Long64_t i=0;i<tree->GetEntries();++i) {
                            require(branch->GetEntry(i)>0,"branch read");
                            require(static_cast<unsigned int>(leaf->GetValueLong64())==view(i),std::string(name)+"/"+leaf->GetName()+" entry "+std::to_string(i));
                        }
                    } else {
                        require(type=="Int_t","unsupported scalar "+type);
                        auto view=reader->GetView<int>(leaf->GetName());
                        for(Long64_t i=0;i<tree->GetEntries();++i) {
                            require(branch->GetEntry(i)>0,"branch read");
                            require(leaf->GetValueLong64()==view(i),std::string(name)+"/"+leaf->GetName()+" entry "+std::to_string(i));
                        }
                    }
                }
            }
        }
        std::cout<<name<<": every ordered value matches ("<<tree->GetEntries()<<" entries)"<<std::endl;
    }
    auto* a=trees.Get<TH1>("h_integral");auto* b=ntuples.Get<TH1>("h_integral");
    require(a && b && a->GetEntries()==b->GetEntries() && a->GetNcells()==b->GetNcells(),"histogram count");
    for(int i=0;i<a->GetNcells();++i) require(a->GetBinContent(i)==b->GetBinContent(i) && a->GetBinError(i)==b->GetBinError(i),"histogram bin/error");
    for(int i=1;i<=a->GetNbinsX()+1;++i) require(a->GetXaxis()->GetBinLowEdge(i)==b->GetXaxis()->GetBinLowEdge(i),"histogram edge");
    std::cout<<"Histogram bins, errors and edges match"<<std::endl;
}
