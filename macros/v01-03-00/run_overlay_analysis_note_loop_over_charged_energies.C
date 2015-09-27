void run_overlay_analysis_note_loop_over_charged_energies()
{
	std::vector<std::string> energies;
	energies.push_back("10");
	energies.push_back("20");
	energies.push_back("30");
	energies.push_back("40");
	energies.push_back("50");

	gROOT->ProcessLine(".L OverlayPurityAnalysis.C++");
	gROOT->ProcessLine(".L process_overlay_analysis_note.C++");

	for(unsigned int e=0 ; e<energies.size() ; e++)
	{
		std::string line = "processAlgorithmOverlayAnalysis(" + energies.at(e) + ")";
		gROOT->ProcessLine(line.c_str());
	}

}
