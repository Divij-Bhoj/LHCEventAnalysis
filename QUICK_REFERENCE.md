# Quick Reference Card

## Essential Commands (Copy-Paste Ready)

### Environment Setup (Run Once Per Terminal Session)
```bash
source ~/cern-workspace/root/bin/thisroot.sh
export FASTJET_DIR=~/cern-workspace/fastjet-install
export LD_LIBRARY_PATH=$FASTJET_DIR/lib:$LD_LIBRARY_PATH
```

### Build Project
```bash
cd ~/cern-workspace/LHCEventAnalysis
mkdir -p build && cd build
cmake ..
make -j4
```

### Run Analysis Workflow
```bash
# 1. Generate data
./GenerateEvents

# 2. Run analysis
./RunAnalysis

# 3. Benchmark performance
./BenchmarkPerformance

# 4. View results
root -l results/analysis_output.root
```

### Git Workflow
```bash
# Stage changes
git add .

# Commit
git commit -m "Your message here"

# Push to GitHub
git push origin main

# Check status
git status
```

### Useful ROOT Commands
```cpp
// Inside root -l results/analysis_output.root

// List all histograms
.ls

// Get and draw histogram
TH1F* h = (TH1F*)gDirectory->Get("z_mass")
h->Draw()

// Fit Gaussian
h->Fit("gaus", "", "", 85, 97)

// Save plot
c1->SaveAs("plot.png")

// Exit
.q
```

### Cleanup Build
```bash
cd ~/cern-workspace/LHCEventAnalysis
rm -rf build/
```

### Generate More Events
```bash
# 100k events
./GenerateEvents 100000 data/events_100k.root

# Run on custom data
./RunAnalysis data/events_100k.root results/output_100k.root
```

### Check File Contents
```bash
# View ROOT file structure
rootls -t results/analysis_output.root

# Check data file size
ls -lh data/*.root
```

### Performance Profiling
```bash
# Time execution
time ./RunAnalysis

# Check CPU usage
top
```

## Directory Structure
```
LHCEventAnalysis/
├── include/          # Header files (.h)
├── src/              # Source files (.cpp)
├── scripts/          # Python plotting scripts
├── data/             # Generated ROOT files
├── results/          # Analysis output
└── build/            # Compiled binaries (auto-generated)
    └── bin/          # Executables
```

## File Descriptions

| File | Purpose |
|------|---------|
| `GenerateEvents` | Create simulated collision data |
| `RunAnalysis` | Run Z and top analyses |
| `BenchmarkPerformance` | Test parallelization speedup |
| `*.root` | ROOT data files (TTree format) |

## Expected Output Sizes

| File | Typical Size |
|------|--------------|
| `simulated_events.root` (50k) | ~10 MB |
| `analysis_output.root` | ~500 KB |
| All source code | ~50 KB |

## Common Error Messages

| Error | Solution |
|-------|----------|
| "ROOT not found" | Run `source ~/cern-workspace/root/bin/thisroot.sh` |
| "libfastjet.so: cannot open" | Check `$FASTJET_DIR` and `$LD_LIBRARY_PATH` |
| "CMakeCache.txt exists" | Delete `build/` and recreate |
| "Permission denied" | Run `chmod +x bin/*` |

## Performance Targets

| Metric | Expected Value |
|--------|----------------|
| Events/sec (1 thread) | ~15,000 - 20,000 |
| Events/sec (4 threads) | ~50,000 - 65,000 |
| Speedup (4 threads) | 3.0x - 3.5x |
| Z→μμ efficiency | ~25-30% |
| tt̄ efficiency | ~15-20% |

## Physics Constants

| Particle | Mass (GeV) | Symbol |
|----------|------------|--------|
| Z boson | 91.2 | Z |
| W boson | 80.4 | W |
| Top quark | 173.0 | t |
| Muon | 0.105 | μ |

## ROOT Histogram Names

| Name | Description |
|------|-------------|
| `z_mass` | Di-muon invariant mass |
| `z_pt` | Z boson transverse momentum |
| `muon_pt` | Muon pT distribution |
| `top_hadronic_mass` | Hadronic top mass |
| `w_hadronic_mass` | Hadronic W mass |
| `n_jets` | Jet multiplicity |
| `met` | Missing transverse energy |

## Keyboard Shortcuts in ROOT

| Key | Action |
|-----|--------|
| `Ctrl+C` | Stop current process |
| `.q` | Quit ROOT |
| `.ls` | List objects |
| `.!ls` | Execute shell command |
| Up Arrow | Previous command |

## GitHub Quick Commands

```bash
# Clone your repo elsewhere
git clone https://github.com/Divij-Bhoj/LHCEventAnalysis.git

# View commit history
git log --oneline

# View remote URL
git remote -v

# Pull latest changes
git pull origin main
```

## Recommended Customizations

```bash
# Add these to ~/.bashrc for convenience

# Alias for CERN workspace
alias cern='cd ~/cern-workspace/LHCEventAnalysis'

# Auto-setup ROOT environment
source ~/cern-workspace/root/bin/thisroot.sh
export FASTJET_DIR=~/cern-workspace/fastjet-install
export LD_LIBRARY_PATH=$FASTJET_DIR/lib:$LD_LIBRARY_PATH
```
