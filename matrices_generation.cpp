#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <vector>
#include <chrono>
#include <filesystem>

int main(int argc, char *argv[])
{
  srand(time(NULL));
  int size = 800;
  if (argc>=2)
    size = std::stoi(argv[1]);
  std::array<std::vector<double>, 3> matrices;
  std::array<std::string, 3> tags = {"A", "B", "C"};

  for (auto &mat : matrices)
    mat.resize(size*size);
  
  for (int i=0;i<2;i++)
  {
    for (auto &elem : matrices[i])
      elem = 20*((double)rand()/RAND_MAX) - 10; //elements in [-10, 10] range
  }

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
    {
      matrices[2][i*size+j] = 0;
      for (int k=0;k<size;k++)
      {
        matrices[2][i*size+j] += matrices[0][i*size+k]*matrices[1][k*size+j];
      }
    }
  }
  std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
  long t = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
  std::cout<<"Matrix mult took "<<t<<" us\n";

  for (int i=0;i<3;i++)
  {
    std::filesystem::create_directories(tags[i]+"/data");
    std::ofstream out(tags[i]+"/data/"+tags[i]);
    for (int j=0;j<size*size;j++)
    {
      if (std::abs(matrices[i][j]) > 1e-12)
      {
        out << tags[i] <<"\t"<< j/size <<"\t"<< j%size<<"\t"<<matrices[i][j];
        if (j != size*size-1)
          out<<"\n";
      }
    }
    out.close();

    std::ofstream out_size(tags[i]+"/size");
    out_size << size <<"\t"<< size;
    out_size.close();
  }
}