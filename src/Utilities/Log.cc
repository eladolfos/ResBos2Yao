#include "ResBos/Log.hh"

void FileLogPolicy::openStream(const std::string& name) {
    stream -> open(name.c_str(), std::ios_base::binary|std::ios_base::out);
    if(!stream -> is_open())
        throw std::runtime_error("LOGGER: Unable to open an output stream");
}

void FileLogPolicy::closeStream() {
    if(stream) stream -> close();
}

void FileLogPolicy::write(const std::string& msg) {
    (*stream) << msg << std::endl;
}

FileLogPolicy::~FileLogPolicy() {
    if(stream) closeStream();
}
