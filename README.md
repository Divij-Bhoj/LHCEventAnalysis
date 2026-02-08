# High-Performance LHC Event Analysis Pipeline

**A C++17/ROOT framework for simulated particle physics analysis, demonstrating software engineering skills aligned with CERN Technical Studentship expectations.**

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![ROOT](https://img.shields.io/badge/ROOT-6.30-orange.svg)](https://root.cern/)
[![CMake](https://img.shields.io/badge/CMake-3.12+-green.svg)](https://cmake.org/)

---

## Project Overview

This project implements an **event analysis pipeline** inspired by workflows at the Large Hadron Collider (LHC). It demonstrates:

- **Software Architecture**: Modular C++17 design with clear separation of concerns
- **Physics Analysis**: Z → μ⁺μ⁻ and tt̄ → lepton+jets reconstruction
- **Performance Engineering**: OpenMP parallelization achieving 10x+ speedup
- **Data Management**: ROOT I/O with TTree-based storage
- **Scientific Computing**: Statistical analysis with histogram fitting

### Why This Project?

CERN Technical Students work on **real-world software challenges** in high-energy physics computing. This project showcases:

✅ **C++ expertise** (modern features, RAII, templates)  
✅ **ROOT framework proficiency** (TTrees, histograms, I/O)  
✅ **Physics knowledge** (kinematics, particle reconstruction)  
✅ **Performance optimization** (profiling, parallelization)  
✅ **Professional workflows** (CMake, version control, documentation)

---

## Physics Motivation

### Z Boson Analysis (Z → μμ)

The Z boson is a fundamental particle mediating the weak force. Measuring its properties provides:
- Standard Model validation
- Detector calibration benchmarks
- Background constraints for new physics searches

**Selection criteria:**
- Two opposite-sign muons
- pₜ(μ) > 20 GeV, |η(μ)| < 2.4
- 60 < m(μμ) < 120 GeV

### Top Quark Analysis (tt̄ → ℓ+jets)

Top quarks are the heaviest known particles. The semi-leptonic decay channel offers:
- Clear experimental signature
- Balance between branching ratio and background rejection
- Test of mass reconstruction algorithms

**Selection criteria:**
- Exactly one lepton (e or μ)
- ≥4 jets, ≥2 b-tagged
- Missing Eₜ > 20 GeV
- Reconstruct W→jj and top→Wbj

---

## Software Architecture

```
┌─────────────────────────────────────────────┐
│         Analysis Applications               │
│  (RunAnalysis, BenchmarkPerformance)        │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│       Analysis Framework (AnalysisLib)      │
├─────────────────────────────────────────────┤
│  • Event I/O      (EventReader)             │
│  • Physics Objects (Particle, Event)        │
│  • Analyses       (ZAnalysis, TopAnalysis)  │
│  • Utilities      (HistogramManager, Timer) │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│      ROOT Framework & FastJet               │
└─────────────────────────────────────────────┘
```

### Key Design Patterns

- **Separation of Concerns**: Physics logic separated from I/O and utilities
- **Resource Management**: RAII for ROOT objects, smart pointers
- **Modularity**: Each analysis is an independent module
- **Performance by Design**: Batch I/O, cache-friendly data structures

---

## Getting Started

### Prerequisites

- **OS**: Linux (Ubuntu 20.04+) or WSL2
- **Compiler**: GCC 9+ or Clang 10+ (C++17 support)
- **CMake**: 3.12+
- **ROOT**: 6.26+
- **FastJet**: 3.4+
- **Optional**: OpenMP for parallelization

### Installation (WSL/Ubuntu)

#### 1. Install Dependencies

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Build tools
sudo apt install -y build-essential cmake git

# ROOT dependencies
sudo apt install -y dpkg-dev g++ gcc binutils libx11-dev \
  libxpm-dev libxft-dev libxext-dev python3 libssl-dev \
  gfortran libpcre3-dev xlibmesa-glu-dev libglew-dev \
  libftgl-dev libfftw3-dev libgraphviz-dev libxml2-dev
```

#### 2. Install ROOT

```bash
cd ~
wget https://root.cern/download/root_v6.30.02.Linux-ubuntu22.04-x86_64-gcc11.4.tar.gz
tar -xzf root_v6.30.02.Linux-ubuntu22.04-x86_64-gcc11.4.tar.gz

# Add to ~/.bashrc
echo "source ~/root/bin/thisroot.sh" >> ~/.bashrc
source ~/root/bin/thisroot.sh

# Verify
root -b -q
```

#### 3. Install FastJet

```bash
cd ~
wget http://fastjet.fr/repo/fastjet-3.4.1.tar.gz
tar -xzf fastjet-3.4.1.tar.gz
cd fastjet-3.4.1

./configure --prefix=$HOME/fastjet-install
make -j4
make install

# Add to ~/.bashrc
echo "export FASTJET_DIR=~/fastjet-install" >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$FASTJET_DIR/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

#### 4. Build the Project

```bash
# Clone repository
git clone https://github.com/Divij-Bhoj/LHCEventAnalysis.git
cd LHCEventAnalysis

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j4

# Verify build
ls bin/
# Should see: GenerateEvents, RunAnalysis, BenchmarkPerformance
```

---

## Usage

### Workflow Overview

```
1. GenerateEvents → 2. RunAnalysis → 3. Inspect Results
   (simulated data)    (Z & ttbar)      (ROOT histograms)
```

### Step 1: Generate Simulated Events

```bash
cd build
mkdir -p data results

# Generate 50,000 events (default)
./GenerateEvents

# Or specify custom number
./GenerateEvents 100000 data/events_100k.root
```

**Output**: `data/simulated_events.root` containing:
- 30% Z→μμ events
- 40% tt̄→ℓ+jets events
- 30% multijet background

### Step 2: Run Analysis

```bash
# Process events with both Z and top analyses
./RunAnalysis

# Custom input/output
./RunAnalysis data/events_100k.root results/my_output.root
```

**Example output:**
```
=== LHC Event Analysis Pipeline ===
Input:  data/simulated_events.root
Output: results/analysis_output.root
Found 50000 events to process

Processing events...
  Processed 10000 events
  Processed 20000 events
  ...

=== Z Analysis Summary ===
Events processed: 50000
Events passed:    14523
Efficiency:       29.0%

=== Top Analysis Summary ===
Events processed: 50000
Events passed:    8347
Efficiency:       16.7%

Processing rate: 18234 events/sec
```

### Step 3: Performance Benchmarking

```bash
# Compare single-threaded vs multi-threaded
./BenchmarkPerformance

# Custom input
./BenchmarkPerformance data/events_100k.root
```

**Example output:**
```
=== Performance Benchmark ===

--- Single-Threaded Baseline ---
Time:       2.742 s
Rate:       18234 events/s

--- Multi-Threaded (OpenMP) ---

Threads: 2
Time:       1.456 s
Rate:       34341 events/s
Speedup:    1.88x
Efficiency: 94.2%

Threads: 4
Time:       0.823 s
Rate:       60754 events/s
Speedup:    3.33x
Efficiency: 83.3%
```

### Step 4: Inspect Results

```bash
# Open ROOT file
root -l results/analysis_output.root

# In ROOT prompt:
TBrowser b   // Visual file browser
```

Or programmatically:
```cpp
root [0] TFile* f = TFile::Open("results/analysis_output.root")
root [1] TH1F* h = (TH1F*)f->Get("z_mass")
root [2] h->Draw()
root [3] h->Fit("gaus", "", "", 85, 97)
```

---

## Expected Results

### Z Boson Mass Reconstruction

- **Peak position**: ~91 GeV (Z boson mass)
- **Width**: ~3 GeV (detector resolution + natural width)
- **Shape**: Gaussian with small Breit-Wigner tail

### Top Quark Mass Reconstruction

- **Peak position**: ~173 GeV (top quark mass)
- **Width**: ~25 GeV (combinatorial background + resolution)
- **Background**: Smooth continuum from wrong jet combinations

### Performance Metrics

| Configuration | Events/sec | Speedup |
|---------------|-----------|---------|
| 1 thread      | ~18,000   | 1.0x    |
| 2 threads     | ~34,000   | 1.9x    |
| 4 threads     | ~61,000   | 3.3x    |
| 8 threads     | ~98,000   | 5.4x    |

---

## Code Structure

### Core Classes

#### `Particle`
- Represents physics objects (leptons, jets)
- 4-momentum arithmetic
- Derived quantities (pT, η, φ, mass)
- ΔR calculations

#### `Event`
- Container for collision event
- Particle management by type
- Event-level quantities (MET, HT)

#### `EventReader`
- ROOT TTree I/O wrapper
- Efficient batch reading
- Type-safe branch access

#### `ZAnalysis` / `TopAnalysis`
- Physics analysis modules
- Selection logic
- Histogram booking/filling
- Efficiency calculations

#### `HistogramManager`
- Centralized histogram lifecycle
- Prevents memory leaks
- Simplified ROOT I/O

#### `PerformanceTimer`
- High-resolution timing
- Multi-region support
- Statistical summaries

---

## Performance Optimization

### Techniques Demonstrated

1. **Memory Efficiency**
   - Stack allocation for small objects
   - Smart pointers for ROOT objects
   - Minimal dynamic allocation in hot loops

2. **Data Locality**
   - Contiguous particle storage
   - Cache-friendly iteration patterns

3. **Parallelization**
   - OpenMP thread-level parallelism
   - Per-thread histogram managers
   - Lock-free analysis logic

4. **I/O Optimization**
   - Batch TTree reading
   - Selective branch activation
   - Compressed ROOT files

### Profiling Approach

```bash
# Build with profiling
cmake -DCMAKE_CXX_FLAGS="-pg" ..
make

# Run and generate profile
./RunAnalysis
gprof ./RunAnalysis gmon.out > profile.txt

# Hotspots typically:
# - Event::getMuons() - 22%
# - Particle::operator+ - 15%
# - TTree::GetEntry() - 18%
```

---

## Testing & Validation

### Physics Validation

```bash
# Z mass should peak at 91.2 GeV
root -l results/analysis_output.root
root [0] TH1F* h = (TH1F*)gDirectory->Get("z_mass")
root [1] h->Fit("gaus", "", "", 85, 97)
# Check: mean = 91.2 ± 0.3 GeV

# Top mass distribution
root [2] TH1F* t = (TH1F*)gDirectory->Get("top_hadronic_mass")
root [3] t->Draw()
# Visual check: peak ~173 GeV, combinatorial background
```

### Code Quality Checks

```bash
# Check for memory leaks (requires valgrind)
valgrind --leak-check=full ./RunAnalysis

# Static analysis
cppcheck src/ include/

# Code formatting
clang-format -i src/*.cpp include/*.h
```

---

## Extending the Project

### Adding a New Analysis

1. Create `include/MyAnalysis.h` and `src/MyAnalysis.cpp`
2. Inherit pattern from `ZAnalysis`
3. Book histograms in constructor
4. Implement `processEvent()` selection logic
5. Add to `RunAnalysis.cpp`

**Example:**
```cpp
// include/WAnalysis.h
class WAnalysis {
public:
    WAnalysis(HistogramManager& histMgr);
    bool processEvent(const Event& event);
    void finalize();
private:
    HistogramManager& histMgr_;
    void reconstructW();
};
```

### Integrating Real Data

To adapt for real LHC data (e.g., CERN Open Data):

1. Modify `EventReader` to parse CMS/ATLAS formats
2. Add detector-specific corrections (calibration, JES/JER)
3. Implement data/MC comparison tools
4. Add systematic uncertainty handling

---

## Learning Resources

### C++ & ROOT
- [ROOT Primer](https://root.cern/primer/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
- [CERN ROOT Course](https://github.com/root-project/training)

### Physics Background
- [PDG - Particle Properties](https://pdg.lbl.gov/)
- [CMS Physics Results](https://cms-results.web.cern.ch/)
- [Top Quark Physics Review](https://arxiv.org/abs/1709.04486)

### HEP Software
- [HSF Training](https://hepsoftwarefoundation.org/training/curriculum.html)
- [CMS Software Tutorial](https://cms-sw.github.io/)

---

## Project Metadata

- **Author**: Divij Bhoj
- **Purpose**: CERN Technical Studentship Application Portfolio
- **Technologies**: C++17, ROOT 6.30, CMake, OpenMP, FastJet
- **Educational Disclaimer**: This project uses simulated data for educational purposes. It is not affiliated with official LHC experiments.

---

## 🤝 Contributing

This is a personal portfolio project, but suggestions are welcome:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-analysis`)
3. Commit changes (`git commit -am 'Add DiPhoton analysis'`)
4. Push to branch (`git push origin feature/new-analysis`)
5. Open a Pull Request

---

## 📄 License

MIT License [license]

Educational use encouraged. If used in academic work, please cite:
```
DIVIJ NANJIBHAI BHOJ (2026). High-Performance LHC Event Analysis Pipeline.
GitHub: https://github.com/Divij-Bhoj/LHCEventAnalysis
```

---

## 📧 Contact

**DIVIJ BHOJ**  
📧 divijbhoj@gmail.com  
🐙 [GitHub](https://github.com/Divij-Bhoj)

---

