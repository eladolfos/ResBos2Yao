#include "ResBos/GridIO.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/Grid2D.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <deque>
#include <set>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        if(!item.empty()) *(result++) = item;
    }
}

template<typename T>
std::string to_string(const T a, const int n=3){
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a;
    return out.str();
}

namespace IO {
    GridIO::GridIO(const Settings &s) {
        processName = s.GetSetting<std::string>("Process");
        pdfName = s.GetSetting<std::string>("PDF");
        iSet = s.GetSetting<int>("iSet");
        ecm = s.GetSetting<double>("ECM");
        beam = s.GetSetting<std::string>("Beam");
        C1 = s.GetSetting<double>("C1");
        C2 = s.GetSetting<double>("C2");
        C3 = s.GetSetting<double>("C3");
        muF = s.GetSetting<double>("muF");
        muR = s.GetSetting<double>("muR");
        qMin = s.GetSetting<double>("Qmin");
        qMax = s.GetSetting<double>("Qmax");
        qtMin = s.GetSetting<double>("qTmin");
        qtMax = s.GetSetting<double>("qTmax");
        yMin = s.GetSetting<double>("Ymin");
        yMax = s.GetSetting<double>("Ymax");
        asymOrder = s.GetSetting<int>("AsymOrder");
        delSigOrder = s.GetSetting<int>("DeltaSigmaOrder");
        scheme = s.GetSetting<std::string>("scheme");
        AOrder = s.GetSetting<int>("AOrder");
        BOrder = s.GetSetting<int>("BOrder");
        COrder = s.GetSetting<int>("COrder");
        HOrder = s.GetSetting<int>("HOrder");
    }

    bool GridIO::FindGrid3D(GridType mode) {
        // Create filename, and store for later
        fname3D = "Grids/"+processName+"_"+beam+"_"+to_string(ecm/1000,1)+"_"
            +pdfName+std::to_string(iSet)+"_"+std::to_string((int)qMin)+"."
            +std::to_string((int)qMax)+"_"+std::to_string((int)qtMin)+"."
            +std::to_string((int)qtMax)+"_"+to_string(yMin,1)+"."
            +to_string(yMax,1)+"_";
        if(mode == GridType::Tot) {
            fname3D += scheme+"_"+std::to_string(AOrder)+std::to_string(BOrder)+std::to_string(COrder);
            if(scheme.compare("CFG") == 0) fname3D += std::to_string(HOrder);
            fname3D += "_"+std::to_string(asymOrder)+"_";
            fname3D += to_string(C1,1)+"_"+to_string(C2,1)+"_"+to_string(C3,1)+"_"
                +to_string(muR,1)+"_"+to_string(muF,1)+"T.grid";
        } else if(mode == GridType::Resum) {
            fname3D += scheme+"_"+std::to_string(AOrder)+std::to_string(BOrder)+std::to_string(COrder);
            if(scheme.compare("CFG") == 0) fname3D += std::to_string(HOrder);
            fname3D +="_"+to_string(C1,1)+"_"+to_string(C2,1)+"_"+to_string(C3,1)+"R.grid";
        } else if(mode == GridType::Asym) {
            fname3D += std::to_string(asymOrder)+"_"+to_string(muR,1)+"_"+to_string(muF,1)+"A.grid";
        } else if(mode == GridType::DelSig) {
            fname3D += std::to_string(delSigOrder)+"_"+to_string(muR,1)+"_"+to_string(muF,1)+"D.grid";
        }

        struct stat buffer;

        //Check for exactMatch
        bool exactMatch = stat(fname3D.c_str(),&buffer) == 0;
        if(exactMatch) return true;

        //Check if the range has been generated before in a larger grid
        Directory gridDir("Grids/");
        std::vector<std::string> grids = gridDir.GetFiles();
        for(auto grid : grids) {
            if(CheckGrid(grid,mode)) {
                fname3D = "Grids/" + grid;
                return true;
            }
        }
        //TODO: Generate only the missing phase space points if a smaller grid already exists.
        //      This requires a lot more work, in adjusting the code for how the grids are generated to allow
        //      for this possibility. This is not important for now, if we say in the manual to generate the largest
        //      phase space first, and then in subsequent runs, focus in a more specific region (enhance statistics in corners)
        return false;
    }

