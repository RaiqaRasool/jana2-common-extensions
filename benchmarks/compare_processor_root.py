"""Compare every tree value and histogram bin in two sequential writer outputs."""
import sys
import ROOT


def compare(before, after):
    files = [ROOT.TFile.Open(path) for path in (before, after)]
    try:
        assert all(f and not f.IsZombie() for f in files), 'Invalid ROOT file'
        names = [{k.GetName() for k in f.GetListOfKeys()} for f in files]
        assert names[0] == names[1], 'Object names differ'
        for name in sorted(names[0]):
            a, b = [f.Get(name) for f in files]
            assert a.ClassName() == b.ClassName(), name
            if a.InheritsFrom('TTree'):
                schemas = [[(br.GetName(), br.GetTitle(), br.GetClassName()) for br in obj.GetListOfBranches()] for obj in (a, b)]
                assert schemas[0] == schemas[1], f'{name}: branch schema differs'
                assert a.GetEntries() == b.GetEntries(), f'{name}: entry counts differ'
                for entry in range(a.GetEntries()):
                    a.GetEntry(entry)
                    b.GetEntry(entry)
                    for branch, _, classname in schemas[0]:
                        if 'vector<' in classname:
                            assert list(getattr(a, branch)) == list(getattr(b, branch)), (name, entry, branch)
                        else:
                            leaves = [obj.GetBranch(branch).GetListOfLeaves() for obj in (a,b)]
                            assert [(l.GetName(), l.GetTypeName()) for l in leaves[0]] == [(l.GetName(), l.GetTypeName()) for l in leaves[1]]
                            for x, y in zip(leaves[0], leaves[1]):
                                assert x.GetNdata() == y.GetNdata()
                                assert [x.GetValueLong64(i) for i in range(x.GetNdata())] == [y.GetValueLong64(i) for i in range(y.GetNdata())], (name, entry, branch)
                print(f'{name}: all {a.GetEntries()} entries match')
            elif a.InheritsFrom('TH1'):
                assert a.GetEntries() == b.GetEntries() and a.GetNcells() == b.GetNcells(), name
                for i in range(a.GetNcells()):
                    assert a.GetBinContent(i) == b.GetBinContent(i) and a.GetBinError(i) == b.GetBinError(i), (name, i)
                for i in range(1, a.GetNbinsX()+2):
                    assert a.GetXaxis().GetBinLowEdge(i) == b.GetXaxis().GetBinLowEdge(i), (name, i)
                print(f'{name}: all bins and errors match')
            else:
                raise AssertionError(f'Unsupported object {name}')
    finally:
        for f in files:
            if f:
                f.Close()


if __name__ == '__main__':
    compare(*sys.argv[1:])
