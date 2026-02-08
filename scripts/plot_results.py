#!/usr/bin/env python3
"""
Plot analysis results from ROOT file
Requires: ROOT with Python bindings (PyROOT)
"""

import ROOT
import sys

def plot_histogram(file, hist_name, output_name, fit=False):
    """Extract and plot a histogram from ROOT file"""
    
    hist = file.Get(hist_name)
    if not hist:
        print(f"Warning: Histogram '{hist_name}' not found")
        return
    
    # Create canvas
    canvas = ROOT.TCanvas("c1", "Analysis Results", 800, 600)
    canvas.SetGrid()
    
    # Style
    hist.SetLineColor(ROOT.kBlue+1)
    hist.SetLineWidth(2)
    hist.SetFillColorAlpha(ROOT.kBlue-9, 0.3)
    
    # Draw
    hist.Draw("HIST")
    
    # Optional fit
    if fit:
        fit_result = hist.Fit("gaus", "S", "", 85, 97)
        if fit_result.IsValid():
            fit_func = hist.GetFunction("gaus")
            fit_func.SetLineColor(ROOT.kRed+1)
            fit_func.SetLineWidth(2)
    
    # Save
    canvas.SaveAs(f"results/{output_name}.png")
    canvas.SaveAs(f"results/{output_name}.pdf")
    print(f"Saved: results/{output_name}.png")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot_results.py <analysis_output.root>")
        sys.exit(1)
    
    # Open ROOT file
    filename = sys.argv[1]
    file = ROOT.TFile.Open(filename)
    if not file or file.IsZombie():
        print(f"Error: Cannot open {filename}")
        sys.exit(1)
    
    print(f"Plotting results from {filename}")
    
    # Set ROOT batch mode (no GUI)
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(1111)
    ROOT.gStyle.SetOptFit(1111)
    
    # Z analysis plots
    plot_histogram(file, "z_mass", "z_mass_spectrum", fit=True)
    plot_histogram(file, "z_pt", "z_pt_distribution")
    plot_histogram(file, "muon_pt", "muon_pt_distribution")
    
    # Top analysis plots
    plot_histogram(file, "top_hadronic_mass", "top_mass_spectrum")
    plot_histogram(file, "w_hadronic_mass", "w_mass_spectrum")
    plot_histogram(file, "n_jets", "jet_multiplicity")
    
    print("\nPlotting complete!")
    file.Close()

if __name__ == "__main__":
    main()
