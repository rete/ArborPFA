{
   std::string canvasName = "canvas";
   std::string canvasTitle = "Canvas title";
   int canvasX1 = 14;
   int canvasY1 = 48;
   int canvasX2 = 600;
   int canvasY2 = 741;
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
   std::string graphYTitle = "E_{rec} [GeV]";
   double rangeUserXMin = 0;
   double rangeUserXMax = 90;
   double rangeUserYMin = 0;
   double rangeUserYMax = 90;

   gStyle->SetOptStat(0);

   TCanvas *pCanvas = new TCanvas(canvasName.c_str(), canvasTitle.c_str(), canvasX1, canvasY1, canvasX2, canvasY2);
   pCanvas->Range(0,0,1,1);
   pCanvas->SetFillColor(0);
   pCanvas->SetBorderMode(0);
   pCanvas->SetBorderSize(2);
   pCanvas->SetTickx(1);
   pCanvas->SetTicky(1);
   pCanvas->SetLeftMargin(0.1666667);
   pCanvas->SetRightMargin(0.05660377);
   pCanvas->SetTopMargin(0.0256917);
   pCanvas->SetBottomMargin(0.07692308);
   pCanvas->SetFrameLineWidth(1);

   // pCanvas->SetFillColor(0);
   // pCanvas->SetBorderMode(0);
   // pCanvas->SetBorderSize(2);
   // pCanvas->SetTickx(1);
   // pCanvas->SetTicky(1);
   // pCanvas->SetLeftMargin(0.15);
   // pCanvas->SetRightMargin(0.03);
   // pCanvas->SetTopMargin(0.05);
   // pCanvas->SetBottomMargin(0.14);
   // pCanvas->SetFrameBorderMode(0);
   // pCanvas->SetFrameBorderMode(0);

   TPad *r1_1 = new TPad("r1_1", "",0.02,0.37,0.95,0.99);
   r1_1->Draw();
   r1_1->cd();
   r1_1->Range(-19.23077,0.01,96.15385,1428.571);
   r1_1->SetFillColor(0);
   r1_1->SetBorderMode(0);
   r1_1->SetBorderSize(2);
   r1_1->SetTickx(1);
   r1_1->SetTicky(1);
   r1_1->SetLeftMargin(0.1666667);
   r1_1->SetRightMargin(0.01);
   r1_1->SetTopMargin(0.02);
   r1_1->SetBottomMargin(0.0);
   r1_1->SetFrameLineWidth(2);
   r1_1->SetFrameBorderMode(0);
   r1_1->SetFrameLineWidth(1);
   r1_1->SetFrameBorderMode(0);

   TH1F *pH = new TH1F("pouette", "", rangeUserXMax-rangeUserXMin, rangeUserXMin, rangeUserXMax);
   pH->SetMinimum(0.1);
   pH->SetMaximum(90);
   pH->Draw();
   // pH->GetXaxis()->SetTitle(graphXTitle.c_str());
   // pH->GetXaxis()->SetLabelFont(42);
   // pH->GetXaxis()->SetTitleSize(0.035);
   // pH->GetXaxis()->SetTitleOffset(1.5);
   // pH->GetXaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetTitle(graphYTitle.c_str());
   pH->GetYaxis()->SetLabelFont(42);
   pH->GetYaxis()->SetTitleSize(0.05);
   pH->GetYaxis()->SetTitleOffset(1.5);
   pH->GetYaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetLabelSize(0.045);
   
   TGraphErrors *gre = new TGraphErrors(nPoints, energies, pandoraData, zero, pandoraDataError);
   gre->SetName("Graph_pandora");
   gre->SetTitle("PandoraPFA - CERN SPS Aug 2012");
   gre->SetLineStyle(7);
   gre->SetLineWidth(2);
   gre->SetLineColor(kBlack);
   gre->SetMarkerColor(kBlack);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.2);
   gre->Draw("p same");
   //   multigraph->Add(gre,"");
   
   gre = new TGraphErrors(nPoints, energies, arborData, zero, arborDataError);
   gre->SetName("Graph_arbor");
   gre->SetTitle("ArborPFA - CERN SPS Aug 2012");
   gre->SetLineColor(kRed);
   gre->SetMarkerColor(kRed);
   gre->SetMarkerStyle(23);
   gre->SetMarkerSize(1.2);
   gre->Draw("p same");
   //   multigraph->Add(gre,"");

   gre = new TGraphErrors(nPoints, energies, noPFAData, zero, noPFADataError);
   gre->SetName("Graph_nopfa");
   gre->SetTitle("No PFA - CERN SPS Aug 2012");
   gre->SetLineColor(kBlue);
   gre->SetMarkerColor(kBlue);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(1.2);
   gre->Draw("p same");

   TF1 *pLin = new TF1("lin", "x", 0, 90);
   pLin->SetLineColor(kGray);
   pLin->SetLineWidth(1);
   pLin->Draw("same");

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
   leg->SetFillStyle(0);
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

   r1_1->Modified();
   pCanvas->cd();

   TPad *r1_2 = new TPad("r1_2", "Energy_2",0.02,0,0.95,0.37);
   r1_2->Draw();
   r1_2->cd();
   r1_2->Range(-19.23077,-0.3993507,96.15385,0.25);
   r1_2->SetFillColor(0);
   r1_2->SetBorderMode(0);
   r1_2->SetBorderSize(2);
   r1_2->SetTickx(1);
   r1_2->SetTicky(1);
   r1_2->SetLeftMargin(0.1666667);
   r1_2->SetRightMargin(0.01);
   r1_2->SetTopMargin(0);
   r1_2->SetBottomMargin(0.23);
   r1_2->SetFrameLineWidth(1);
   r1_2->SetFrameBorderMode(0);

   TH1F *pH = new TH1F("pouette2", "", rangeUserXMax-rangeUserXMin, rangeUserXMin, rangeUserXMax);
   pH->SetMinimum(-0.12);
   pH->SetMaximum(0.12);
   pH->Draw();
   pH->GetXaxis()->SetTitle(graphXTitle.c_str());
   pH->GetXaxis()->SetLabelFont(42);
   pH->GetXaxis()->SetTitleSize(0.065);
   pH->GetXaxis()->SetTitleOffset(1);
   pH->GetXaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetTitle("#DeltaE/E_{beam}");
   pH->GetYaxis()->SetLabelFont(42);
   pH->GetYaxis()->SetTitleSize(0.12);
   pH->GetYaxis()->SetTitleOffset(0.5);
   pH->GetYaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetLabelSize(0.07);

   gre = new TGraphErrors(nPoints);//, energies, pandoraData, zero, pandoraDataError);
   gre->SetName("Graph_pandoraDev");
   gre->SetTitle("PandoraPFA - CERN SPS Aug 2012");
   gre->SetLineStyle(7);
   gre->SetLineWidth(2);
   gre->SetLineColor(kBlack);
   gre->SetMarkerColor(kBlack);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.2);

   for(int i=0 ; i<nPoints ; i++)
   {
     gre->SetPoint(i, energies[i], (pandoraData[i]-energies[i]) / energies[i]);
     gre->SetPointError(i, 0, pandoraDataError[i]/energies[i]);
   }

   gre->Draw("p same");
   //   multigraph->Add(gre,"");
   
   gre = new TGraphErrors(nPoints);//, energies, arborData, zero, arborDataError);
   gre->SetName("Graph_arborDev");
   gre->SetTitle("ArborPFA - CERN SPS Aug 2012");
   gre->SetLineColor(kRed);
   gre->SetMarkerColor(kRed);
   gre->SetMarkerStyle(23);
   gre->SetMarkerSize(1.2);

   for(int i=0 ; i<nPoints ; i++)
   {
     gre->SetPoint(i, energies[i], (arborData[i]-energies[i]) / energies[i]);
     gre->SetPointError(i, 0, arborDataError[i]/energies[i]);
   }

   gre->Draw("p same");
   //   multigraph->Add(gre,"");

   gre = new TGraphErrors(nPoints);//, energies, noPFAData, zero, noPFADataError);
   gre->SetName("Graph_nopfaDev");
   gre->SetTitle("No PFA - CERN SPS Aug 2012");
   gre->SetLineColor(kBlue);
   gre->SetMarkerColor(kBlue);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(1.2);
 
   for(int i=0 ; i<nPoints ; i++)
   {
     gre->SetPoint(i, energies[i], (noPFAData[i]-energies[i]) / energies[i]);
     gre->SetPointError(i, 0, noPFADataError[i]/energies[i]);
   }
   
   gre->Draw("p same");


   pLin = new TF1("lin0", "0", 0, 90);
   pLin->SetLineColor(kGray);
   pLin->SetLineWidth(1);
   pLin->Draw("same");


   r1_2->Modified();
   pCanvas->cd();
   //pCanvas->SetSelected(pCanvas);

   pCanvas->Modified();
   pCanvas->cd();
   pCanvas->SetSelected(pCanvas);
}