    bool GridIO::CheckGrid(std::string grid,GridType mode) const {
        // Check GridType
        if(ToGridType(grid[grid.size()-6]) != mode) return false;
        grid.erase(grid.end()-6,grid.end());
        
        // Parse grid name into tokens for comparison
        size_t pos = 0;
        std::vector<std::string> tokens;
        while((pos=grid.find("_")) != std::string::npos) {
            tokens.push_back(grid.substr(0,pos));
            grid.erase(0,pos+1);
        }
        tokens.push_back(grid);

        // Check Process
        if(tokens[0].compare(processName) != 0) return false;
        // Check Beam
        if(tokens[1].compare(beam) != 0) return false;
        // Check ECM
        if(tokens[2].compare(to_string(ecm/1000,1)) != 0) return false;
        // Check PDF
        if(tokens[3].compare(pdfName+std::to_string(iSet)) != 0) return false;
        // Check Q Range
        pos=tokens[4].find(".");
        double qMinGrid = std::stod(tokens[4].substr(0,pos));
        double qMaxGrid = std::stod(tokens[4].substr(pos+1,tokens[4].size()));
        if(qMin < qMinGrid || qMax > qMaxGrid) return false;
        // Check Qt Range
        pos=tokens[5].find(".");
        double qtMinGrid = std::stod(tokens[5].substr(0,pos));
        double qtMaxGrid = std::stod(tokens[5].substr(pos+1,tokens[5].size()));
        if(qtMin < qtMinGrid || qtMax > qtMaxGrid) return false;
        // Check y Range
        pos=tokens[6].find(".");
        double yMinGrid = std::stod(tokens[6].substr(0,pos+2));
        double yMaxGrid = std::stod(tokens[6].substr(pos+3,tokens[6].size()));
        if(yMin < yMinGrid || yMax > yMaxGrid) return false;
        // Check Scheme, Orders, and Scales
        if(mode == GridType::Tot) {
            if(tokens[7].compare(scheme) != 0) return false;
            if(scheme.compare("CSS") == 0) {
                if(std::stoi(tokens[8])%10 != COrder) return false;
                if(std::stoi(tokens[8])/10%10 != BOrder) return false;
                if(std::stoi(tokens[8])/100%10 != AOrder) return false;
            } else {
                if(std::stoi(tokens[8])%10 != HOrder) return false;
                if(std::stoi(tokens[8])/10%10 != COrder) return false;
                if(std::stoi(tokens[8])/100%10 != BOrder) return false;
                if(std::stoi(tokens[8])/1000%10 != AOrder) return false;
            }
            if(std::stoi(tokens[9]) != asymOrder) return false;
            if(std::stod(tokens[10]) != C1) return false;
            if(std::stod(tokens[11]) != C2) return false;
            if(std::stod(tokens[12]) != C3) return false;
            if(std::stod(tokens[13]) != muR) return false;
            if(std::stod(tokens[14]) != muF) return false;
        } else if(mode == GridType::Resum) {
            if(tokens[7].compare(scheme) != 0) return false;
            if(scheme.compare("CSS") == 0) {
                if(std::stoi(tokens[8])%10 != COrder) return false;
                if(std::stoi(tokens[8])/10%10 != BOrder) return false;
                if(std::stoi(tokens[8])/100%10 != AOrder) return false;
            } else {
                if(std::stoi(tokens[8])%10 != HOrder) return false;
                if(std::stoi(tokens[8])/10%10 != COrder) return false;
                if(std::stoi(tokens[8])/100%10 != BOrder) return false;
                if(std::stoi(tokens[8])/1000%10 != AOrder) return false;
            }
            if(std::stod(tokens[9]) != C1) return false;
            if(std::stod(tokens[10]) != C2) return false;
            if(std::stod(tokens[11]) != C3) return false;
        } else if(mode==GridType::Asym) {
            if(std::stoi(tokens[7]) != asymOrder) return false;
            if(std::stod(tokens[8]) != muR) return false;
            if(std::stod(tokens[9]) != muF) return false;
        } else if(mode==GridType::DelSig) {
            if(std::stoi(tokens[7]) != delSigOrder) return false;
            if(std::stod(tokens[8]) != muR) return false;
            if(std::stod(tokens[9]) != muF) return false;
        }

        return true;
    }

