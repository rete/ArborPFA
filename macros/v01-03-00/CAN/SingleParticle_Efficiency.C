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
     {0.9143773913,
      0.948160708,
      0.9627783895,
      0.9646122456,
      0.9658003449,
      0.9635051489,
      0.9670493007,
      0.9712703228};

   // double pandoraDataError[] =
   //  {0.00510528,
   //   0.001520964,
   //   0.001855603,
   //   0.002933244,
   //   0.002174735,
   //   0.003213737,
   //   0.002105186,
   //   0.002026826};

   double arborData[] = 
     {0.975407,
      0.967591,
      0.962257,
      0.96736,
      0.962902,
      0.969566,
      0.970436,
      0.961919};

   double arborDataError[] =
     {0.004139075,
      0.001143327,
      0.001862598,
      0.002497137,
      0.002020139,
      0.002900492,
      0.00202452,
      0.001838915};

   std::string graphXTitle = "E_{beam} [GeV]";
   std::string graphYTitle = "Hit clustering efficiency #epsilon_{s}";
   double rangeUserXMin = 0;
   double rangeUserXMax = 90;
   double rangeUserYMin = 0.9;
   double rangeUserYMax = 1;

   gStyle->SetOptStat(0);

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

   TH1F *pH = new TH1F("pouette", "", rangeUserXMax-rangeUserXMin, rangeUserXMin, rangeUserXMax);
   pH->SetMinimum(rangeUserYMin);
   pH->SetMaximum(rangeUserYMax);
   pH->Draw();
   pH->GetXaxis()->SetTitle(graphXTitle.c_str());
   pH->GetXaxis()->SetLabelFont(42);
   pH->GetXaxis()->SetTitleSize(0.05);
   pH->GetXaxis()->SetTitleOffset(1.);
   pH->GetXaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetTitle(graphYTitle.c_str());
   pH->GetYaxis()->SetLabelFont(42);
   pH->GetYaxis()->SetTitleSize(0.045);
   pH->GetYaxis()->SetTitleOffset(1.3);
   pH->GetYaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetLabelSize(0.035);
   
   gre = new TGraphErrors(nPoints, energies, arborData, zero, arborDataError);
   gre->SetName("Graph_arbor");
   gre->SetTitle("ArborPFA - CERN SPS Aug 2012");
   gre->SetLineColor(kRed);
   gre->SetMarkerColor(kRed);
   gre->SetMarkerStyle(23);
   gre->SetMarkerSize(1.6);
   gre->Draw("p same");

   TPaveText *pt = new TPaveText(0.3, 0.2, 0.93, 0.3, "tbNDC");
   pt->SetTextSize(0.05);
   pt->SetTextColor(kGray+2);
   pt->SetFillColor(0);
   pt->SetLineWidth(0);
   pt->SetBorderSize(0);
   pt->AddText("CALICE SDHCAL Preliminary");
   pt->SetTextFont(62);
   pt->Draw();
   
   TLegend *leg = new TLegend(0.3,0.8,0.88,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(0);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetTextSize(0.04);

   TLegendEntry *entry=0;
   entry=leg->AddEntry("Graph_arbor","ArborPFA - CERN SPS Aug 2012","p");
   entry->SetFillStyle(1001);
   entry->SetMarkerColor(kRed);
   entry->SetMarkerStyle(23);
   entry->SetMarkerSize(1);

   leg->Draw();

   pCanvas->Modified();
   pCanvas->cd();
   pCanvas->SetSelected(pCanvas);
}
