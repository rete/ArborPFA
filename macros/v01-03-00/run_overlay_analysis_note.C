
void run_overlay_analysis_note()
{
	gROOT->ProcessLine(".L OverlayPurityAnalysis.C++");
	gROOT->ProcessLine(".L process_overlay_analysis_note.C++");
	gROOT->ProcessLine("processAlgorithmOverlayAnalysis(10)");
}
