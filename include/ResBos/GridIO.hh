#ifndef GRIDIO_HH
#define GRIDIO_HH

#include <map>
#include <iomanip>
#include <iterator>
#include <vector>
#include <iostream>
#include <dirent.h>

#include "ResBos/Enums.hh"

namespace Utility {
    class Grid3D;
    class Grid2D;
}

typedef std::map<int,Utility::Grid3D*> Grid3D;
typedef std::map<int,Utility::Grid2D*> Grid2D;
namespace IO {

    class Directory {
        public:
            Directory(std::string dir) : dir(dir) {
                DIR *directory;
                struct dirent *pdir;
                directory = opendir(dir.c_str());
                while(pdir=readdir(directory)) {
                    std::string file = pdir->d_name;
                    if(file.size() < 5) continue;
                    if(file.compare(file.size()-5,5,".grid") == 0)
                        files.push_back(file);
                }
                closedir(directory);
            }
            void ls() {
                std::copy(files.begin(), files.end(),
                        std::ostream_iterator<std::string>(std::cout, "\n"));
            }
            std::vector<std::string> GetFiles() {return files;}
        private:
            std::string dir;
            std::vector<std::string> files;
    };

    class Settings;
    class GridIO {
        public:
            GridIO(const Settings&);
            bool FindGrid3D(GridType);
            Grid3D LoadGrid3D() const;
            void SaveGrid3D(Grid3D) const;
            bool FindGrid2D(Conv);
            Grid2D LoadGrid2D() const;
            void SaveGrid2D(Grid2D) const;
        private:
            bool CheckGrid(std::string grid, GridType mode) const;

            std::string processName, pdfName, beam;
            int iSet;
            double ecm;
            double C1,C2,C3;
            double muR, muF;
            std::string fname3D,fname2D;
            double qMin, qMax, yMin, yMax, qtMin, qtMax;
            int asymOrder, delSigOrder, AOrder, BOrder, COrder, HOrder;
            std::string scheme;
    };
}

#endif
