#include "NonPertFit/FixedTarget.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Utility.hh"
#include "ResBos/Brent.hh"

FixedTarget::FixedTarget()
{
    resbos = std::shared_ptr<ResBos::ResBos>(new ResBos::ResBos());
}

void FixedTarget::ReadFile(std::string FileName)
{
    CalcType = FileName;

    std::string FileLine;
    std::vector<std::vector<double>> FullTable;
    std::vector<double> Pert1Table;
    std::vector<double> Pert2Table;
    std::vector<double> QTable;
    std::vector<double> QtTable;
    std::vector<double> yTable;

    std::ifstream datafile;
    datafile.open(FileName, std::ios::in);

    int nHead = 6;

    for(int i = 0; i < nHead; i++){//first 3(2) comment lines
      getline(datafile, FileLine);
      std::cout<<FileLine<<std::endl;
    }

    while(getline(datafile, FileLine)){
      std::vector<double> LineVector;
      double Value;
      std::stringstream LineStream(FileLine);
      while(LineStream >> Value){
        LineVector.push_back(Value);
      }
      if(LineVector.size() == 0) break;

      FullTable.push_back(LineVector);//FullTable.at(i).at(j) is i row and j column
    }

    std::ifstream pertfile;
    pertfile.open(FileName + "_pert.txt", std::ios::in);

    while(getline(pertfile, FileLine)){
      std::vector<double> LineVector;
      double Value;
      std::stringstream LineStream(FileLine);
      while(LineStream >> Value){
        LineVector.push_back(Value);
      }
      if(LineVector.size() == 0) break;

      Pert1Table.push_back(LineVector.at(3));
      Pert2Table.push_back(LineVector.at(5));
      QTable.push_back(LineVector.at(0));
      QtTable.push_back(LineVector.at(1));
      yTable.push_back(LineVector.at(2));
    }

    if(FileName == "e605") Init("pCu", 38.8);
    else if(FileName == "e866f_pd") Init("pd", 38.76);
    else if(FileName == "e866f_pp") Init("pp", 38.76);
    else if(FileName == "e866ppxf") Init("pp", 38.75);
    else if(FileName == "e906aF_pd") Init("pd", 15.07);
    else if(FileName == "e906aF_pp") Init("pp", 15.07);


    std::ofstream outfile(FileName + "_ResBos2_w" + std::to_string(resbos -> GetCalculation() -> GetAOrder()) + std::to_string(resbos -> GetCalculation() -> GetBOrder()) + std::to_string(resbos -> GetCalculation() -> GetCOrder()) + ".dat");
    for(int i = 0; i < FullTable.size(); i++){
      //if(i != 0 && i != 3 && i != 42) continue;
      double y = FullTable.at(i).at(0);
      double Q = FullTable.at(i).at(1);
      double data = FullTable.at(i).at(3);

      for(int ipert = 0; ipert < QTable.size(); ipert++){
        if(Q == QTable.at(ipert) && y == yTable.at(ipert)){
          if(fabs(Pert1Table.at(ipert)) < 1e-20) continue;
          qt_interp.push_back(QtTable.at(ipert));
          pert1_interp.push_back(Pert1Table.at(ipert));
          pert2_interp.push_back(Pert2Table.at(ipert));
          yk_interp.push_back(Pert2Table.at(ipert) / Pert1Table.at(ipert));
        }
      }

      double result;
      if(asym->GetAsymOrder() >= 2) result = GetPrediction(y, Q, true);
      else result = GetPrediction(y, Q);
      outfile<<std::setw(6)<<*std::right<<std::fixed<<std::setprecision(3)<<y<<"   ";
      outfile<<std::setw(4)<<*std::right<<std::fixed<<std::setprecision(2)<<Q<<"   ";
      outfile<<std::setw(9)<<*std::right<<std::uppercase<<std::scientific<<std::setprecision(3)<<data<<"   ";
      outfile<<std::setw(9)<<*std::right<<std::uppercase<<std::scientific<<std::setprecision(3)<<result<<std::endl;

      qt_interp.clear();
      pert1_interp.clear();
      pert2_interp.clear();
      yk_interp.clear();
    }

}

