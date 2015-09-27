{
//=========Macro generated from canvas: c1/c1
//=========  (Fri May 22 12:27:30 2015) by ROOT version5.34/05
   TCanvas *c1 = new TCanvas("c1", "c1",1380,521,700,500);
   c1->Range(-30.97561,-397.9889,42.19512,2444.789);
   c1->SetFillColor(0);
   c1->SetBorderMode(0);
   c1->SetBorderSize(2);
   c1->SetTickx(1);
   c1->SetTicky(1);
   c1->SetLeftMargin(0.15);
   c1->SetRightMargin(0.03);
   c1->SetTopMargin(0.05);
   c1->SetBottomMargin(0.14);
   c1->SetFrameBorderMode(0);
   c1->SetFrameBorderMode(0);

   gStyle->SetOptStat(0);
   
   TH1F *histo = new TH1F("histo_neutral_mcenergy_ArborPFA_Test Beam_5cm__2","",61,-20,40);
   histo->SetBinContent(23,11);
   histo->SetBinContent(24,57);
   histo->SetBinContent(25,144);
   histo->SetBinContent(26,300);
   histo->SetBinContent(27,664);
   histo->SetBinContent(28,1182);
   histo->SetBinContent(29,1854);
   histo->SetBinContent(30,2109);
   histo->SetBinContent(31,2193);
   histo->SetBinContent(32,1768);
   histo->SetBinContent(33,1229);
   histo->SetBinContent(34,647);
   histo->SetBinContent(35,354);
   histo->SetBinContent(36,145);
   histo->SetBinContent(37,53);
   histo->SetBinContent(38,20);
   histo->SetBinContent(39,5);
   histo->SetBinContent(40,5);
   histo->SetBinContent(41,1);
   histo->SetBinContent(44,1);
   histo->SetBinContent(49,1);
   histo->SetBinContent(50,1);
   histo->SetEntries(12744);
   histo->SetDirectory(0);
   histo->SetLineColor(kBlack);
   histo->SetLineWidth(2);
   histo->GetXaxis()->SetLabelFont(42);
   histo->GetXaxis()->SetTitleSize(0.035);
   histo->GetXaxis()->SetTitleOffset(1.5);
   histo->GetXaxis()->SetTitleFont(42);
   histo->GetYaxis()->SetLabelFont(42);
   histo->GetYaxis()->SetTitleSize(0.035);
   histo->GetYaxis()->SetTitleOffset(1.5);
   histo->GetYaxis()->SetTitleFont(42);
   histo->GetXaxis()->SetTitle("E_{rec} [GeV]");
   histo->GetXaxis()->SetTitleSize(0.05);
   histo->GetXaxis()->SetTitleOffset(1.1);
   histo->Draw("");

   TPaveText *stats = new TPaveText(0.68,0.635,0.88,0.8,"nbNDC");
   stats->SetName("stats");
   stats->SetBorderSize(0);
   stats->SetFillColor(0);
   stats->SetTextAlign(12);
   stats->SetTextFont(42);
   stats->AddText("Entries = 12744");
   stats->AddText("Mean    = 9.544");
   stats->AddText("RMS     = 2.301");
   stats->Draw();
   
   c1->Modified();
   c1->cd();
   c1->SetSelected(c1);

   c1->SaveAs("histo_neutral_mcenergy_ArborPFA_TestBeam_10GeV_n_5GeV_ch_5_cm.pdf");
}