    void GridIO::SaveGrid3D(Grid3D grid) const {
        // Check if the grids directory exists, if it doesn't create it
        struct stat st = {0};
        if(stat("Grids",&st) == -1) {
            mkdir("Grids",0700);
        }

        // Open file name based off the checked for grid
        std::ofstream output(fname3D.c_str());

        for(unsigned int iX = 0; iX < grid.at(0)->GetXDim(); iX++) {
            double x = grid.at(0)->GetX(iX);
            for(unsigned int iY = 0; iY < grid.at(0)->GetYDim(); iY++) {
                double y = grid.at(0)->GetY(iY);
                for(unsigned int iZ = 0; iZ < grid.at(0)->GetZDim(); iZ++) {
                    double z = grid.at(0)->GetZ(iZ);
                    output << x << "  " << y << "  " << z << "  ";
                    for(auto g : grid) {
                        output << g.second->FindPoint(iX,iY,iZ) << "  ";
                    }
                    output << std::endl;
                }
            }
        }


        //char *errMsg = 0;
        //int rc = sqlite3_open(filename.c_str(),&db);
        //if(rc) {
        //    sqlite3_close(db);
        //    throw std::runtime_error("Convolution: Unable to obtain database to store grid");
        //}
        ////Create CXF SQL table
        //std::string sql = "CREATE TABLE CXF(ID INT PRIMARY KEY NOT NULL, PID INT NOT NULL, X REAL NOT NULL, Q REAL NOT NULL, CXF REAL NOT NULL)";
        //rc = sqlite3_exec(db,sql.c_str(),callback,0,&errMsg);
        //if(rc != SQLITE_OK) {
        //    sqlite3_free(errMsg);
        //    throw std::runtime_error("Convolution: Error in SQL execution");
        //}
    }

    Grid3D GridIO::LoadGrid3D() const {
        //Initialize the Grid3D objects
        Grid3D grid;

        std::ifstream input(fname3D.c_str());

        std::vector<double> x, y, z;
        std::set<double> xSet, ySet, zSet;
        std::vector<std::vector<double>> vals;
        std::string dummy;

        while(std::getline(input,dummy)) {
            std::deque<std::string> tokens;
            split(dummy, ' ', std::back_inserter(tokens));
            xSet.insert(std::stod(tokens.front()));
            tokens.pop_front();
            ySet.insert(std::stod(tokens.front()));
            tokens.pop_front();
            zSet.insert(std::stod(tokens.front()));
            tokens.pop_front();
            std::vector<double> val;
            while(tokens.size()!=0) {
                val.push_back(std::stod(tokens.front())); 
                tokens.pop_front();
            }
            vals.push_back(val);
        }

        std::copy(xSet.begin(),xSet.end(),std::back_inserter(x));
        std::copy(ySet.begin(),ySet.end(),std::back_inserter(y));
        std::copy(zSet.begin(),zSet.end(),std::back_inserter(z));

//        std::vector<std::vector<double>> valsTranspose(vals[0].size(),std::vector<double>(vals.size()));
//        for(int i = 0; i < vals.size(); i++) {
//            for(int j = 0; j < vals[0].size(); j++) {
//                valsTranspose[j][i] = vals[i][j];
//            }
//        }

        for(unsigned int iGrid = 0; iGrid < vals[0].size(); ++iGrid) {
            Utility::Grid3D *g = new Utility::Grid3D(x,y,z);
            for(unsigned int iX = 0; iX < x.size(); ++iX) {
                for(unsigned int iY = 0; iY < y.size(); ++iY) {
                    for(unsigned int iZ = 0; iZ < z.size(); ++iZ) {
                        g -> AddPoint(iX,iY,iZ,vals[y.size()*z.size()*iX+z.size()*iY+iZ][iGrid]);
                    }
                }
                grid[iGrid] = g;
            }
        }

        return grid;
    }