void FixedTarget::Init(std::string m_beam, double m_ECM)
{
    this->m_beam = m_beam;
    this->m_ECM = m_ECM;

    // Setup PDFs
    Utility::PDF* pdf = new Utility::PDF(global_settings.get());
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    Utility::Hoppet* hoppet = new Utility::Hoppet(pdf -> GetPDF());

    // Setup beams
    std::pair<Beam::Beam*, Beam::Beam*> beams = Beam::Beam::MakeBeams(m_beam, m_ECM);
    beams.first -> SetPDF(pdf);
    beams.second -> SetPDF(pdf);
    beams.first -> SetHoppet(hoppet);
    beams.second -> SetHoppet(hoppet);
    resbos -> SetBeams(beams);

    // Setup the process
    std::string processName = "A0";

    std::unique_ptr<ResBos::Process> process = ResBos::ProcessFactory::Instance().Create(
        processName,resbos,*global_settings);
    process -> Initialize(*global_settings);
    resbos -> SetProcess(std::move(process));

    // Setup the calculation class
    auto resum = ResBos::CalculationFactory::Instance().Create("Resummation");
    asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    pert = ResBos::CalculationFactory::Instance().Create("Perturbative");
    resum -> Initialize(global_settings.get(), resbos);
    asym -> Initialize(global_settings.get(), resbos);
    pert -> Initialize(global_settings.get(), resbos);
    resbos -> SetCalculation(std::move(resum));

    // Setup the convolutions only for the first experiment since the rest can reuse
    static Beam::Convolution *conv;
    if(!conv) {
        conv = new Beam::Convolution(resbos, pdf, hoppet, true,
                resbos -> GetCalculation() -> GetCOrder(),
                resbos -> GetCalculation() -> GetScheme(),
                resbos -> GetCalculation() -> GetC1(),
                resbos -> GetCalculation() -> GetC2(),
                resbos -> GetCalculation() -> GetC3());
        conv -> SetPDFs(pdfs);
        std::vector<Conv> convEnums = {
            Conv::C1, Conv::C1P1, Conv::C1P1P1, Conv::C1P2, Conv::C2
        };

        for(auto convEnum : convEnums) {
            if(!conv -> LoadGrid(convEnum)) {
                conv -> GenerateGrid(convEnum);
                conv -> SaveGrid(convEnum);
            }
        }

        conv -> GenerateGrid(Conv::C);
    }

    // Add convolution results to the beam class
    beams.first -> SetConvolution(conv);
    beams.second -> SetConvolution(conv);
}

