#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <vector>

#pragma pack(push, 1)
struct Edge
{
  uint32_t v0;
  uint32_t v1;
  unsigned char weight;
};
#pragma pack(pop)

void serialize(const char *input_path, const char *output_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());

  std::vector<Edge> edges;
  uint32_t v0, v1, w;
  while (in)
  {
    in >> v0;
    in >> v1;
    in >> w;
    if (in)
      edges.push_back({v0,v1,(unsigned char)w});
  }

  std::ofstream out(output_path, std::ios_base::binary);
  assert(out.is_open());
  size_t size = edges.size();
  out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
  out.write(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());
  out.close();
  in.close();
}

void deserialize(const char *input_path, const char *output_path)
{
  std::ifstream in(input_path);
  assert(in.is_open());

  size_t count = 0;
  std::vector<Edge> edges;
  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  edges.resize(count);
  in.read(reinterpret_cast<char*>(edges.data()), sizeof(Edge)*edges.size());
  in.close();

  std::ofstream out(output_path);
  assert(out.is_open());
  for (Edge &edge : edges)
    out << edge.v0 <<"\t"<< edge.v1 <<"\t"<< (uint32_t)edge.weight<<"\n";
  out.close();
  in.close();
}

int main(int argc, char **argv)
{
  assert(argc == 6);
  if (strcmp(argv[1],"-s") == 0)
    serialize(argv[3], argv[5]);
  else
    deserialize(argv[3], argv[5]);
}
