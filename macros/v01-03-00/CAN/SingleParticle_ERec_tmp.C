{
{

   std::string canvasName = "canvas";
   std::string canvasTitle = "Canvas title";
   int canvasX1 = 200;
   int canvasY1 = 52;
   int canvasX2 = 700;
   int canvasY2 = 650;
   int nPoints = 8;

   double zero [] = {0, 0, 0, 0, 0, 0, 0, 0};
   double energies[] = {10, 20, 30, 40, 50, 60, 70, 80};

   double pandoraData[] = 
     {9.592139,
      19.44187,
      27.63257,
      37.85563,
      46.14601,
      60.03956,
      67.23956,
      72.81142};

   double pandoraDataError[] =
     {0.036946,
      0.0225806,
      0.04033875,
      0.08631291,
      0.07712404,
      0.1419706,
      0.1080929,
      0.1182856};

   double arborData[] = 
    {10.47579,
     20.44046,
     28.70774,
     38.997,
     47.47988,
     61.56612,
     68.76356,
     74.02058};

   double arborDataError[] =
     {0.03447006,
      0.01974367,
      0.04229435,
      0.07861879,
      0.07845369,
      0.1384805,
      0.1082608,
      0.113262};

   double noPFAData[] = 
    {10.5116,
     20.5701,
     29.0285,
     39.634,
     48.1973,
     62.1095,
     69.4983,
     74.7649};

   double noPFADataError[] = 
    {0.0357198,
     0.0193659,
     0.0394093,
     0.07411,
     0.0753174,
     0.11351,
     0.0980916,
     0.110341};

   std::string graphXTitle = "E_{beam} [GeV]";
   std::string graphYTitle = "E_{rec} GeV";
   double rangeUserXMin = 0;
   double rangeUserXMax = 90;
   double rangeUserYMin = 0;
   double rangeUserYMax = 90;

   TCanvas *pCanvas = new TCanvas(canvasName.c_str(), canvasTitle.c_str(), canvasX1, canvasY1, canvasX2, canvasY2);
   pCanvas->SetFillColor(0);
   pCanvas->SetBorderMode(0);
   pCanvas->SetBorderSize(2);
   pCanvas->SetTickx(1);
   pCanvas->SetTicky(1);
   pCanvas->SetLeftMargin(0.15);
   pCanvas->SetRightMargin(0.03);
   pCanvas->SetTopMargin(0.05);
   pCanvas->SetBottomMargin(0.14);
   pCanvas->SetFrameBorderMode(0);
   pCanvas->SetFrameBorderMode(0);
   
   TMultiGraph *multigraph = new TMultiGraph();
   
   TGraphErrors *gre = new TGraphErrors(nPoints, energies, pandoraData, zero, pandoraDataError);
   gre->SetName("Graph_pandora");
   gre->SetTitle("PandoraPFA - CERN SPS Aug 2012");
   gre->SetLineStyle(7);
   gre->SetLineWidth(2);
   gre->SetLineColor(kBlack);
   gre->SetMarkerColor(kBlack);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.2);
   multigraph->Add(gre,"");
   
   gre = new TGraphErrors(nPoints, energies, arborData, zero, arborDataError);
   gre->SetName("Graph_arbor");
   gre->SetTitle("ArborPFA - CERN SPS Aug 2012");
   gre->SetLineColor(kRed);
   gre->SetMarkerColor(kRed);
   gre->SetMarkerStyle(23);
   gre->SetMarkerSize(1.2);
   multigraph->Add(gre,"");

   gre = new TGraphErrors(nPoints, energies, noPFAData, zero, noPFADataError);
   gre->SetName("Graph_nopfa");
   gre->SetTitle("No PFA - CERN SPS Aug 2012");
   gre->SetLineColor(kBlue);
   gre->SetMarkerColor(kBlue);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(1.2);
   multigraph->Add(gre,"");

   multigraph->Draw("ap");
   multigraph->GetXaxis()->SetTitle(graphXTitle.c_str());
   multigraph->GetXaxis()->SetLabelFont(42);
   multigraph->GetXaxis()->SetTitleSize(0.035);
   multigraph->GetXaxis()->SetTitleOffset(1.5);
   multigraph->GetXaxis()->SetTitleFont(42);
   multigraph->GetXaxis()->SetRangeUser(rangeUserXMin, rangeUserXMax);
   multigraph->GetYaxis()->SetTitle(graphYTitle.c_str());
   multigraph->GetYaxis()->SetLabelFont(42);
   multigraph->GetYaxis()->SetTitleSize(0.035);
   multigraph->GetYaxis()->SetTitleOffset(1.5);
   multigraph->GetYaxis()->SetTitleFont(42);
   multigraph->GetYaxis()->SetRangeUser(rangeUserYMin, rangeUserYMax);

   TPaveText *pt = new TPaveText(0.55, 0.2, 0.93, 0.3, "tbNDC");
   pt->SetTextSize(0.05);
   pt->SetTextColor(kGray+2);
   pt->SetFillColor(0);
   pt->SetLineWidth(0);
   pt->SetBorderSize(0);
   pt->AddText("CALICE Preliminary");
   pt->SetTextFont(62);
   pt->Draw();
   
   TLegend *leg = new TLegend(0.3,0.67,0.88,0.8,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(0);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetTextSize(0.03);

   TLegendEntry *entry=leg->AddEntry("Graph_pandora","PandoraPFA - CERN SPS Aug 2012", "p");
   entry->SetFillStyle(1001);
   entry->SetLineColor(1);
   entry->SetLineStyle(7);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);

   entry=leg->AddEntry("Graph_arbor","ArborPFA - CERN SPS Aug 2012","p");
   entry->SetFillStyle(1001);
   entry->SetMarkerColor(kRed);
   entry->SetMarkerStyle(23);
   entry->SetMarkerSize(1);

   entry=leg->AddEntry("Graph_nopfa","No PFA - CERN SPS Aug 2012","p");
   entry->SetFillStyle(1001);
   entry->SetMarkerColor(kBlue);
   entry->SetMarkerStyle(24);
   entry->SetMarkerSize(1);

   leg->Draw();

   pCanvas->Modified();
   pCanvas->cd();
   pCanvas->SetSelected(pCanvas);
}