double FixedTarget::GetPrediction(double y, double Q, bool is_yk)
{
/*    std::vector<double> qtVector = {0.09106, 0.1006, 0.1112, 0.1229, 0.1358, 0.1501, 0.1659, 0.1834, 0.2027, 0.224 , 0.2475, 0.2736, 0.3023, 0.3341, 0.3693, 0.4081, 0.451 , 0.4985, 0.5509, 0.6088,
                                    0.6729, 0.7436, 0.8218, 0.9083, 1.004 , 1.109 , 1.226 , 1.355 , 1.497 , 1.655 , 1.829 , 2.021 , 2.234 , 2.469 , 2.729 , 3.016 , 3.333 , 3.683 , 4.071 , 4.499 , 4.972 ,
                                    5.495 , 6.073 , 6.711 , 7.417 , 8.197 , 9.059, 10.00};


    for(int i = 0; i < qtVector.size(); i++){
        double pT = qtVector.at(i);
        int locate = Locate(qt_interp, pT);
        std::vector<double> X = {qt_interp.at(locate), qt_interp.at(locate+1)};
        std::vector<double> Y = {yk_interp.at(locate), yk_interp.at(locate+1)};
        double yk = Polint(X, Y, pT);
        if(!is_yk) yk = 1.0;

        std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
        std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
        std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);

        std::cout<<pT<<"   "<<tmp[0]<<"   "<<tmp[0] + tmpPert[0] * yk - tmpAsym[0]<<"   "<<tmpPert[0] * yk<<"   "<<tmpAsym[0]<<"   "<<tmpPert[0] * yk - tmpAsym[0]<<"   "<<yk<<std::endl;
    }
*/
    double result = 0.0;
    double err;

    double qtMax = 10.0;
    if(CalcType == "e906aF_pp" || CalcType == "e906aF_pd") qtMax = 5.0;
    if(is_yk) qtMax = qt_interp.at(qt_interp.size() - 1) - 0.0001;

    //double y = 0.0;
    //double Q = 10.709;

    int Npt = 1000;
    double pTmin = 0.0001;
    double pTmax = qtMax;
    double dqt = (pTmax - pTmin) / (Npt-1);

    // Determine W-piece integration range
    std::map<double, double> SigWMap;
    double pTPeak;
    double pTZero;
    double pTCrossing = 0.0001;
    for(size_t i = 0; i < Npt; i++) {
        double pT = pTmin + static_cast<int>(i)*dqt;
        std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
        std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
        std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);
        if(tmp.at(0) < 0.0) break;
        SigWMap[tmp[0]] = pT;
        pTZero = pT;
        //if(tmp.at(0) < 0.0) break;
        //std::cout<<"pT = "<<pT<<" W = "<<tmp[0]<<" P = "<<tmpPert[0]<<" A = "<<tmpAsym[0]<<" W+Y = "<<tmp[0] + tmpPert[0] - tmpAsym[0]<<std::endl;
    }
    auto SigWiter = SigWMap.end();
    SigWiter--;
    pTPeak = SigWiter->second;

    auto WYCrossing = [&](double pT){
       std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
       std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
       std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);

       return tmp[0] + tmpPert[0] - tmpAsym[0] - tmpPert[0];
    };

    Brent brent{WYCrossing};
    try{
        double tmp = brent.CalcRoot(pTPeak, pTZero);
        if(!std::isnan(tmp))
            pTCrossing = tmp;
    } catch (std::runtime_error &e) {
        pTCrossing = pTZero;
    }
    //std::cout<<"Peak = "<<pTPeak<<" Zero = "<<pTZero<<" pTCrossing = "<<pTCrossing<<std::endl;

    Npt = 40;
    pTmin = 0.0001;
    pTmax = pTCrossing;
    dqt = (pTmax - pTmin) / (Npt - 1);

    // Only calculate Y Piece if it hasn't been calculated yet
    std::vector<double> SigY(Npt);
    std::vector<double> SigA(Npt);
    std::vector<double> SigP(Npt);
    for(size_t i = 0; i < Npt; i++) {
        double pT = pTmin + static_cast<int>(i)*dqt;
        double yk;

        if(is_yk){
          int locate = Locate(qt_interp, pT);
          if(pT < 1.0){
            std::vector<double> X = {qt_interp.at(locate), qt_interp.at(locate+1), qt_interp.at(locate+2), qt_interp.at(locate+3)};
            std::vector<double> Y = {yk_interp.at(locate), yk_interp.at(locate+1), yk_interp.at(locate+2), yk_interp.at(locate+3)};
            yk = Polint(X, Y, pT);
          }
          else{
            std::vector<double> X = {qt_interp.at(locate), qt_interp.at(locate+1)};
            std::vector<double> Y = {yk_interp.at(locate), yk_interp.at(locate+1)};
            yk = Polint(X, Y, pT);
          }
        }
        if(!is_yk) yk = 1.0;

        if(is_yk){
          if(pT < 0.5){
            yk = 1.0;
            asym -> SetAsymOrder(1);
          }
          else asym -> SetAsymOrder(2);
        }

        std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
        std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);

        if(CalcType == "e605"){
          SigY[i] = tmpPert[0] * yk - tmpAsym[0];
          SigA[i] = tmpAsym[0];
          SigP[i] = tmpPert[0] * yk;
        }
        else{
          SigY[i] = (tmpPert[0] * yk - tmpAsym[0]) * (m_ECM / 2.0 / sqrt(Q * Q + pT * pT) / cosh(y));
          SigA[i] = tmpAsym[0] * (m_ECM / 2.0 / sqrt(Q * Q + pT * pT) / cosh(y));
          SigP[i] = tmpPert[0] * yk * (m_ECM / 2.0 / sqrt(Q * Q + pT * pT) / cosh(y));
        }
        tmpPert.clear();
        tmpAsym.clear();
        std::cout<<"Y piece: Q = "<<Q<<" pT = "<<pT<<" y = "<<y<<" pert = "<<tmpPert[0]<<" asym = "<<tmpAsym[0]<<" y = "<<tmpPert[0] - tmpAsym[0]<<std::endl;
    }
    double asymInt = Simpson(Npt, dqt, SigA, err);
    double pertInt = Simpson(Npt, dqt, SigP, err);
    double yInt = Simpson(Npt, dqt, SigY, err);
    //result += (pertInt - asymInt) * pow(Q, 3.0) / 1000.0 * 2.0 * Q;
    result += yInt * pow(Q, 3.0) / 1000.0 * 2.0 * Q;
    //std::cout<<"Y-piece = "<<(pertInt - asymInt)*pow(Q, 3.0) / 1000.0 * 2.0 * Q<<std::endl; 

    SigA.clear();
    SigP.clear();
    SigY.clear();

    Npt = 40;
    pTmin = 0.0001;
    pTmax = pTCrossing;
    dqt = (pTmax - pTmin) / (Npt - 1);

    std::vector<double> SigW(Npt);
    for(size_t i = 0; i < Npt; i++) {
        double pT = pTmin + static_cast<int>(i)*dqt;
        std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
        if(CalcType == "e605") SigW[i] = tmp[0];
        else SigW[i] = tmp[0] * (m_ECM / 2.0 / sqrt(Q * Q + pT * pT) / cosh(y));
        tmp.clear();
        //std::cout<<"W piece: Q = "<<Q<<" pT = "<<pT<<" y = "<<y<<" resum = "<<tmp[0]<<std::endl;
    }
    auto resumInt = Simpson(Npt, dqt, SigW, err);

    result += resumInt*pow(Q, 3.0) / 1000.0 * 2.0 * Q;
    //std::cout<<"W-piece = "<<resumInt*pow(Q, 3.0) / 1000.0 * 2.0 * Q<<std::endl;

    SigW.clear();

    // Only Perturbative
    Npt = 20;
    pTmin = pTCrossing;
    pTmax = qtMax;
    dqt = (pTmax - pTmin) / (Npt - 1);

    for(size_t i = 0; i < Npt; i++) {
        double pT = pTmin + static_cast<int>(i)*dqt;
        double yk;

        if(is_yk){
          int locate = Locate(qt_interp, pT);
          if(pT < 1.0){
            std::vector<double> X = {qt_interp.at(locate), qt_interp.at(locate+1), qt_interp.at(locate+2), qt_interp.at(locate+3)};
            std::vector<double> Y = {yk_interp.at(locate), yk_interp.at(locate+1), yk_interp.at(locate+2), yk_interp.at(locate+3)};
            yk = Polint(X, Y, pT);
          }
          else{
            std::vector<double> X = {qt_interp.at(locate), qt_interp.at(locate+1)};
            std::vector<double> Y = {yk_interp.at(locate), yk_interp.at(locate+1)};
            yk = Polint(X, Y, pT);
          }
        }
        if(!is_yk) yk = 1.0;

        std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
        if(CalcType == "e605") SigP[i] = tmpPert[0] * yk;
        else SigP[i] = tmpPert[0] * yk * (m_ECM / 2.0 / sqrt(Q * Q + pT * pT) / cosh(y));
        tmpPert.clear();
        //std::cout<<"Y piece: pT = "<<pT<<" pert = "<<tmpPert[0]<<std::endl;
    }
    pertInt = Simpson(Npt, dqt, SigP, err);
    result += pertInt * pow(Q, 3.0) / 1000.0 * 2.0 * Q;
    //std::cout<<"Perturbative piece = "<<pertInt * yk * pow(Q, 3.0) / 1000.0 * 2.0 * Q<<std::endl;

    SigP.clear();

    std::cout<<"y = "<<y<<" Q = "<<Q<<" result = "<<result<<std::endl;

    return result;
}