    bool GridIO::FindGrid2D(Conv conv) {
        // Make the filename consistent with the load and save functions
        fname2D; 
        switch(conv) {
            case Conv::C1:
                fname2D = "ConvGrids/C1_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
            case Conv::C1P1:
                fname2D = "ConvGrids/C1P1_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
            case Conv::C1P1P1:
                fname2D = "ConvGrids/C1P1P1_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
            case Conv::C1P2:
                fname2D = "ConvGrids/C1P2_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
            case Conv::C2:
                fname2D = "ConvGrids/C2_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
            case Conv::C2P1:
                fname2D = "ConvGrids/C2P1_"+pdfName+"_"+std::to_string(iSet)+".txt";
                break;
        }

        struct stat buffer;
        return stat(fname2D.c_str(),&buffer) == 0;

    }

    void GridIO::SaveGrid2D(Grid2D grid) const {
        // Check if the grids directory exists, if it doesn't create it
        struct stat st = {0};
        if(stat("ConvGrids",&st) == -1) {
            mkdir("ConvGrids",0700);
        }

        std::ofstream output(fname2D.c_str());

        for(unsigned int iX = 0; iX < grid.at(1) -> GetXDim(); iX++) {
            double x = grid.at(1) -> GetX(iX);
            for(unsigned int iY = 0; iY < grid.at(1) -> GetYDim(); iY++) {
                double y = grid.at(1) -> GetY(iY);
                output << x << "  " << y << "  ";
                for(auto g : grid) {
                    output << g.second->FindPoint(iX,iY) << "  ";
                }
                output << std::endl;
            }
        }


        //char *errMsg = 0;
        //int rc = sqlite3_open(filename.c_str(),&db);
        //if(rc) {
        //    sqlite3_close(db);
        //    throw std::runtime_error("Convolution: Unable to obtain database to store grid");
        //}
        ////Create CXF SQL table
        //std::string sql = "CREATE TABLE CXF(ID INT PRIMARY KEY NOT NULL, PID INT NOT NULL, X REAL NOT NULL, Q REAL NOT NULL, CXF REAL NOT NULL)";
        //rc = sqlite3_exec(db,sql.c_str(),callback,0,&errMsg);
        //if(rc != SQLITE_OK) {
        //    sqlite3_free(errMsg);
        //    throw std::runtime_error("Convolution: Error in SQL execution");
        //}
    }

    Grid2D GridIO::LoadGrid2D() const {
        std::ifstream input(fname2D.c_str());

        std::vector<double> x, y;
        std::set<double> xSet, ySet;
        std::vector<std::vector<double>> vals(11);
        std::string dummy;

        double xLast = 0, yLast = 0;
        while(std::getline(input,dummy)) {
            std::deque<std::string> tokens;
            split(dummy, ' ', std::back_inserter(tokens));
            xSet.insert(std::stod(tokens.front()));
            tokens.pop_front();
            ySet.insert(std::stod(tokens.front()));
            tokens.pop_front();
            std::vector<double> val;
            for(int i = 0; i < 11; i++) {
                vals[i].push_back(std::stod(tokens.front())); 
                tokens.pop_front();
            }
        }

        std::copy(xSet.begin(),xSet.end(),std::back_inserter(x));
        std::copy(ySet.begin(),ySet.end(),std::back_inserter(y));

        Grid2D grid;
        for(unsigned int iGrid = 0; iGrid < vals.size(); iGrid++) {
            Utility::Grid2D *g = new Utility::Grid2D(x,y);
            for(unsigned int iX = 0; iX < x.size(); iX++) {
                for(unsigned int iY = 0; iY < y.size(); iY++) {
                    g -> AddPoint(iX,iY,vals[iGrid][iY+y.size()*iX]);
                }
            }
            grid[iGrid-5] = g;
        }

        return grid;
    }
}
