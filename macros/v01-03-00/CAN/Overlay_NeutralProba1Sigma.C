

void readData(const std::string &fileName, double *data, int nData)
{
  std::ifstream f;
  f.open(fileName.c_str());

  if(!f.is_open())
    {
      std::cout << "pas de fichier : " << fileName << std::endl;
      throw;
    }

  int i = 0;
  while(1)
    {
      if(!f.good())
	break;

      if(i == nData)
	break;

      data[i] = 0.;
      f >> data[i];
      i++;
    }

  if(i != nData)
    throw;
}

void Overlay_NeutralProba1Sigma()
{
   std::string canvasName = "canvas";
   std::string canvasTitle = "Neutral Efficiency";
   int canvasX1 = 200;
   int canvasY1 = 52;
   int canvasX2 = 700;
   int canvasY2 = 650;
   int nPoints = 6;
   int nGraphs = 5;
   int neutralEnergy = 10;

   double zero [] = {0, 0, 0, 0, 0, 0, 0, 0};
   double energies[] = {10, 20, 30, 40, 50};
   double distances[] = {5, 10, 15, 20, 25, 30};

   std::string graphXTitle = "Distance between showers [cm]";
   std::string graphYTitle = "1 #sigma event fraction of E_{n}";
   double rangeUserXMin = 0;
   double rangeUserXMax = 35;
   double rangeUserYMin = 0.001;
   double rangeUserYMax = 1;

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
   gStyle->SetOptStat(0);
   pH->GetYaxis()->SetTitle(graphYTitle.c_str());
   pH->GetXaxis()->SetTitle(graphXTitle.c_str());
   pH->GetYaxis()->SetLabelFont(42);
   pH->GetYaxis()->SetTitleSize(0.05);
   pH->GetYaxis()->SetTitleOffset(1);
   pH->GetYaxis()->SetTitleFont(42);
   pH->GetYaxis()->SetLabelSize(0.045);

   TLegend *leg = new TLegend(0.3,0.55,0.8,0.8,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(0);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetTextSize(0.03);

   int currentColor = 1;

   for(unsigned int e=0 ; e<nGraphs ; e++)
   {
     int energy = energies[e];
  
     double *data = new double[nPoints];
     double *dataError = zero;//new double[nPoints];

     std::stringstream fileName;
     fileName << "data_neutralProba1Sigma_ArborPFA_Test\ Beam_" << energy << "Gev.txt";
     readData(fileName.str(), data, nPoints);
     
     // fileName.str("");
     // fileName << "fichier_data_error_energy_" << energy << "GeV.txt";
     // readData(fileName.str(), dataError, nPoints);
   
     TGraphErrors *gre = 0;
     std::stringstream graphName;
     graphName << "Charged particle energy = " << energy << " GeV";
     gre = new TGraphErrors(nPoints, distances, data, zero, dataError);
     gre->SetName(graphName.str().c_str());
     gre->SetTitle(graphName.str().c_str());
     gre->SetLineColor(currentColor);
     gre->SetMarkerColor(currentColor);
     gre->SetMarkerStyle(23);
     gre->SetMarkerSize(1.2);
     gre->Draw("lp same");
   
     TLegendEntry *entry = 0;
     entry=leg->AddEntry(gre, graphName.str().c_str(), "lp");
     entry->SetFillStyle(1001);
     entry->SetMarkerColor(currentColor);
     entry->SetMarkerStyle(23);
     entry->SetMarkerSize(1);

     currentColor++;
   }

   TPaveText *pt = new TPaveText(0.55, 0.2, 0.93, 0.3, "tbNDC");
   pt->SetTextSize(0.05);
   pt->SetTextColor(kGray+2);
   pt->SetFillColor(0);
   pt->SetLineWidth(0);
   pt->SetBorderSize(0);
   pt->AddText("CALICE Preliminary");
   pt->SetTextFont(62);
   pt->Draw();
   
   leg->Draw();

   pCanvas->Modified();
   pCanvas->cd();
   pCanvas->SetSelected(pCanvas);
}
