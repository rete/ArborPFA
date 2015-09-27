{
	gROOT->ProcessLine(".L SinglePurityAnalysis.C++");
	gROOT->ProcessLine(".L process_single_purity_analysis.C++");
	gROOT->ProcessLine("process_single_purity_analysis()");
}
