/////////////////////////////////////////////////////////////////////////
//
// A simple spin analysis study (TFG Ana Belen)
// 
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "TCanvas.h"
#include "TRandom3.h"
#include "TH1F.h"
#include "TF1.h"

// Declaraci?n de funciones para obtener el p-value en el caso de ajustar o no las distribuciones de probabilidad.
void get_pvalue_fit(TF1 *pdf, TH1F *data, double& chi2, int& ndof, double& pvalue, double& beta);
void get_pvalue_nofit(TF1 *pdf, TH1F *data, double& chi2, int& ndof, double& pvalue);

// Funci?n principal (crea los pseudoexperimentos y representa el resultado en un gr?fico junto con las distribuciones de probabilidad de cada esp?n. Devuelve el p-value para cada esp?n)
void AnalysisOmega(Int_t numExps=10, Int_t numEvts=770, Bool_t doFit=kTRUE, Bool_t doFitBeta=kTRUE, Int_t seed=1)
{
  assert(numExps>0 && numEvts>0);
  TRandom3 random(seed);
  printf("Generating %u experiments with %u events/experiment \n",numExps,numEvts);
  
  // Definici?n de PDFs con beta (normalizadas a 1)
  TF1 *pdf1 = new TF1("pdf1","[0]*0.5*(1.+[1]*x)",-1,1); 
  TF1 *pdf2 = new TF1("pdf2","[0]*0.25*(1.+3.*pow(x,2)+[1]*x*(5.-9.*pow(x,2)))",-1,1);
  TF1 *pdf3 = new TF1("pdf3","[0]*3./8.*(1.-2.*pow(x,2)+5.*pow(x,4)+[1]*x*(5.-26.*pow(x,2)+25.*pow(x,4)))",-1,1);

  TH1F *data = new TH1F("data","data",10,-1,1);  // Define la anchura del bin para los datos generados aleatoriamente
  double binwidth = 0.2;
  TString binwidth_str="0.2";
    
  // Generaci?n aleatoria de valores
  double chi2_pdf1_mean(0.),chi2_pdf2_mean(0.),chi2_pdf3_mean(0.);
  int ndof_pdf1,ndof_pdf2,ndof_pdf3;
  vector<double> beta_pdf1_list,beta_pdf2_list,beta_pdf3_list;
  double beta_pdf1_mean(0.),beta_pdf2_mean(0.),beta_pdf3_mean(0.);
  
  for (Int_t iExp=0;iExp<numExps;iExp++) {
    
    printf("Experiment %u \n",iExp);

    // clean-up
    data->Reset();
    
    // Crea los eventos con una distribuci?n pdf2 y rellena los valores en un histograma
    pdf2->SetParameter(0,1.); // Normalizaci?n de la probabilidad a 1
    /*
    // Generar eventos a mano
    double x,y,ymax(pdf2->GetMaximum());
    for (Int_t iEvt=0;iEvt<numEvts;iEvt++) {
    y = ymax;
    while (y>ymax) {
    x = random.Uniform(-1.,1.);
    y = random.Uniform(0.,ymax);
    }
    data->Fill(x);
    }
    */
    // Generar eventos autom?ticamente
    data->FillRandom("pdf2",numEvts);
    data->SetLineColor(kBlack);

    // Representar PDFs y el histograma generado
    TCanvas* c = new TCanvas("c","c",900,900);
    //
    pdf1->SetParameter(0,numEvts*binwidth); // Normaliza al n?mero de eventos
    pdf2->SetParameter(0,numEvts*binwidth); 
    pdf3->SetParameter(0,numEvts*binwidth); 
    //
    pdf1->SetParameter(1,0.); 
    pdf2->SetParameter(1,0.); 
    pdf3->SetParameter(1,0.); 
    //
    if (!doFitBeta) {
      pdf1->FixParameter(1,0); // Fija beta a 0 (desintegraci?n que conserva la paridad)
      pdf2->FixParameter(1,0);
      pdf3->FixParameter(1,0);
    }

    double chi2_pdf1,pvalue_pdf1,beta_pdf1;
    pdf1->SetLineColor(kBlack);
    pdf1->SetLineWidth(1);
    if (doFit) {
      data->Fit(pdf1,"R");
      get_pvalue_fit(pdf1,data,chi2_pdf1,ndof_pdf1,pvalue_pdf1,beta_pdf1);
    } else {
      pdf1->Draw("L");
      get_pvalue_nofit(pdf1,data,chi2_pdf1,ndof_pdf1,pvalue_pdf1);
    }
    //
    double chi2_pdf2,pvalue_pdf2,beta_pdf2;
    pdf2->SetLineColor(kBlue);
    pdf2->SetLineStyle(10);
    pdf2->SetLineWidth(1);
    if (doFit) {
      data->Fit(pdf2,"R+");
      get_pvalue_fit(pdf2,data,chi2_pdf2,ndof_pdf2,pvalue_pdf2,beta_pdf2);
    } else{
      pdf2->Draw("R+");
      get_pvalue_nofit(pdf2,data,chi2_pdf2,ndof_pdf2,pvalue_pdf2);
    }
    //
    double chi2_pdf3,pvalue_pdf3,beta_pdf3;
    pdf3->SetLineColor(kRed);
    pdf3->SetLineStyle(3);
    pdf3->SetLineWidth(2);
    if (doFit) {
      data->Fit(pdf3,"R+");
      pdf3->SetLineColor(kRed);
      pdf3->SetLineStyle(3);
      get_pvalue_fit(pdf3,data,chi2_pdf3,ndof_pdf3,pvalue_pdf3,beta_pdf3);
    } else {
      pdf3->Draw("L+");
      get_pvalue_nofit(pdf3,data,chi2_pdf3,ndof_pdf3,pvalue_pdf3);
    }


    // Estilo del histograma
    {
        gStyle->SetOptStat(0);
        gStyle->SetTextFont(13);
        gStyle->SetStripDecimals(kFALSE);
        data->SetTitle("");
        data->GetXaxis()->SetTitle("cos#theta_{h}");
        data->GetYaxis()->SetTitleOffset(0);
        data->GetYaxis()->SetTitle("N?mero de cuentas/" + binwidth_str);
        data->GetXaxis()->SetLabelSize(0.03);
        data->GetYaxis()->SetLabelSize(0.03);
        data->SetMarkerStyle(20);
        data->SetMarkerSize(0.8);
        data->SetMarkerColor(kBlack);

        TAxis* xaxis = (TAxis*)data->GetXaxis();
        TAxis* yaxis = (TAxis*)data->GetYaxis();


        xaxis->SetLabelFont(132);
        yaxis->SetLabelFont(132);
        xaxis->SetTitleFont(132);
        yaxis->SetTitleFont(132);

        data->Draw("P0E1"); }

    //Leyenda del histograma

    { auto legend = new TLegend(0.325, 0.63, 0.675, 0.85);

    legend->AddEntry(pdf1, "Esp?n #frac{1}{2}", "l");
    legend->SetLineColor(kBlack);
    legend->AddEntry(pdf2, "Esp?n #frac{3}{2}"); //// #frac{3}{2}
    legend->AddEntry(pdf3, "Esp?n #frac{5}{2}", "l");
    legend->AddEntry(data, "Valores de la simulaci?n", "p");
    legend->SetTextSize(0.03);
    legend->Draw(); }

    //
    chi2_pdf1_mean += chi2_pdf1;
    chi2_pdf2_mean += chi2_pdf2;
    chi2_pdf3_mean += chi2_pdf3;
    //
    beta_pdf1_list.push_back(beta_pdf1);
    beta_pdf1_mean += beta_pdf1;
    beta_pdf2_list.push_back(beta_pdf2);
    beta_pdf2_mean += beta_pdf2;
    beta_pdf3_list.push_back(beta_pdf3);
    beta_pdf3_mean += beta_pdf3;    
    //
    TString file_name("analysisOmegabetalibre_exp"); file_name += to_string(iExp); file_name += ".pdf";
    c->Print(file_name,"pdf");
    
  }
    
  // Muestra en pantalla un resumen de los resultados
  printf("\n");
  chi2_pdf1_mean /= numExps;
  chi2_pdf2_mean /= numExps;
  chi2_pdf3_mean /= numExps;
  //
  beta_pdf1_mean /= numExps;
  beta_pdf2_mean /= numExps;
  beta_pdf3_mean /= numExps;
  double beta_pdf1_variance(0.),beta_pdf2_variance(0.),beta_pdf3_variance(0.);
  for (Int_t iExp=0;iExp<numExps;iExp++) {
    beta_pdf1_variance += pow(beta_pdf1_list[iExp]-beta_pdf1_mean,2);
    beta_pdf2_variance += pow(beta_pdf2_list[iExp]-beta_pdf2_mean,2);
    beta_pdf3_variance += pow(beta_pdf3_list[iExp]-beta_pdf3_mean,2);
  }
  beta_pdf1_variance /= (numExps-1);
  beta_pdf2_variance /= (numExps-1);
  beta_pdf3_variance /= (numExps-1);
  //
  if (doFit && doFitBeta)
    printf("Average beta %s: %f +- %f\n",pdf1->GetName(),beta_pdf1_mean,sqrt(beta_pdf1_variance));
  printf("Average Chi2 / ndf %s: %f / %d --> p-value: %f\n\n",pdf1->GetName(),chi2_pdf1_mean,ndof_pdf1,TMath::Prob(chi2_pdf1_mean,ndof_pdf1));
  //
  if (doFit && doFitBeta)
    printf("Average beta %s: %f +- %f\n",pdf2->GetName(),beta_pdf2_mean,sqrt(beta_pdf2_variance));
  printf("Average Chi2 / ndf %s: %f / %d --> p-value: %f\n\n",pdf2->GetName(),chi2_pdf2_mean,ndof_pdf2,TMath::Prob(chi2_pdf2_mean,ndof_pdf2));
  //
  if (doFit && doFitBeta)
    printf("Average beta %s: %f +- %f\n",pdf3->GetName(),beta_pdf3_mean,sqrt(beta_pdf3_variance));
  printf("Average Chi2 / ndf %s: %f / %d --> p-value: %f\n",pdf3->GetName(),chi2_pdf3_mean,ndof_pdf3,TMath::Prob(chi2_pdf3_mean,ndof_pdf3));

  return;
}