//=========Macro generated from canvas: cc3/Mean charged energy
//=========  (Tue Feb 17 12:20:47 2015) by ROOT version5.34/05
   TCanvas *cc3 = new TCanvas("cc3", "Mean charged energy",11,158,750,691);
   cc3->Range(-8.637073,-15.55556,87.14341,95.55556);
   cc3->SetFillColor(0);
   cc3->SetBorderMode(0);
   cc3->SetBorderSize(2);
   cc3->SetTickx(1);
   cc3->SetTicky(1);
   cc3->SetLeftMargin(0.15);
   cc3->SetRightMargin(0.03);
   cc3->SetTopMargin(0.05);
   cc3->SetBottomMargin(0.14);
   cc3->SetFrameBorderMode(0);
   cc3->SetFrameBorderMode(0);
   
   TMultiGraph *multigraph = new TMultiGraph();
   multigraph->SetName("MeanChargedEnergyMultiGraph");
   multigraph->SetTitle("Reconstructed Energy [GeV]");
   
   TGraphErrors *gre = new TGraphErrors(8);
   gre->SetName("Graph");
   gre->SetTitle("PandoraPFA - Test Beam");
   gre->SetLineWidth(2);
   gre->SetMarkerStyle(21);
   gre->SetPoint(0,10,9.592139);
   gre->SetPointError(0,0,0.036946);
   gre->SetPoint(1,20,19.44187);
   gre->SetPointError(1,0,0.0225806);
   gre->SetPoint(2,30,27.63257);
   gre->SetPointError(2,0,0.04033875);
   gre->SetPoint(3,40,37.85563);
   gre->SetPointError(3,0,0.08631291);
   gre->SetPoint(4,50,46.14601);
   gre->SetPointError(4,0,0.07712404);
   gre->SetPoint(5,60,60.03956);
   gre->SetPointError(5,0,0.1419706);
   gre->SetPoint(6,70,67.23956);
   gre->SetPointError(6,0,0.1080929);
   gre->SetPoint(7,80,72.81142);
   gre->SetPointError(7,0,0.1182856);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","PandoraPFA - Test Beam",100,3,87);
   Graph_Graph1->SetMinimum(3.217741);
   Graph_Graph1->SetMaximum(79.26716);
   Graph_Graph1->SetDirectory(0);
   Graph_Graph1->SetStats(0);
   Graph_Graph1->GetXaxis()->SetLabelFont(42);
   Graph_Graph1->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetXaxis()->SetTitleOffset(1.5);
   Graph_Graph1->GetXaxis()->SetTitleFont(42);
   Graph_Graph1->GetYaxis()->SetLabelFont(42);
   Graph_Graph1->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetYaxis()->SetTitleOffset(1.5);
   Graph_Graph1->GetYaxis()->SetTitleFont(42);
   Graph_Graph1->GetZaxis()->SetLabelFont(42);
   Graph_Graph1->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph1);
   
   multigraph->Add(gre,"");
   
   gre = new TGraphErrors(8);
   gre->SetName("Graph");
   gre->SetTitle("ArborPFA - Test Beam");

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#ff0000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(2);

   ci = TColor::GetColor("#ff0000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(23);
   gre->SetPoint(0,10,10.47579);
   gre->SetPointError(0,0,0.03447006);
   gre->SetPoint(1,20,20.44046);
   gre->SetPointError(1,0,0.01974367);
   gre->SetPoint(2,30,28.70774);
   gre->SetPointError(2,0,0.04229435);
   gre->SetPoint(3,40,38.997);
   gre->SetPointError(3,0,0.07861879);
   gre->SetPoint(4,50,47.47988);
   gre->SetPointError(4,0,0.07845369);
   gre->SetPoint(5,60,61.56612);
   gre->SetPointError(5,0,0.1384805);
   gre->SetPoint(6,70,68.76356);
   gre->SetPointError(6,0,0.1082608);
   gre->SetPoint(7,80,74.02058);
   gre->SetPointError(7,0,0.113262);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","ArborPFA - Test Beam",100,3,87);
   Graph_Graph2->SetMinimum(4.072065);
   Graph_Graph2->SetMaximum(80.5031);
   Graph_Graph2->SetDirectory(0);
   Graph_Graph2->SetStats(0);
   Graph_Graph2->GetXaxis()->SetLabelFont(42);
   Graph_Graph2->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetXaxis()->SetTitleOffset(1.5);
   Graph_Graph2->GetXaxis()->SetTitleFont(42);
   Graph_Graph2->GetYaxis()->SetLabelFont(42);
   Graph_Graph2->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetYaxis()->SetTitleOffset(1.5);
   Graph_Graph2->GetYaxis()->SetTitleFont(42);
   Graph_Graph2->GetZaxis()->SetLabelFont(42);
   Graph_Graph2->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph2);
   
   multigraph->Add(gre,"");
   multigraph->Draw("ap");
   multigraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
   multigraph->GetXaxis()->SetRange(0,101);
   multigraph->GetXaxis()->SetLabelFont(42);
   multigraph->GetXaxis()->SetTitleSize(0.035);
   multigraph->GetXaxis()->SetTitleOffset(1.5);
   multigraph->GetXaxis()->SetTitleFont(42);
   multigraph->GetYaxis()->SetTitle("Reconstructed energy");
   multigraph->GetYaxis()->SetLabelFont(42);
   multigraph->GetYaxis()->SetTitleSize(0.035);
   multigraph->GetYaxis()->SetTitleOffset(1.5);
   multigraph->GetYaxis()->SetTitleFont(42);
   
   TPaveText *pt = new TPaveText(0.01,0.9390678,0.6075194,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(1);
   pt->SetFillColor(0);
   TText *text = pt->AddText("Reconstructed Energy [GeV]");
   pt->Draw();
   
   TLegend *leg = new TLegend(0.5,0.67,0.88,0.88,NULL,"brNDC");
   leg->SetBorderSize(1);
   leg->SetLineColor(0);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   TLegendEntry *entry=leg->AddEntry("Graph","PandoraPFA - Test Beam","lpf");
   entry->SetFillStyle(1001);
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry=leg->AddEntry("Graph","ArborPFA - Test Beam","lpf");
   entry->SetFillStyle(1001);

   ci = TColor::GetColor("#ff0000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);

   ci = TColor::GetColor("#ff0000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(23);
   entry->SetMarkerSize(1);
   leg->Draw();
   
   TF1 *lin = new TF1("lin","x",0,90);
   lin->SetFillColor(19);
   lin->SetFillStyle(0);

   ci = TColor::GetColor("#cccccc");
   lin->SetLineColor(ci);
   lin->SetLineWidth(2);
   lin->GetXaxis()->SetLabelFont(42);
   lin->GetXaxis()->SetTitleSize(0.035);
   lin->GetXaxis()->SetTitleOffset(1.5);
   lin->GetXaxis()->SetTitleFont(42);
   lin->GetYaxis()->SetLabelFont(42);
   lin->GetYaxis()->SetTitleSize(0.035);
   lin->GetYaxis()->SetTitleOffset(1.5);
   lin->GetYaxis()->SetTitleFont(42);
   lin->Draw("same");
   cc3->Modified();
   cc3->cd();
   cc3->SetSelected(cc3);
}