double FixedTarget::Simpson(const size_t& nx, const double& dx, const std::vector<double>& f, double& err)
{
    double simp, errd, trpz;
    size_t ms = 0;
    if(dx <= 0) return 0;
    if(nx <= 1) simp = 0;
    else if(nx == 2) {
        simp = (f[0]+f[1])/2.0;
        errd = (f[0]-f[1])/2.0;
    } else {
        ms = nx % 2;

        double add = 0;
        size_t nz = nx;
        if(ms == 0) {
            add = (9.0*f[nx-1]+19.0*f[nx-2]-5*f[nx-3]+f[nx-4])/24.0;
            nz--;
        }

        if(nz == 3) {
            simp = (f[0]+4*f[1]+f[2])/3.0;
            trpz = (f[0]+2*f[1]+f[2])/2.0;
        } else {
            double se = f[1];
            double s0 = 0;
            size_t nm1 = nz - 1;
            for(size_t i = 3; i < nm1; i += 2) {
                se += f[i];
                s0 += f[i-1];
            }
            simp = (f[0]+4*se+2*s0+f[nz-1])/3.0;
            trpz = (f[0]+2*se+2*s0+f[nz-1])/2.0;
        }

        errd = trpz - simp;
        simp = simp + add;
    }

    if(fabs(simp) > 1E-10) err = errd/simp;
    else err = 0.0;

    simp *= dx;

    return simp;
}

