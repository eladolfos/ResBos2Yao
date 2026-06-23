#pragma once

#include <string>
#include <vector>
#include <Eigen/Dense>

template<class ContainerT>
void tokenize(const std::string& str, ContainerT& tokens,
              const std::string& delimiters=" \n\t", bool trimEmpty=true) {
    std::string::size_type lastPos = 0, length = str.length();
    
    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while(lastPos < length+1) {
        std::string::size_type pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos) pos = length;

        if(pos != lastPos || !trimEmpty) {
            tokens.push_back(value_type(str.data()+lastPos,
                             static_cast<size_type>(pos)-lastPos));
        }
        lastPos = pos+1;
    }
}

enum class PDFError {
    uncorr,
    corr,
    mixed,
};

struct DataPoint {
    double Qmin, Qmax, ymin, ymax, pT;
    double pTmin, pTmax;
    double data, uncorr_error, pdf_error;
    double yk, efficiency;
    Eigen::VectorXd corr_errors;
};

class DataSet {
    public:
        DataSet() = default;
        DataSet(const std::string&);
        DataSet(const DataSet &other) = default;
        DataSet &operator=(const DataSet &other) = default;

        size_t ID() const { return m_id; }
        DataPoint GetDataPoint(size_t idx) const { return m_data[idx]; }
        size_t NPoints() const { return m_data.size(); }
        std::string Name() const { return m_name; }
        std::string Beam() const { return m_beam; }
        double ECM() const { return m_ecm; }
        double Chi2(const Eigen::VectorXd&, bool) const;
        size_t NCorr() const { return ncorr; }
        void Print() const;

    private:
        inline void RemoveComment(std::string &line) const {
            if(line.find('#') != line.npos) line.erase(line.find('#'));
        }
        inline bool OnlyWhitespace(const std::string &line) const {
            return (line.find_first_not_of(' ') == line.npos);
        }
        void ParseHeaderLine(const std::string &line, size_t lineNum, size_t headerLine);
        void ParseLine(const std::string &line, size_t lineNum);

        // Variables
        std::string m_name;
        size_t m_id{};
        std::string m_beam;
        double m_ecm{};
        std::vector<DataPoint> m_data;
        Eigen::VectorXd D, S, P;
        size_t ncorr{};
        PDFError m_pdf;
};