void get_pvalue_fit(TF1 *pdf, TH1F *data, double& chi2, int& ndof, double& pvalue, double& beta)
{
  printf("Normalization %s: %4.3f +/- %4.3f\n",pdf->GetName(),pdf->GetParameter(0),pdf->GetParError(0));
  beta = pdf->GetParameter(1);
  printf("beta %s: %4.3f +/- %4.3f\n",pdf->GetName(),beta,pdf->GetParError(1));
  chi2 = data->GetFunction(pdf->GetName())->GetChisquare();
  ndof = data->GetFunction(pdf->GetName())->GetNDF();
  pvalue = TMath::Prob(chi2,ndof);
  printf("Chi2 / ndf %s: %f / %d --> p-value: %f\n",pdf->GetName(),chi2,ndof,pvalue);
}

void get_pvalue_nofit(TF1 *pdf, TH1F *data, double& chi2, int& ndof, double& pvalue)
{
  chi2 = data->Chisquare(pdf,"L"); // Use option "R" for restricting the chisquare calculation to the given range of the function
                                   // Use option "L" for using the chisquare based on the poisson likelihood (Baker-Cousins Chisquare)
  ndof = data->GetNbinsX();
  pvalue = TMath::Prob(chi2,ndof);
  printf("Chi2 / ndf %s: %f / %d --> p-value: %f\n",pdf->GetName(),chi2,ndof,pvalue);
}
