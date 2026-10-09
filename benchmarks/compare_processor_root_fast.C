// Compare complete ordered processor output in C++, avoiding Python per-value overhead.
#include <TBranch.h>
#include <TBranchElement.h>
#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TLeaf.h>
#include <TTree.h>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

void compare_processor_root_fast(const char* before, const char* after) {
    TFile a(before, "READ"), b(after, "READ");
    auto require = [](bool ok, const std::string& where) {
        if (!ok) throw std::runtime_error("ROOT comparison failed: " + where);
    };
    require(!a.IsZombie() && !b.IsZombie(), "file open");
    auto names = [](TFile& file) {
        std::set<std::string> result;
        for (auto* key : *file.GetListOfKeys()) result.insert(key->GetName());
        return result;
    };
    require(names(a) == names(b), "object names");
    for (const auto& name : names(a)) {
        TObject *x = a.Get(name.c_str()), *y = b.Get(name.c_str());
        require(x && y && std::string(x->ClassName()) == y->ClassName(), name);
        if (auto* ta = dynamic_cast<TTree*>(x)) {
            auto* tb = dynamic_cast<TTree*>(y);
            require(ta->GetEntries() == tb->GetEntries(), name + " entries");
            require(ta->GetListOfBranches()->GetEntries() == tb->GetListOfBranches()->GetEntries(), name + " branches");
            std::vector<std::pair<TBranch*, TBranch*>> branches;
            for (auto* obj : *ta->GetListOfBranches()) {
                auto* ba = static_cast<TBranch*>(obj);
                auto* bb = tb->GetBranch(ba->GetName());
                require(bb && std::string(ba->GetTitle()) == bb->GetTitle() && std::string(ba->GetClassName()) == bb->GetClassName(), name + "/" + ba->GetName());
                const std::string classname = ba->GetClassName();
                require(classname.empty() || classname == "vector<unsigned int>", "unsupported branch class " + classname);
                require(ba->GetListOfLeaves()->GetEntries() == bb->GetListOfLeaves()->GetEntries(), name + " leaves");
                for (int i=0; i<ba->GetListOfLeaves()->GetEntries(); ++i) {
                    auto* la = static_cast<TLeaf*>(ba->GetListOfLeaves()->At(i));
                    auto* lb = static_cast<TLeaf*>(bb->GetListOfLeaves()->At(i));
                    require(std::string(la->GetName()) == lb->GetName() && std::string(la->GetTypeName()) == lb->GetTypeName(), name + " leaf schema");
                    if (classname.empty()) require(std::string(la->GetTypeName()) == "UInt_t" || std::string(la->GetTypeName()) == "Int_t", "unsupported leaf type");
                }
                branches.emplace_back(ba, bb);
            }
            for (Long64_t entry=0; entry<ta->GetEntries(); ++entry) {
                require(ta->GetEntry(entry) > 0 && tb->GetEntry(entry) > 0, name + " read");
                for (auto [ba, bb] : branches) {
                    if (std::string(ba->GetClassName()) == "vector<unsigned int>") {
                        auto* va = reinterpret_cast<const std::vector<unsigned int>*>(static_cast<TBranchElement*>(ba)->GetObject());
                        auto* vb = reinterpret_cast<const std::vector<unsigned int>*>(static_cast<TBranchElement*>(bb)->GetObject());
                        require(va && vb && *va == *vb, name + "/" + ba->GetName() + " entry " + std::to_string(entry));
                    } else {
                        for (int i=0; i<ba->GetListOfLeaves()->GetEntries(); ++i) {
                            auto* la = static_cast<TLeaf*>(ba->GetListOfLeaves()->At(i));
                            auto* lb = static_cast<TLeaf*>(bb->GetListOfLeaves()->At(i));
                            require(la->GetNdata() == lb->GetNdata(), name + " leaf length");
                            for (int j=0; j<la->GetNdata(); ++j) require(la->GetValueLong64(j) == lb->GetValueLong64(j), name + "/" + la->GetName() + " entry " + std::to_string(entry));
                        }
                    }
                }
            }
            std::cout << name << ": all " << ta->GetEntries() << " ordered entries match\n";
        } else if (auto* ha = dynamic_cast<TH1*>(x)) {
            auto* hb = dynamic_cast<TH1*>(y);
            require(ha->GetEntries() == hb->GetEntries() && ha->GetNcells() == hb->GetNcells(), name + " bins");
            for (int i=0; i<ha->GetNcells(); ++i) require(ha->GetBinContent(i) == hb->GetBinContent(i) && ha->GetBinError(i) == hb->GetBinError(i), name + " bin content/error");
            for (int i=1; i<=ha->GetNbinsX()+1; ++i) require(ha->GetXaxis()->GetBinLowEdge(i) == hb->GetXaxis()->GetBinLowEdge(i), name + " edges");
            std::cout << name << ": all bins and errors match\n";
        } else {
            throw std::runtime_error("Unsupported ROOT object " + name);
        }
    }
}
