{
//=========Macro generated from canvas: c1_n40/c1_n40
//=========  (Fri Sep 25 14:39:03 2015) by ROOT version5.34/05
   TCanvas *c1_n40 = new TCanvas("c1_n40", "c1_n40",1367,28,700,500);
   c1_n40->Range(-30.97561,-400.8926,42.19512,2462.626);
   c1_n40->SetFillColor(0);
   c1_n40->SetBorderMode(0);
   c1_n40->SetBorderSize(2);
   c1_n40->SetTickx(1);
   c1_n40->SetTicky(1);
   c1_n40->SetLeftMargin(0.15);
   c1_n40->SetRightMargin(0.03);
   c1_n40->SetTopMargin(0.05);
   c1_n40->SetBottomMargin(0.14);
   c1_n40->SetFrameBorderMode(0);
   c1_n40->SetFrameBorderMode(0);

   gStyle->SetOptStat(0);
   
   TH1F *histo = new TH1F("histo","",51,-10,40);
   histo->SetBinContent(-10+23,5);
   histo->SetBinContent(-10+24,14);
   histo->SetBinContent(-10+25,53);
   histo->SetBinContent(-10+26,147);
   histo->SetBinContent(-10+27,436);
   histo->SetBinContent(-10+28,1084);
   histo->SetBinContent(-10+29,2068);
   histo->SetBinContent(-10+30,2898);
   histo->SetBinContent(-10+31,3521);
   histo->SetBinContent(-10+32,3265);
   histo->SetBinContent(-10+33,2525);
   histo->SetBinContent(-10+34,1496);
   histo->SetBinContent(-10+35,825);
   histo->SetBinContent(-10+36,379);
   histo->SetBinContent(-10+37,123);
   histo->SetBinContent(-10+38,46);
   histo->SetBinContent(-10+39,22);
   histo->SetBinContent(-10+40,6);
   histo->SetBinContent(-10+41,2);
   histo->SetBinContent(-10+43,1);
   histo->SetBinContent(-10+45,1);
   histo->SetBinContent(-10+52,1);
   histo->SetEntries(18918);
   histo->SetDirectory(0);
   histo->SetLineColor(kBlack);
   histo->SetLineWidth(1);
   histo->GetXaxis()->SetLabelFont(42);
   histo->GetXaxis()->SetLabelSize(0.045);
   histo->GetXaxis()->SetTitleSize(0.035);
   histo->GetXaxis()->SetTitleOffset(1.5);
   histo->GetXaxis()->SetTitleFont(42);
   histo->GetYaxis()->SetLabelFont(42);
   histo->GetYaxis()->SetLabelSize(0.045);
   histo->GetYaxis()->SetTitleSize(0.035);
   histo->GetYaxis()->SetTitleOffset(1.5);
   histo->GetYaxis()->SetTitleFont(42);
   histo->GetXaxis()->SetTitle("E_{rec} [GeV]");
   histo->GetXaxis()->SetTitleSize(0.06);
   histo->GetXaxis()->SetTitleOffset(1.1);
   histo->Draw("");

   TPaveText *stats = new TPaveText(0.68,0.635,0.88,0.8,"nbNDC");
   stats->SetName("stats1");
   stats->SetBorderSize(0);
   stats->SetFillColor(0);
   stats->SetLineColor(kBlack);
   stats->SetTextAlign(12);
   stats->SetTextFont(42);
   stats->SetTextColor(kBlack);
   stats->SetTextSize(0.05);
   stats->AddText("   No PFA      ");
   stats->AddText("Mean    = 10.3 ");
   stats->AddText("RMS     = 2.113");
   stats->Draw();

   TPaveText *pt = new TPaveText(0.3, 0.2, 0.93, 0.3, "tbNDC");
   pt->SetTextSize(0.05);
   pt->SetTextColor(kGray+2);
   pt->SetFillColor(0);
   pt->SetLineWidth(0);
   pt->SetBorderSize(0);
   pt->AddText("CALICE SDHCAL Preliminary");
   pt->SetTextFont(62);
   pt->Draw();

   c1_n40->Modified();
   c1_n40->cd();
   c1_n40->SetSelected(c1_n40);
}
