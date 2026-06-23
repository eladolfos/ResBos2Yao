#include "NonPertFit/Dataset.hh"
#include <fstream>
#include <iostream>
#include "fmt/format.h"
#include "fmt/ostream.h"

DataSet::DataSet(const std::string &filename) {
    std::ifstream datafile(filename);
    if(!datafile) throw std::runtime_error("DataSet: File " + filename + " couldn't be opened!");

    std::string line;
    size_t lineNum = 0;

    // Parse Header
    size_t headerLine = 0;
    while(std::getline(datafile, line)) {
        lineNum++;
        std::string temp = line;
        if(temp.empty()) continue;
        RemoveComment(temp);
        if(OnlyWhitespace(temp)) continue;
        ParseHeaderLine(temp, lineNum, headerLine++);
        if(headerLine == 4) break;
    }

    // Parse Body
    while(std::getline(datafile, line)) {
        lineNum++;
        std::string temp = line;
        if(temp.empty()) continue;
        RemoveComment(temp);
        if(OnlyWhitespace(temp)) continue;

        ParseLine(temp, lineNum);
    }

    datafile.close();

    // Convert to Eigen
    S = Eigen::VectorXd::Zero(m_data.size());
    P = Eigen::VectorXd::Zero(m_data.size());
    D = Eigen::VectorXd(m_data.size());
    for(size_t i = 0; i < m_data.size(); ++i) {
        S(i) = m_data[i].uncorr_error;
        D(i) = m_data[i].data;
        P(i) = m_data[i].pdf_error;
    }
    // S_inv = S.inverse();
    Print();
}

void DataSet::Print() const {
    std::cout << "DataSet: " << m_name << "\n";
    std::cout << "  ID: " << m_id << "\n";
    std::cout << "  Beam: " << m_beam << "\n";
    std::cout << "  ECM: " << m_ecm << "\n";
    std::cout << "  NPts: " << m_data.size() << "\n";
    for(size_t i = 0; i < m_data.size(); ++i) {
        std::cout << "    * " << m_data[i].Qmin << " " << m_data[i].Qmax << " " << m_data[i].ymin << " ";
//        std::cout << m_data[i].ymax << " " << m_data[i].pT << " ";
        std::cout << m_data[i].ymax << " " << m_data[i].pTmin << " " << m_data[i].pTmax << " ";
        std::cout << m_data[i].data << " " << m_data[i].uncorr_error << " " << m_data[i].pdf_error << " " << m_data[i].yk << " ";
        std::cout << m_data[i].efficiency << " " << m_data[i].corr_errors << "\n";
    }
}

void DataSet::ParseHeaderLine(const std::string &line, size_t lineNum, size_t headerLine) {
    std::vector<std::string> tokens;
    tokenize(line, tokens);
    if(headerLine == 0) {
        if(tokens.size() != 2) 
            throw std::runtime_error("Invalid header at line: " + std::to_string(lineNum));
        m_name = tokens[0];
        m_id = std::stoll(tokens[1]);
    } else if(headerLine == 1) {
        if(tokens.size() != 3)
            throw std::runtime_error("Invalid header at line: " + std::to_string(lineNum));
        m_beam = tokens[2];
    } else if(headerLine == 2) {
        if(tokens.size() != 3)
            throw std::runtime_error("Invalid header at line: " + std::to_string(lineNum));
        m_ecm = std::stod(tokens[2]);
    } else if(headerLine == 3) {
        if(tokens.size() != 3)
            throw std::runtime_error("Invalid header at line: " + std::to_string(lineNum));
        if(tokens[2] == "uncorrelated") {
            m_pdf = PDFError::uncorr;
        } else if(tokens[2] == "correlated") {
            m_pdf = PDFError::corr;
        } else if(tokens[2] == "mixed") {
            m_pdf = PDFError::mixed;
        } else {
            throw std::runtime_error("Invalid pdf error. Options are \"uncorrelated\", \"correlated\", \"mixed\"");
        }
    }
}