double FixedTarget::Polint(std::vector<double> InpVector, std::vector<double> FuncVector, double Xinp)
{
 int NMax = 10;
 std::vector<double> C(NMax);
 std::vector<double> D(NMax);

 double Yout = 0.0;
 double DY = 0.0;

 int N = InpVector.size();

 int NS = 1;

 double Diff = fabs(Xinp - InpVector.at(0));

 for(int i = 1; i <= N; i++){
   double DiffT = fabs(Xinp - InpVector.at(i - 1));
   if(DiffT < Diff){
     NS = i;
     Diff = DiffT;
   }
   C.at(i - 1) = FuncVector.at(i - 1);
   D.at(i - 1) = FuncVector.at(i - 1);
 }

 Yout = FuncVector.at(NS - 1);
 NS = NS - 1;

 for(int M = 1; M <= N - 1; M++){
   for(int J = 1; J <= N - M; J++){
     double H0 = InpVector.at(J - 1) - Xinp;
     double HP = InpVector.at(J + M - 1) - Xinp;
     double W = C.at(J) - D.at(J - 1);
     double Den = H0 - HP;
     if(fabs(Den) < 1e-20) std::cout<<" DEN = 0 in Polint"<<std::endl;
     Den = W / Den;
     D.at(J - 1) = HP * Den;
     C.at(J - 1) = H0 * Den;
   }
   if((2 * NS) < (N - M)){
     DY = C.at(NS);
   }
   else{
     DY = D.at(NS - 1);
     NS = NS - 1;
   }
   Yout = Yout + DY;
 }

 C.clear();
 D.clear();

 //cout<<"Polint: "<<Yout<<endl;

 return Yout;
}

int FixedTarget::Locate(std::vector<double> Points, double Input)
{
 int Position = 0;

 int N = Points.size();

 int Lower = 0;
 int Upper = N + 1;

 int Middle = 0;
 while((Upper - Lower) > 1){
   Middle = (Lower + Upper) / 2;
   int logic1 = (Points.at(N - 1) > Points.at(0));
   int logic2 = (Input > Points.at(Middle - 1));

   if(logic1 == logic2){
     Lower = Middle;
   }
   else{
     Upper = Middle;
   }
 }

 Position = Lower - 1;

 if(Position < 0) Position = 0;

 return Position;
}

