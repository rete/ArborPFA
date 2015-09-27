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

   double arborData[] = 
    {0.2119042,
     0.1627236,
     0.1486449,
     0.1383596,
     0.1307226,
     0.1152201,
     0.1063646,
     0.1010931};

   double arborDataError[] =
     {0.004139075,
      0.001143327,
      0.001862598,
      0.002497137,
      0.002020139,
      0.002900492,
      0.00202452,
      0.001838915};

   std::string graphXTitle = "Graph x Title";
   std::string graphYTitle = "Graph y Title";
   double rangeUserXMin = 0;
   double rangeUserXMax = 90;
   double rangeUserYMin = 0;
   double rangeUserYMax = 0.4;

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
   
   TGraphErrors *gre = 0;
   gre = new TGraphErrors(nPoints, energies, arborData, zero, arborDataError);
   gre->SetName("Graph_arbor");
   gre->SetTitle("ArborPFA - CERN SPS Aug 2012");
   gre->SetLineColor(kRed);
   gre->SetMarkerColor(kRed);
   gre->SetMarkerStyle(23);
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

   TLegendEntry *entry = 0;
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