void DataSet::ParseLine(const std::string &line, size_t lineNum) {
    std::vector<std::string> tokens;
    tokenize(line, tokens);
    DataPoint point;
    size_t idx = 0;
    point.Qmin = std::stod(tokens[idx++]);
    point.Qmax = std::stod(tokens[idx++]);
    point.ymin = std::stod(tokens[idx++]);
    point.ymax = std::stod(tokens[idx++]);

/*    point.pT = std::stod(tokens[4]);
    point.data = std::stod(tokens[5]);
    point.uncorr_error = std::stod(tokens[6]);
    // point.pdf_error = std::stod(tokens[7])/100;
    point.yk = std::stod(tokens[7]);
    point.efficiency = std::stod(tokens[8]);
    if(ncorr == 0) {
        ncorr = tokens.size() - 9;
    } else {
        if(ncorr != tokens.size() - 9) 
            throw std::runtime_error("Invalid data point at line: " + std::to_string(lineNum));
    }

    Eigen::VectorXd corr(static_cast<int>(ncorr));
    for(size_t i = 0; i < ncorr; ++i) {
        corr[i] = std::stod(tokens[9+i])/100;
    }
    point.corr_errors = corr;
*/
    point.pT = std::stod(tokens[idx++]);
    point.pTmin = std::stod(tokens[idx++]);
    point.pTmax = std::stod(tokens[idx++]);
    point.data = std::stod(tokens[idx++]);
    point.uncorr_error = std::stod(tokens[idx++]);
    if(m_pdf == PDFError::uncorr || m_pdf == PDFError::mixed)
       { point.pdf_error = std::stod(tokens[idx++])/100;}
    else { point.pdf_error = 0;}
    if(m_name == "ATLAS_Rapidity" || m_name == "CMS_Rapidity" || m_name == "LHCb") point.pdf_error = point.pdf_error / 3.0;
    point.yk = std::stod(tokens[idx++]);
    point.efficiency = std::stod(tokens[idx++]);
    if(ncorr == 0) {
        ncorr = tokens.size() - idx;
    } else {
        if(ncorr != tokens.size() - idx)
            throw std::runtime_error("Invalid data point at line: " + std::to_string(lineNum));
    }

    Eigen::VectorXd corr(static_cast<int>(ncorr));
    for(size_t i = 0; i < ncorr; ++i) {
        corr[i] = std::stod(tokens[idx+i])/100;
    }
    point.corr_errors = corr;

    m_data.push_back(point);
}

double DataSet::Chi2(const Eigen::VectorXd &theory, bool print) const {
    Eigen::MatrixXd beta(m_data.size(), ncorr);
    Eigen::VectorXd pdf(m_data.size());
    Eigen::VectorXd pdfPlusS(m_data.size());
    for(size_t k = 0; k < m_data.size(); ++k) {
        for(size_t alpha = 0; alpha < ncorr; ++alpha) {
            beta(k, alpha) = m_data[k].corr_errors[alpha]*theory(k);
        }
        pdf(k) = P(k)*theory(k);
        pdfPlusS(k) = sqrt(pdf(k) * pdf(k) + S(k) * S(k));
   }

//    Eigen::MatrixXd S_inv = (S+pdf).array().inverse().matrix().asDiagonal();
    Eigen::MatrixXd S_inv = (pdfPlusS).array().inverse().matrix().asDiagonal();

    Eigen::MatrixXd b = S_inv*beta;
    Eigen::VectorXd d = S_inv*(D-theory);
    Eigen::MatrixXd C_inv = Eigen::MatrixXd::Identity(NPoints(), NPoints())-b*(Eigen::MatrixXd::Identity(ncorr, ncorr)+(S_inv*beta).transpose()*(S_inv*beta)).inverse()*b.transpose();
    Eigen::VectorXd lambda = b.transpose()*C_inv*d;
    Eigen::VectorXd r = C_inv*d;

    double chi2 = r.dot(r) + lambda.dot(lambda);

    if(print) {
        std::string results;
        results+= fmt::format("Dataset: {}\n", m_name);
        results+= fmt::format("Npts = {}, chi^2 = {}, chi^2/N = {}\n", m_data.size(), chi2, chi2/m_data.size());
        results+= fmt::format("Lambda^2 = {}, lambda_alpha = {}\n", lambda.dot(lambda), lambda.transpose());
        results+= fmt::format("{:<5} {:<5} {:<4} {:<4} {:<6} {:<10} {:<10} {:<10} {:<7} {:<10} {:<10} {:<10} {:<7}\n",
                              "QMin", "QMax", "ymin", "ymax", "pT", "data", "theory",
                              "totErr", "ChiSq", "shift", "shiftData", "uncorrErr", "reducedChi2");
        std::string format{"{:<5.2f} {:<5.2f} {:<4.1f} {:<4.1f} {:<4.3f} {:<10.4e} {:<10.4e} {:<10.4e} {:< 7.3f} {:< 10.4e} {:<10.4e} {:<10.4e} {:< 7.3f}\n"};
        Eigen::VectorXd corr_err = beta*lambda;
        Eigen::VectorXd shifted = D - corr_err;
        for(size_t i = 0; i < m_data.size(); ++i) {
            double sign = (D(i)-theory(i))/std::abs(D(i)-theory(i));
            double tot_err = sqrt(S(i)*S(i) + P(i)*P(i)*theory(i)*theory(i) + corr_err(i)*corr_err(i));
            results+= fmt::format(format,
                                  m_data[i].Qmin, m_data[i].Qmax, m_data[i].ymin, m_data[i].ymax, m_data[i].pT,
                                  m_data[i].data, theory(i), tot_err,
                                  sign*pow((m_data[i].data-theory(i))/tot_err, 2), 
                                  corr_err(i), shifted(i), m_data[i].uncorr_error, 
                                  r(i)*r(i)*r(i)/std::abs(r(i)));
        }
        std::ofstream output(m_name + "_results.dat");
        output << results;
        output.close();
        std::cout << results;
    }

    return chi2;
}
