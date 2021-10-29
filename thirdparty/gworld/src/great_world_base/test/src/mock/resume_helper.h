#pragma once
#include <fstream>

#include "mock/log.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"

void SaveGlobal(uint8_t* buffer) {
  gw::mock::MockMemoryOStream os;
  os << (uint64_t)buffer;

  std::ofstream outfile;
  outfile.open("../cell_shm.dat");
  outfile << os.size() << os.str();
  outfile.close();

  printf("save =========== %lx,%p\n", (uint64_t)buffer, buffer);
}

uint8_t* LoadGlobal() {
  char space_data[1000];
  std::size_t space_data_size;
  std::ifstream infile;
  infile.open("../cell_shm.dat");
  infile >> space_data_size;
  infile.read(space_data, space_data_size);
  infile.close();

  gw::StringSlice ss(space_data, space_data_size);
  gw::mock::MockMemoryIStream is(ss);

  uint64_t addr;
  is >> addr;
  printf("load =========== %lx\n", addr);
  return reinterpret_cast<uint8_t*>(addr);
}